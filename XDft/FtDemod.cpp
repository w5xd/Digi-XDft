#include "stdafx.h"
#include "FtDemod.h"
#include "FtDemodImpl.h"
#include "AudioSink.h"

namespace XDft { namespace impl {

	FtDemod::FtDemod()
	{	}

	FtDemod::~FtDemod()
	{	}

    void FtDemod::Create()
    {        m_impl = std::make_shared<FtDemodImpl>();    }

    bool FtDemod::IsValid()const { return static_cast<bool>(m_impl); }

	struct RxAudioSink : public XD::AudioSink
	{
		RxAudioSink(std::shared_ptr<FtDemodImpl> impl,
			const AudioCompleteFcn_t &complete)
			: m_weak(impl)
			, m_complete(complete)
		{}

		bool __stdcall AddMonoSoundFrames(const short *p, unsigned count) override
		{
			auto demod = m_weak.lock();
			if (demod)
				return demod->AddMonoSoundFrames12KHz(p, count);
			return false;
		}

		void __stdcall AudioComplete() override
		{
			if (m_complete)
				m_complete();
		}

		void __stdcall ReleaseSink() override
		{
			delete this;
		}
	private:
		std::weak_ptr<FtDemodImpl> m_weak;
		AudioCompleteFcn_t m_complete;
	};

	void *FtDemod::GetRxSink(const AudioCompleteFcn_t &complete)
	{
        /* The XDft8::AudioSink abstract interface enables
        ** multiple assemblies within the same XDft.dll-based
        ** process to talk to each other without any managed/unmanaged transitions
        */
		return new RxAudioSink(m_impl, complete);
	}

	void FtDemod::SetDiskDat(bool v) { m_impl->SetDiskDat(v); }
	bool FtDemod::AddMonoSoundFrames12KHz(const short *p, unsigned count)
	{		return m_impl->AddMonoSoundFrames12KHz(p, count);	}
	void FtDemod::Decode(const DecodeClientFcn_t &f, WsjtExe ex, unsigned short atOffsetMsec)const
	{		m_impl->Decode(f, ex, m_impl->CycleNumber(), atOffsetMsec);	}
    unsigned FtDemod::Clock(unsigned t, const DecodeClientFcn_t &f, WsjtExe ex, 
        bool &invokedDecode, int &cy)
    {       return m_impl->Clock(t, f, ex, invokedDecode, cy);    }
    bool FtDemod::DecodeAgain(const DecodeClientFcn_t&f, WsjtExe w, int cycleNumber, unsigned short msecOffset)
    {   return m_impl->Decode(f, w, cycleNumber, msecOffset);}
    unsigned FtDemod::GetSignalSpectrum(float *pSpectrum, int numPoints, float &powerDb)
    {   return m_impl->GetSignalSpectrum(pSpectrum, numPoints, powerDb);  }
    void FtDemod::SetAudioSamplesCallback(const AudioCbFcn_t&f, unsigned si, unsigned sc,
        void *AudioProcessor)
    {        m_impl->SetAudioSamplesCallback(f, si, sc, AudioProcessor);    }
    void FtDemod::SetStartDecodeCallback(const StartDecodeCallback_t&f)
    {       m_impl->SetStartDecodeCallback(f);}
    void FtDemod::set_DemodPreZeroMsec(short v)
    {  return m_impl->set_DemodPreZeroMsec(v);   }
    short FtDemod::get_DemodPreZeroMsec()
    {        return m_impl->get_DemodPreZeroMsec();  }
     void FtDemod::set_DefaultDecodeShiftMsec(unsigned short v)
    {  return m_impl->set_DefaultDecodeShiftMsec(v);   }
    unsigned short FtDemod::get_DefaultDecodeShiftMsec()
    {        return m_impl->get_DefaultDecodeShiftMsec();  }
    void FtDemod::Reset() { m_impl->Reset(); }
    void FtDemod::set_nfa(int v) { m_impl->set_nfa(v); }
    int FtDemod::get_nfa() { return     m_impl->get_nfa(); }
    void FtDemod::set_nfb(int v) { m_impl->set_nfb(v); }
    int FtDemod::get_nfb() { return     m_impl->get_nfb(); }
    void FtDemod::set_n2pass(int v) { m_impl->set_n2pass(v); }
    int FtDemod::get_n2pass() {   return     m_impl->get_n2pass();    }
    void FtDemod::set_ndepth(int v) { m_impl->set_ndepth(v); }
    int FtDemod::get_ndepth() { return     m_impl->get_ndepth(); }
    void FtDemod::set_lft8apon(bool v) { m_impl->set_lft8apon(v); }
    bool FtDemod::get_lft8apon() { return     m_impl->get_lft8apon(); }
    void FtDemod::set_nfqso(int v) { m_impl->set_nfqso(v); }
    int FtDemod::get_nfqso() { return     m_impl->get_nfqso(); }
    void FtDemod::set_nftx(int v) { m_impl->set_nftx(v); }
    int FtDemod::get_nftx() { return     m_impl->get_nftx(); }
    void FtDemod::set_nexp_decode(int v) { m_impl->set_nexp_decode(v); }
    int FtDemod::get_nexp_decode() { return     m_impl->get_nexp_decode(); }
    void FtDemod::set_nQSOProgress(int v) { m_impl->set_nQSOProgress(v); }
    int FtDemod::get_nQSOProgress() { return     m_impl->get_nQSOProgress(); }
    void FtDemod::set_nzhsym(int v) { m_impl->set_nzhsym(v); }
    int FtDemod::get_nzhsym() { return     m_impl->get_nzhsym(); }
    void FtDemod::set_npts8(int v) { m_impl->set_npts8(v); }
    int FtDemod::get_npts8() { return     m_impl->get_npts8(); }
    void FtDemod::set_mycall(const std::string &v) { m_impl->set_mycall(v); }
    std::string FtDemod::get_mycall() { return m_impl->get_mycall(); }
    void FtDemod::set_hiscall(const std::string &v) { m_impl->set_hiscall(v); }
    std::string FtDemod::get_hiscall() { return m_impl->get_hiscall(); }
    void FtDemod::set_digiMode(DigiMode e) { return m_impl->set_digiMode(e); }
    DigiMode FtDemod::get_digiMode() { return m_impl->get_digiMode(); }
}}