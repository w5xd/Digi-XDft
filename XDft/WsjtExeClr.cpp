#include "stdafxClr.h"
#include "WsjtExeClr.h"
#include "WsjtExe.h"
namespace XDft {
    WsjtExeBase::WsjtExeBase()
        : m_impl(0)
    {    }

    WsjtExeBase::~WsjtExeBase()
    {        this->!WsjtExeBase();    }

    WsjtExeBase::!WsjtExeBase()
    {
        if (m_impl)
            m_impl->Shutdown();
        delete m_impl;  // unmanaged resources
        m_impl = 0;
    }

    void WsjtExeBase::AppDataName::set(System::String^v) { m_impl->SetAppDataName(msclr::interop::marshal_as<std::wstring>(v)); }
    System::String^ WsjtExeBase::AppDataName::get() { return msclr::interop::marshal_as<System::String ^>(m_impl->GetAppDataName()); }
    System::String^ WsjtExeBase::AppDirectoryPath::get() { return msclr::interop::marshal_as<System::String ^>(m_impl->GetAppDataPath()); }

    bool WsjtExeBase::CreateWsjtProcess(WsjtSharedMemory ^v) {
        try {
            return m_impl->CreateWsjtProcess(v->GetImpl());
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    WsjtExe::WsjtExe()
    {
        m_impl = new impl::WsjtExe(impl::WsjtExe::ExeType_t::RUN_XDFT8DEMOD);
    }

    Jt9Exe::Jt9Exe()
    {
        m_impl = new impl::WsjtExe(impl::WsjtExe::ExeType_t::RUN_JT9);
    }

    void Jt9Exe::WsjtBinPath::set(System::String^v) { m_impl->SetBinPath(msclr::interop::marshal_as<std::wstring>(v)); }
    System::String^ Jt9Exe::WsjtBinPath::get() { return msclr::interop::marshal_as<System::String ^>(m_impl->GetBinPath()); }
    void Jt9Exe::ExeName::set(System::String^v) { m_impl->SetExeName(msclr::interop::marshal_as<std::wstring>(v)); }
    System::String^ Jt9Exe::ExeName::get() { return msclr::interop::marshal_as<System::String ^>(m_impl->GetExeName()); }

    WsjtExeBase::XDftDemodExitException::XDftDemodExitException(const std::string &msg) : System::Exception(gcnew System::String(msg.c_str()))
    { }
}