#include "stdafxClr.h"
#include "DemodulatorClr.h"
#include "WsjtExe.h"
#include "WsjtExeClr.h"
#include "FtDemod.h"

namespace XDft {

	Demodulator::Demodulator()
		: m_Ft8Demod(new impl::FtDemod())
	{
        m_Ft8Demod->Create();
    }

    Demodulator::~Demodulator()
    {        this->!Demodulator();    }

    Demodulator::!Demodulator()
    {
        delete m_Ft8Demod;
        m_Ft8Demod = 0;
    }

    void Demodulator::DemodulatorResultCallback::set(DemodResult^cb)
    {
        m_demodResult = cb;
    }

	System::IntPtr Demodulator::GetRealTimeRxSink()
	{
		m_Ft8Demod->SetDiskDat(false);
		m_Ft8Demod->Reset(); // clear out any prior audio data
		return System::IntPtr(m_Ft8Demod->GetRxSink()); // caller owns memory
	}

    DemodResult^ Demodulator::DemodulatorResultCallback::get()
    {
        return m_demodResult;
    }

    // forward a std function to a C++ clr delegate
    void ForwardDemod(const std::string &line, int cycleNumber, long long cbId)
    {
        DemodResult^ client =
            System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer<DemodResult^>
            (System::IntPtr(cbId));
        if (nullptr == client)
            return;
        try {
            client(msclr::interop::marshal_as<System::String ^>(line), cycleNumber);
        }
        catch (System::Exception ^)
        { /* ignore clr exceptions if client throws them*/
        }
    }

	void PlaybackFinalDecode(long long cbId, impl::WsjtExe wsjt, impl::FtDemod demod)
	{
        impl::DecodeClientFcn_t f =
            std::bind(&ForwardDemod, std::placeholders::_1, std::placeholders::_2, cbId );
        demod.Decode(f, wsjt);
	}

	System::IntPtr Demodulator::Playback(WsjtExeBase ^wsjt)
	{
        try {
            if (!m_Ft8Demod->IsValid())
                return System::IntPtr(0);
            // send the time data to FtDemod
            if (nullptr == wsjt)
				return System::IntPtr(0);
			wsjt->GetImpl().SetDecodeLineFcn();
            wsjt->GetImpl().SetRunEnable(false);
			m_Ft8Demod->SetDiskDat(true);
            m_Ft8Demod->Reset(); // clear saved audio
			impl::AudioCompleteFcn_t completeFcn;	
            if (m_demodResult != nullptr)
            {
                // decode once at the end of the wav file...
				completeFcn = std::bind(&PlaybackFinalDecode, 
                    System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate <DemodResult^>(m_demodResult).ToInt64(),
                    wsjt->GetImpl(), *m_Ft8Demod);
            }
            return System::IntPtr(m_Ft8Demod->GetRxSink(completeFcn)); // caller owns memory
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
	}

    // forward a std function to a C++ clr delegate
    static void ForwardAudio(const float *paudio, unsigned count, long long cbId)
    {
        AudioCallback^ client =
            System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer<AudioCallback^>
            (System::IntPtr(cbId));
        if (nullptr == client)
            return;
		array<float> ^ar = nullptr;
		if (count > 0)
		{
			ar = gcnew array<float>(static_cast<int>(count));
            pin_ptr<float> pnative = &ar[0];
            for (unsigned i = 0; i < count; i++)
                pnative[i] = *paudio++;
        }
        client(ar);
    }

    void Demodulator::SetAudioSamplesCallback(AudioCallback^cb, unsigned sampleInterval, unsigned sampleCount,
        System::IntPtr nativeProcessor)
    {
        m_audioSamplesCallback = cb;
        impl::AudioCbFcn_t f;
        if (nullptr != cb)
            f = std::bind(&ForwardAudio, std::placeholders::_1, std::placeholders::_2,
                System::Runtime::InteropServices::Marshal::
                GetFunctionPointerForDelegate <AudioCallback^>(cb).ToInt64());
        try {
            m_Ft8Demod->SetAudioSamplesCallback(f, sampleInterval, sampleCount, nativeProcessor.ToPointer());
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    unsigned Demodulator::Clock(unsigned tick, WsjtExeBase^wsjt, 
        bool %invokedDecode, int %cycleNumber)
    {
        try 
        {
            if (m_Ft8Demod->IsValid())
            {
                impl::DecodeClientFcn_t f;
                if (nullptr != m_demodResult)
                    f = std::bind(&ForwardDemod, std::placeholders::_1, std::placeholders::_2,
                        System::Runtime::InteropServices::Marshal::
                        GetFunctionPointerForDelegate <DemodResult^>(m_demodResult).ToInt64());
                bool didDecode = false; int cycle = 0;
                unsigned ret = m_Ft8Demod->Clock(tick, f, wsjt->GetImpl(), didDecode, cycle);
                invokedDecode = didDecode;
                cycleNumber = cycle;
                return ret;
            }
            return 15;
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    bool Demodulator::DecodeAgain(WsjtExeBase ^wsjt, int cycleNumber, unsigned short msecOffset)
    {
        try 
        {
            if (m_Ft8Demod->IsValid())
            {
                impl::DecodeClientFcn_t f;
                if (nullptr != m_demodResult)
                    f = std::bind(&ForwardDemod, std::placeholders::_1, std::placeholders::_2,
                        System::Runtime::InteropServices::Marshal::
                        GetFunctionPointerForDelegate <DemodResult^>(m_demodResult).ToInt64());
                return m_Ft8Demod->DecodeAgain(f, wsjt->GetImpl(), cycleNumber, msecOffset);
            }
            return false;
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }    
    }

    unsigned Demodulator::GetSignalSpectrum(array<float> ^spectrum, float %powerMeter)
    {
        if (nullptr == spectrum)
            throw gcnew System::Exception(L"spectrum parameter may not be null");
        try {
            float power;
            cli::pin_ptr<float> pSpectrum = &spectrum[0];
            unsigned ret = m_Ft8Demod->GetSignalSpectrum(pSpectrum, spectrum->Length, power);
            powerMeter = power;
            return ret;
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    short Demodulator::DemodulateSoundPreUtcZeroMsec::get()
    {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_DemodPreZeroMsec();
        return 0;
    }

    void Demodulator::DemodulateSoundPreUtcZeroMsec::set(short v)
    {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_DemodPreZeroMsec(v);
    }

    unsigned short Demodulator::DemodulateDefaultSoundShiftMsec::get()
    {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_DefaultDecodeShiftMsec();
        return 0;
    }

    void Demodulator::DemodulateDefaultSoundShiftMsec::set(unsigned short v)
    {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_DefaultDecodeShiftMsec(v);
    }
    int Demodulator::nfa::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nfa();
        return -1;
    }

    void Demodulator::nfa::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nfa(v);
    }

    int Demodulator::nfb::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nfb();
        return -1;
    }

    void Demodulator::nfb::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nfb(v);
    }

    int Demodulator::n2pass::get()   {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_n2pass();    
        return -1;
    }

    void Demodulator::n2pass::set(int v) { 
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_n2pass(v);    
    }

    int Demodulator::ndepth::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_ndepth();
        return -1;
    }

    void Demodulator::ndepth::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_ndepth(v);
    }

    int Demodulator::nfqso::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nfqso();
        return -1;
    }

    void Demodulator::nfqso::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nfqso(v);
    }

    int Demodulator::nftx::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nftx();
        return -1;
    }

    void Demodulator::nftx::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nftx(v);
    }

    bool Demodulator::lft8apon::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_lft8apon();
        return false;
    }

    void Demodulator::lft8apon::set(bool v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_lft8apon(v);
    }

    int Demodulator::nexp_decode::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nexp_decode();
        return false;
    }

    void Demodulator::nexp_decode::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nexp_decode(v);
    }
 
    int Demodulator::nQSOProgress::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nQSOProgress();
        return false;
    }

    void Demodulator::nQSOProgress::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nQSOProgress(v);
    }

   int Demodulator::nzhsym::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_nzhsym();
        return false;
    }

    void Demodulator::nzhsym::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_nzhsym(v);
    }

   int Demodulator::npts8::get() {
        if (m_Ft8Demod->IsValid())
            return m_Ft8Demod->get_npts8();
        return false;
    }

    void Demodulator::npts8::set(int v) {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_npts8(v);
    }

    System::String^ Demodulator::mycall::get() {
        if (m_Ft8Demod->IsValid())
            return msclr::interop::marshal_as<System::String ^>(m_Ft8Demod->get_mycall());
        return nullptr;
    }

    void Demodulator::mycall::set(System::String ^v) {
        if (m_Ft8Demod->IsValid())
        {
            std::string arg;
            if (v != nullptr)
                arg = msclr::interop::marshal_as<std::string>(v);
            m_Ft8Demod->set_mycall(arg);
        }
    }

    System::String^ Demodulator::hiscall::get() {
        if (m_Ft8Demod->IsValid())
            return msclr::interop::marshal_as<System::String ^>(m_Ft8Demod->get_hiscall());
        return nullptr;
    }

    void Demodulator::hiscall::set(System::String ^v) {
        if (m_Ft8Demod->IsValid())
        {
            std::string arg;
            if (v != nullptr)
                arg = msclr::interop::marshal_as<std::string>(v);
            m_Ft8Demod->set_hiscall(arg);
        }
    }

    DigiMode Demodulator::digiMode::get()
    {
        if (m_Ft8Demod->IsValid())
            return static_cast<DigiMode>(static_cast<int>(m_Ft8Demod->get_digiMode()));
        return DigiMode::DIGI_FT8;
    }

    void Demodulator::digiMode::set(DigiMode e)
    {
        if (m_Ft8Demod->IsValid())
            m_Ft8Demod->set_digiMode(static_cast<impl::DigiMode>(static_cast<int>(e)));
    }

}

