#include "stdafxClr.h"
#include "WsjtSharedMemoryClr.h"
#include "WsjtSharedMemory.h"
namespace XDft {

    WsjtSharedMemory::WsjtSharedMemory(System::String ^key, bool useJt9)
        : m_impl(new impl::WsjtSharedMemory(msclr::interop::marshal_as<std::string>(key),
            useJt9))
    {    }

    WsjtSharedMemory::~WsjtSharedMemory()
    {
        this->!WsjtSharedMemory();
    }
    WsjtSharedMemory::!WsjtSharedMemory()
    {
        delete m_impl;
        m_impl = 0;
    }

    System::String ^WsjtSharedMemory::SharedMemoryKey::get() {
        return msclr::interop::marshal_as<System::String^>(m_impl->GetSharedMemoryKey());
    }
    bool WsjtSharedMemory::CreateWsjtSharedMem() { return m_impl->CreateWsjtSharedMem(); };
    void WsjtSharedMemory::Spy() {        m_impl->Spy();    }
}
