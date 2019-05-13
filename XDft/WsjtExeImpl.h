#pragma once
#include "WsjtExe.h"
#include "WsjtSharedMemory.h"
namespace XDft {
    namespace impl {
        /* WsjtExeImpl
        ** class to manage the wsjt-x executable. Invoke it as a subprocess,
        ** connect to its stdin and stdout, map to its shared memory FORTRAN common
        ** block, terminate it when we do.
        */
        class WsjtExeImplBase
        {
        public:
            WsjtExeImplBase();
            virtual ~WsjtExeImplBase();

            // where to locate the wsjt-x install binaries
            void SetBinPath(const std::wstring &v) { m_binPath = v; }
            std::wstring GetBinPath() const { return m_binPath; }

            // name of directories in user's Windows AppData folder to use
            void SetAppDataName(const std::wstring &v) { m_appDataName = v; }
            std::wstring GetAppDataName() const { return m_appDataName; }
            std::wstring GetAppDataPath() const { return m_appDataDir; }

            // name of executable file to se
            void SetExeName(const std::wstring &v) { m_exeName = v; }
            std::wstring GetExeName() const { return m_exeName; }
            
            // start subprocess
            bool CreateWsjtProcess(WsjtSharedMemory);
            WsjtSharedMemory GetSharedMemory() const { return m_sharedMemory; }

            void SetDecodeLineFcn(const DecodeLineFcn_t&);
            virtual bool DecodeInProgress()const;
            virtual void SetRunEnable(bool);
            void DecodeFinished();
            void Shutdown();
        protected:
            typedef  std::unique_lock<std::mutex > lock_t;
            bool CreatePipes();
            void CloseAllHandles();
            void ChildStdOutReadThread();
            virtual void SetQuit(bool);
            virtual void ProcessStarting(const std::wstring &tempDir, WsjtSharedMemory sm)=0;

            bool m_stop;
            int m_running;
            PROCESS_INFORMATION m_pi;
            HANDLE m_hChildStd_IN_Rd;
            HANDLE m_hChildStd_IN_Wr;
            HANDLE m_hChildStd_OUT_Rd;
            HANDLE m_hChildStd_OUT_Wr;

            std::thread m_readThread;
            std::vector<char> m_readThreadBuf;
            mutable std::mutex m_mutex;
            std::condition_variable m_cond;

            std::wstring m_binPath;
            std::wstring m_appDataName;
            std::wstring m_appDataDir;
            std::wstring m_exeName;

            DecodeLineFcn_t m_decodeLineFcn;
            WsjtSharedMemory m_sharedMemory;

            bool m_decodeInProgress;
            bool m_decodeNotifiedFinished;
            std::chrono::steady_clock::time_point m_timeNotifiedFinished;
        };

        class Jt9ExeImpl : public WsjtExeImplBase
        {
        public:
            Jt9ExeImpl();
            void SetRunEnable(bool)override;
            bool DecodeInProgress()const override;
        protected:
            void ProcessStarting(const std::wstring &tempDir, WsjtSharedMemory sm) override;
            void SetQuit(bool)override;
            std::wstring m_lockFilePath;
            std::wstring m_quitFilePath;
        };

        class XDft8DemodExeImpl : public WsjtExeImplBase
        {
        public:
            XDft8DemodExeImpl();
            ~XDft8DemodExeImpl();
            void SetRunEnable(bool)override;
        protected:
            void ProcessStarting(const std::wstring &tempDir, WsjtSharedMemory sm) override;
            void SetQuit(bool)override;
            void CloseHandles();

            HANDLE m_goEvent;
            HANDLE m_quitEvent;
        };
    }
}