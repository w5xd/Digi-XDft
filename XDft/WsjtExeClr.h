#pragma once
#include "WsjtSharedMemoryClr.h"
#include "WsjtExe.h"
namespace XDft {
    namespace impl {
        class WsjtExe;
    }

    public ref class WsjtExeBase
    {
    public:
        ref class XDftDemodExitException : public System::Exception {
        public:
            XDftDemodExitException(const std::string &msg);
        };
        ~WsjtExeBase();
        !WsjtExeBase();

        property System::String^ AppDataName { System::String^get(); void set(System::String^); }
        property System::String^ AppDirectoryPath { System::String ^get(); }

        bool CreateWsjtProcess(WsjtSharedMemory ^WsjtSharedMemory);

        impl::WsjtExe GetImpl() { return *m_impl; };
    protected:
        WsjtExeBase();
        impl::WsjtExe *m_impl;
    };

    public ref class WsjtExe : public WsjtExeBase
    {
    public:
        WsjtExe();
    };

    public ref class Jt9Exe : public WsjtExeBase
    {
    public:
        Jt9Exe();
        property System::String^ WsjtBinPath { System::String^get(); void set(System::String^); }
        property System::String^ ExeName {System::String^get(); void set(System::String^); }
    };
}
