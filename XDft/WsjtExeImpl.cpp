#include "stdafx.h"
#include <sstream>
#include <fstream>
#include <shlobj_core.h>
#include <commons.h>
#include "WsjtExe.h"
#include "WsjtExeImpl.h"
#include "StringConvert.h"

/* Manage a subprocess that is linked to XDwsjtFT8.dll
** There are two subclasses of WsjtExeImplBase but
** only one, XDft8DemodExeImpl, is used. The other,
** Jt9ExeImpl, can control the jt9.exe that WSJT-X
** installs and uses. That seemed like a good idea:
** it saved having to build the FT8 decoding modules.
** But, as of this writing, those decoding modules
** compile into XDwsjtFT8.dll, which interfaced
** by our own XDft8Demo.exe
*/

namespace XDft { namespace impl {
    static const auto WAIT_AFTER_DECODE_DONE = std::chrono::milliseconds(100);

    WsjtExeImplBase::WsjtExeImplBase() : m_pi({})
        , m_hChildStd_IN_Rd(0)
        , m_hChildStd_IN_Wr(0)
        , m_hChildStd_OUT_Rd(0)
        , m_hChildStd_OUT_Wr(0)
        , m_stop(false)
        , m_running(false)
        , m_decodeNotifiedFinished(false)
	{}

	WsjtExeImplBase::~WsjtExeImplBase()
	{		CloseAllHandles();	}

    // win32 boiler plate to pipe to a child process
	bool WsjtExeImplBase::CreatePipes()
	{
		SECURITY_ATTRIBUTES saAttr; ZeroMemory(&saAttr, sizeof(saAttr));
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;

		// Create a pipe for the child process's STDOUT. 
        if (!CreatePipe(&m_hChildStd_OUT_Rd, &m_hChildStd_OUT_Wr, &saAttr, 0))
            throw std::runtime_error("Failed to create pipe for child STDOUT");;

		// Ensure the read handle to the pipe for STDOUT is not inherited.
		if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
			return false;

		// Create a pipe for the child process's STDIN. 
		if (!CreatePipe(&m_hChildStd_IN_Rd, &m_hChildStd_IN_Wr, &saAttr, 0))
            throw std::runtime_error("Failed to create pipe for child STDIN");;

		// Ensure the write handle to the pipe for STDIN is not inherited. 
		if (!SetHandleInformation(m_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
			return false;
		return true;
	}

    // start the subprocess executable running
	bool WsjtExeImplBase::CreateWsjtProcess(WsjtSharedMemory sm)
	{
		if (m_pi.hProcess)
			return false;  // cannot start if already running

		if (!CreatePipes())
			return false;

		STARTUPINFOW         si = { 0 };
		si.cb = sizeof(si);
        // we get both the child's stdout and stderr
        // on the same file handle. Can't tell one
        // from the other, but don't need to.
		si.hStdError = m_hChildStd_OUT_Wr;
		si.hStdOutput = m_hChildStd_OUT_Wr;
		si.hStdInput = m_hChildStd_IN_Rd;
		si.dwFlags |= STARTF_USESTDHANDLES;

		static const int MAX_STR(512);

		// other command line args...
		wchar_t folderPath[MAX_PATH];
		folderPath[0] = 0;
        if (FAILED(SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, SHGFP_TYPE_CURRENT, folderPath)))
            throw std::runtime_error("SHGetFolderPath failed");

		std::wstring cmd;
        cmd = L"\""; // open quote of path to exe
		cmd += m_binPath;
        if (!m_binPath.empty())
            cmd += L'/';
        cmd += m_exeName; // executable
        cmd += L"\" "; // close quote path to exe
		// shared memory key
        cmd += L"-s ";
		cmd += MultiByteToWide(sm.GetSharedMemoryKey());
		// -w fftw patience, -m 1 threads, -e path to exe's
        // Neither jt9.exe nor XDft8Demod use -m or -w or -e...but
        // WSJT-X sends them...
		cmd += L" -w 1 -m 1 -e \""; // trailing open quote
		cmd += m_binPath;
        m_appDataDir = folderPath;
        m_appDataDir += L'\\';
        m_appDataDir += m_appDataName;
		// path to data files
		cmd += L"\" -a \"";
		cmd += m_appDataDir;
        cmd += L"\"";
        if (!::CreateDirectoryW(m_appDataDir.c_str(), 0))
        {
            if (::GetLastError() != ERROR_ALREADY_EXISTS)
                throw std::runtime_error((std::string("Failed to create ") + WideToMultiByte(m_appDataDir)));
        }
        m_appDataDir += L'\\';
        std::wstring tempDir;
		tempDir += folderPath;
		tempDir += L"\\Temp\\";
        if (!::CreateDirectoryW(tempDir.c_str(), 0))
        {
            if (::GetLastError() != ERROR_ALREADY_EXISTS)
                throw std::runtime_error((std::string("Failed to create ") + WideToMultiByte(tempDir)));
        }
		tempDir += m_appDataName;
		// path to temp files
		cmd += L" -t \""; // with open quote
		cmd += tempDir;
        cmd += L'"'; // trailing quote
        if (!::CreateDirectoryW(tempDir.c_str(), 0))
        {
            if (::GetLastError() != ERROR_ALREADY_EXISTS)
                throw std::runtime_error((std::string("Failed to create ") + WideToMultiByte(tempDir)));
        }
        ProcessStarting(tempDir, sm);
        SetQuit(false);
        SetRunEnable(false);

		// cmdBuf must be WRITEABLE...per Win32 SDK
		wchar_t cmdBuf[MAX_STR];
		wcscpy_s(cmdBuf, cmd.c_str());
        if (!::CreateProcessW(NULL, cmdBuf, 0, 0, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
            0, m_binPath.c_str(), &si, &m_pi))
            throw std::runtime_error("Failed to create subprocess");
		DWORD wii = WaitForInputIdle(m_pi.hProcess, 1000);
		lock_t l(m_mutex);
		m_readThread = std::thread(std::bind(&WsjtExeImplBase::ChildStdOutReadThread, this));
		while (m_running == 0)
			m_cond.wait(l);
        m_sharedMemory = sm;
		return true;
	}

    // these two virtuals are implemented in the base 
    // because we want to CloseAllHandles from the destructor...
    // ...because we really do NOT want to allow those handles to leak.
    // Client SHOULD call ShutDown....but just in case, here we are:
    void WsjtExeImplBase::SetQuit(bool){}
    void WsjtExeImplBase::SetRunEnable(bool){}

    void WsjtExeImplBase::Shutdown()
    {        CloseAllHandles();   }

	void WsjtExeImplBase::CloseAllHandles()
	{
		BOOL ch;
		if (m_pi.hThread)
			ch = CloseHandle(m_pi.hThread);
		m_pi.hThread = 0;

		if (m_pi.hProcess)
		{
            SetQuit(true);
            SetRunEnable(true);
			if (WAIT_OBJECT_0 != WaitForSingleObject(m_pi.hProcess, 2000))
			    TerminateProcess(m_pi.hProcess, 0);
			ch = CloseHandle(m_pi.hProcess);
            SetQuit(false);
		}
		m_pi.hProcess = 0;

		if (m_hChildStd_IN_Rd)
		{
			::CancelIoEx(m_hChildStd_IN_Rd, 0);
			ch = CloseHandle(m_hChildStd_IN_Rd);
		}
		m_hChildStd_IN_Rd = 0;
		if (m_hChildStd_IN_Wr)
		{
			::CancelIoEx(m_hChildStd_IN_Wr, 0);
			ch = CloseHandle(m_hChildStd_IN_Wr);
		}
		m_hChildStd_IN_Wr = 0;
		if (m_hChildStd_OUT_Rd)
		{
			::CancelIoEx(m_hChildStd_OUT_Rd, 0);
			ch = CloseHandle(m_hChildStd_OUT_Rd);
		}
		m_hChildStd_OUT_Rd = 0;
		if (m_hChildStd_OUT_Wr)
		{
			::CancelIoEx(m_hChildStd_OUT_Wr, 0);
			ch = CloseHandle(m_hChildStd_OUT_Wr);
		}
		m_hChildStd_OUT_Wr = 0;

		{
			lock_t l(m_mutex);
			m_stop = true;
			m_cond.notify_all();
		}
		if (m_readThread.joinable())
			m_readThread.join();
	}

	void WsjtExeImplBase::ChildStdOutReadThread()
	{
		/*  thread to read the child process stdout 	*/
		{
			lock_t l(m_mutex);
			m_running += 1;
			m_cond.notify_all();
		}
		std::vector<char> readThreadBuf(1024);
		for (;;)
		{
			DWORD nbr;
			if (!ReadFile(m_hChildStd_OUT_Rd, &readThreadBuf[0], static_cast<int>(readThreadBuf.size()), &nbr, 0))
				break;
			if (nbr > 0)
			{
				lock_t l(m_mutex);
				if (m_stop)
					break;
				// concat the newly read characters
				m_readThreadBuf.insert(m_readThreadBuf.end(), readThreadBuf.begin(), readThreadBuf.begin() + nbr);
				bool done(false);
				while (m_decodeLineFcn && !done)
				{
					std::string s;
					done = true;
					for (unsigned i = 0; i < m_readThreadBuf.size(); i++)
					{
						if (m_readThreadBuf[i] == '\n')
						{
							m_readThreadBuf.erase(m_readThreadBuf.begin(), m_readThreadBuf.begin() + i + 1);
							done = false;
							if (!s.empty())
							{
								auto f = m_decodeLineFcn;
								l.unlock();
								if (f)
									f(s);
								l.lock();
								break;
							}
						}
						else if (m_readThreadBuf[i] == '\r')
						{/* silently swallow CR */}
						else
							s += m_readThreadBuf[i];
					}
				}
				m_cond.notify_all();
			}
		}
		{
			lock_t l(m_mutex);
			m_running -= 1;
			m_cond.notify_all();
		}
	}

     bool WsjtExeImplBase::DecodeInProgress() const
    {
        if (WAIT_OBJECT_0 == WaitForSingleObject(m_pi.hProcess, 0))
            throw std::runtime_error(WideToMultiByte(m_exeName) + " process has exited");
        lock_t l(m_mutex);
        if (static_cast<bool>(m_decodeLineFcn))
            return true;
        if (!m_decodeNotifiedFinished)
            return false;
        bool ret = std::chrono::steady_clock::now() - m_timeNotifiedFinished < WAIT_AFTER_DECODE_DONE;
        return ret;
    }
    
    void WsjtExeImplBase::SetDecodeLineFcn(const DecodeLineFcn_t &f)
	{
		lock_t l(m_mutex);
		m_decodeLineFcn = f;
        if (f)
            m_decodeNotifiedFinished = false;
	}

    void WsjtExeImplBase::DecodeFinished()
    {
        lock_t l(m_mutex);
        m_decodeNotifiedFinished = true;
        m_timeNotifiedFinished = std::chrono::steady_clock::now();
        m_decodeLineFcn = DecodeLineFcn_t();
    }
          
    //***********************************************************
    // XDft8DemodExeImpl
    // While jt9.exe is cross platform, XDft8 is not. So we use
    // Windows-specific shared event handles to coordinate between
    // parent and child
    XDft8DemodExeImpl::XDft8DemodExeImpl()
        : m_goEvent(INVALID_HANDLE_VALUE)
        , m_quitEvent(INVALID_HANDLE_VALUE)
    {
        static const wchar_t ExeName[] = L"XDftDemod.exe";
        m_exeName = ExeName;
        HMODULE hModule = 0;
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            ExeName,
            &hModule);
        std::vector<wchar_t> buf(MAX_PATH);
        auto charlen = GetModuleFileNameW(hModule, &buf[0], static_cast<DWORD>(buf.size()));
        m_binPath.assign(buf.begin(), buf.begin() + charlen);
        auto pos = m_binPath.rfind(L'\\');
        if (pos != m_binPath.npos)
            m_binPath.resize(pos);
    }
    XDft8DemodExeImpl::~XDft8DemodExeImpl()
    {        CloseHandles();    }

    void XDft8DemodExeImpl::CloseHandles()
    {
        if (m_goEvent != INVALID_HANDLE_VALUE)
            ::CloseHandle(m_goEvent);
        m_goEvent = INVALID_HANDLE_VALUE;
        if (m_quitEvent != INVALID_HANDLE_VALUE)
            ::CloseHandle(m_quitEvent);
        m_quitEvent = INVALID_HANDLE_VALUE;
    }

    void XDft8DemodExeImpl::ProcessStarting(const std::wstring &tempDir, WsjtSharedMemory sm)
    {
        CloseHandles(); // naming convention must match that used by XDft8Demo.exe
        std::string keyGo = sm.GetSharedMemoryKey() + "-go";
        m_goEvent = ::CreateEventA(0, FALSE, FALSE, keyGo.c_str());
        std::string keyQuit = sm.GetSharedMemoryKey() + "-quit";
        m_quitEvent = ::CreateEventA(0, TRUE, FALSE, keyQuit.c_str());
    }

    void XDft8DemodExeImpl::SetRunEnable(bool v)
    {   // the event is created a auto-reset...
        if (v)
            ::SetEvent(m_goEvent);
        else // ...so in our case this doesn't do much:
            ::ResetEvent(m_goEvent);
    }

    void XDft8DemodExeImpl::SetQuit(bool v)
    {
        if (v)
            ::SetEvent(m_quitEvent);
        else
            ::ResetEvent(m_quitEvent);
    }

    //***********************************************************
    // Jt9ExeImpl
    Jt9ExeImpl::Jt9ExeImpl()
    {  m_exeName = L"jt9.exe";  }

    void Jt9ExeImpl::ProcessStarting(const std::wstring &tempDir, WsjtSharedMemory sm)
    {
        m_lockFilePath = tempDir + L"\\.lock";
        m_quitFilePath = tempDir + L"\\.quit";
    }

    void Jt9ExeImpl::SetRunEnable(bool v)
    {
        if (!m_lockFilePath.empty())
        {
            if (v)
                ::DeleteFileW(m_lockFilePath.c_str());
            else
            {
                if (!std::wifstream(m_lockFilePath.c_str()).is_open())
                    std::wofstream lockFile(m_lockFilePath.c_str());
            }
        }
    }

    void Jt9ExeImpl::SetQuit(bool v)
    {
        if (!m_quitFilePath.empty())
        {
            if (v)
            {
                if (!std::wifstream(m_quitFilePath.c_str()).is_open())
                    std::wofstream lockFile(m_quitFilePath.c_str());
            }
            else
                ::DeleteFileW(m_quitFilePath.c_str());
        }
    }

}}