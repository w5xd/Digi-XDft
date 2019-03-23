#include "stdafx.h"
#include "WsjtSharedMemory.h"
#include "SharedMemoryImpl.h"
namespace XDft { namespace impl {

    WsjtSharedMemory::WsjtSharedMemory(const std::string &key, bool useJt9)
        : m_impl(std::make_shared<SharedMemoryImpl>(key, useJt9))
    {   }
    WsjtSharedMemory::WsjtSharedMemory() {}
    WsjtSharedMemory::~WsjtSharedMemory()
    { }

    std::string WsjtSharedMemory::GetSharedMemoryKey()const { return m_impl->GetSharedMemoryKey(); }
    void WsjtSharedMemory::Close() {     m_impl->Close();   }
    bool WsjtSharedMemory::CreateWsjtSharedMem() { return m_impl->CreateWsjtSharedMem(); }
    void WsjtSharedMemory::Spy() {        m_impl->Spy();    }
    struct dec_data *WsjtSharedMemory::GetWsjtCommonBlock() { return m_impl->GetWsjtCommonBlock(); }

}}