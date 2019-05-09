#include "pch.h"
#include <commons.h>
#include <XDdecode.h>

/* XDftDemod. What is this?
** The underlying FORTRAN code in wsjtx for
** modulating and demodulating is not re-entrant.
** That means that any given process in the OS can only
** operate on one sound card (i.e. one radio) at a time.
** The way WSJT-X appears to plan to deal with this is
** to host the FORTRAN code in a child process such that
** it can have multiple child processes concurrently, one
** per radio. It currently does not have multiple child 
** processes and I'll leave it to the reader to guess why
** they don't have a complete solution.
**
** Regardless, we reproduce that design here. The 
** receive-side FT8 code is hosted in a child process
** and the application using the XDft8.dll assembly
** is welcome to instance multiple.
**
** Below is the main() for the child subprocess.
*/

class WinHandle
{
public:
    WinHandle() : m_Handle(INVALID_HANDLE_VALUE)
    {}
    ~WinHandle()
    {   close();    }
    WinHandle(const WinHandle&) = delete;
    operator HANDLE () const{   return m_Handle;  }
    HANDLE operator=(HANDLE v) {
        close();
        m_Handle = v; 
        return m_Handle;
    }
private:
    void close()
    {
        if (m_Handle != INVALID_HANDLE_VALUE)
            ::CloseHandle(m_Handle);
        m_Handle = INVALID_HANDLE_VALUE;
    }
    HANDLE m_Handle;
};

int main(int argc, char *argv[])
{
    std::string data_dir;
    std::string temp_dir;
    std::string sharedMemoryKey;

#ifdef _DEBUG
    ::MessageBoxA(0, "Debug from here", argc > 0? argv[0] : "", 0);
#endif

    // command line arguments match those used by jt9.exe
    // ...at least the ones we care about
    for (int i = 1; i < argc; i++)
    {
        if (strcmp("-a", argv[i]) == 0)
        {
            i += 1;
            if (i < argc)   // the ft8 decoder sprinkles files here
                data_dir = argv[i];
        }
        else if (strcmp("-t", argv[i]) == 0)
        {               
            i += 1;
            if (i < argc) // and the ft8 decoder sprinkles files here
                temp_dir = argv[i];
        }
        else if (strcmp("-s", argv[i]) == 0)
        {
            i += 1;
            if (i < argc)
                sharedMemoryKey = argv[i];
        }

    }

    struct dec_data *pFortanCommon = 0;
    std::unique_ptr<struct dec_data>pLifetime;
    WinHandle smem;
    WinHandle goEvent;
    WinHandle quitEvent;
    std::vector<HANDLE> events;
    if (!sharedMemoryKey.empty())
    {
        std::string smemName = sharedMemoryKey;
        smemName += "-memory";
        smem = ::OpenFileMappingA(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, smemName.c_str());
        if (smem == INVALID_HANDLE_VALUE)
        {
            std::cerr << "Failed to open shared memory " << smemName << std::endl;
            return 1;
        }
        auto pShare = ::MapViewOfFile(smem, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        if (pShare == 0)
        {
            std::cerr << "Failed to map shared memory " << smemName << std::endl;
            return 1;
        }
        MEMORY_BASIC_INFORMATION mbi = {};
        ::VirtualQuery(pShare, &mbi, sizeof(mbi));
        if (mbi.RegionSize < sizeof(*pFortanCommon))
        {
            std::cerr << "Shared memory too short " << mbi.RegionSize << " versus " << sizeof(*pFortanCommon)
                << std::endl;
            return 1;
        }
        pFortanCommon = reinterpret_cast<struct dec_data*>(pShare);

        std::string goEventName = sharedMemoryKey;
        goEventName += "-go";
        HANDLE ev = ::OpenEventA(SYNCHRONIZE, FALSE, goEventName.c_str());
        if (ev == 0)
        {
            auto error = GetLastError();
            std::cerr << "Failed to open go event " << goEventName << " error " << error << std::endl;
            return 1;
        }
        goEvent = ev;

        std::string quitEventName = sharedMemoryKey;
        quitEventName += "-quit";
        ev = ::OpenEventA(SYNCHRONIZE, FALSE, quitEventName.c_str());
        if (ev == 0)
        {
            std::cerr << "Failed to open quit event " << quitEventName << std::endl;
            return 1;
        }
        quitEvent = ev;
        events.push_back(quitEvent);
        events.push_back(goEvent);
   }
    else
    {   // when run without the command line args, do something
        // so that we can poke around in the debugger
        pLifetime.reset(new struct dec_data);
        pFortanCommon = pLifetime.get();
        memset(pFortanCommon, 0, sizeof(struct dec_data));
    }

    // one time init required
	xdinitfftw3_(data_dir.c_str());

    for (;;)
    {
        if (!events.empty())
        {
            auto waitResult = ::WaitForMultipleObjects(
                static_cast<DWORD>(events.size()), &events[0], FALSE, INFINITE);
            unsigned idx = waitResult - WAIT_OBJECT_0;
            if (idx == 0)
                break; // was the quit object
            if (idx != 1)
            {
                std::cerr << "Failed to wait for multiple objects" << std::endl;
                break; // this is an error. Either go or quit should have been signaled
            }
        }
        // here is where the magic happens..decodes and it writes to stdout
        xddecode_(&pFortanCommon->params, pFortanCommon->d2, temp_dir.c_str());
        if (events.empty())
            break; // when testing, we go through the loop only once
    }

    // one time post-process required
	xduninitfftw3_(data_dir.c_str());
}
