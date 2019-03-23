#pragma once
#include "WsjtSharedMemory.h"
namespace XDft {
    namespace impl {
        class WsjtSharedMemory;
    }
    public ref class WsjtSharedMemory
    {
    public:
        WsjtSharedMemory(System::String ^key, bool useJt9);
        ~WsjtSharedMemory();
        !WsjtSharedMemory();

        bool CreateWsjtSharedMem();
        property System::String ^ SharedMemoryKey { System::String ^ get(); }
        void Spy();	// for looking at what wsjtx is presenting its jt9.exe
        impl::WsjtSharedMemory GetImpl() {return *m_impl;}
    private:
        impl::WsjtSharedMemory *m_impl;
    };
}

