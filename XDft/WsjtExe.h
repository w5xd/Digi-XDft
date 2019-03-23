#pragma once
#include "WsjtSharedMemory.h"
namespace XDft {
    namespace impl {
        class WsjtExeImplBase;
        typedef std::function<void(const std::string&)> DecodeLineFcn_t;
        class WsjtExe
        {
        public:
            enum ExeType_t { RUN_JT9, RUN_XDFT8DEMOD };
            WsjtExe(ExeType_t);
            WsjtExe();
            ~WsjtExe();
            void SetBinPath(const std::wstring &v);
            std::wstring GetBinPath();
            void SetAppDataName(const std::wstring &);
            std::wstring GetAppDataName();
            std::wstring GetAppDataPath();
            void SetExeName(const std::wstring &);
            std::wstring GetExeName();
            bool CreateWsjtProcess(WsjtSharedMemory);
            WsjtSharedMemory GetSharedMemory();
            void SetRunEnable(bool)const;
            void SetDecodeLineFcn(const DecodeLineFcn_t& = DecodeLineFcn_t())const;
            void DecodeFinished() const;
            bool DecodeInProgress()const;
            void Shutdown();

            bool IsValid()const;
        private:
            std::shared_ptr<WsjtExeImplBase> m_impl;
        };
    }
}

