#include "stdafx.h"
#include "WsjtExe.h"
#include "WsjtExeImpl.h"
namespace XDft { namespace impl {

	WsjtExe::WsjtExe(ExeType_t which)
	{	
        switch (which)
        {
        case RUN_JT9:
            m_impl = std::make_shared< Jt9ExeImpl>();
            break;
        case RUN_XDFT8DEMOD:
            m_impl = std::make_shared< XDft8DemodExeImpl>();
            break;
        }
    }

    WsjtExe::WsjtExe()
    {  }

	WsjtExe::~WsjtExe()
	{	}

    bool WsjtExe::IsValid()const { return static_cast<bool>(m_impl); }
	void WsjtExe::SetBinPath(const std::wstring &v) { m_impl->SetBinPath(v); }
	std::wstring WsjtExe::GetBinPath() { return m_impl->GetBinPath(); }
	void WsjtExe::SetAppDataName(const std::wstring &v) { m_impl->SetAppDataName(v); }
	std::wstring WsjtExe::GetAppDataName() { return m_impl->GetAppDataName(); }
    std::wstring WsjtExe::GetAppDataPath() { return m_impl->GetAppDataPath(); }
    void WsjtExe::SetExeName(const std::wstring &v) { m_impl->SetExeName(v); }
    std::wstring WsjtExe::GetExeName() { return m_impl->GetExeName(); }
    bool WsjtExe::CreateWsjtProcess(WsjtSharedMemory sm) { return m_impl->CreateWsjtProcess(sm); }
	void WsjtExe::SetRunEnable(bool v) const { m_impl->SetRunEnable(v); }
	void WsjtExe::SetDecodeLineFcn(const DecodeLineFcn_t&f) const { m_impl->SetDecodeLineFcn(f); }
    bool WsjtExe::DecodeInProgress()const { return m_impl->DecodeInProgress();   }
    void WsjtExe::DecodeFinished()const { m_impl->DecodeFinished(); }
    WsjtSharedMemory WsjtExe::GetSharedMemory() { return m_impl->GetSharedMemory(); }
    void WsjtExe::Shutdown() { m_impl->Shutdown(); }

}}