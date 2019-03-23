#include "stdafx.h"
#include "Ft8Demod.h"
#include "Ft8DemodImpl.h"
#include "AudioSink.h"

namespace XDft { namespace impl {

	Ft8Demod::Ft8Demod()
	{	}

	Ft8Demod::~Ft8Demod()
	{	}

    void Ft8Demod::Create()
    {        m_impl = std::make_shared<Ft8DemodImpl>();    }

    bool Ft8Demod::IsValid()const { return static_cast<bool>(m_impl); }

	struct RxAudioSink : public XD::AudioSink
	{
		RxAudioSink(std::shared_ptr<Ft8DemodImpl> impl,
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
		std::weak_ptr<Ft8DemodImpl> m_weak;
		AudioCompleteFcn_t m_complete;
	};

	void *Ft8Demod::GetRxSink(const AudioCompleteFcn_t &complete)
	{
        /* The XDft8::AudioSink abstract interface enables
        ** multiple assemblies within the same XDft.dll-based
        ** process to talk to each other without any managed/unmanaged transitions
        */
		return new RxAudioSink(m_impl, complete);
	}

	void Ft8Demod::SetDiskDat(bool v) { m_impl->SetDiskDat(v); }
	bool Ft8Demod::AddMonoSoundFrames12KHz(const short *p, unsigned count)
	{		return m_impl->AddMonoSoundFrames12KHz(p, count);	}
	void Ft8Demod::Decode(const DecodeClientFcn_t &f, WsjtExe ex)const
	{		m_impl->Decode(f, ex);	}
    unsigned Ft8Demod::Clock(unsigned t, const DecodeClientFcn_t &f, WsjtExe ex, 
        bool &invokedDecode, int &cy)
    {       return m_impl->Clock(t, f, ex, invokedDecode, cy);    }
    unsigned Ft8Demod::GetSignalSpectrum(float *pSpectrum, int numPoints, float &powerDb)
    {   return m_impl->GetSignalSpectrum(pSpectrum, numPoints, powerDb);  }
    void Ft8Demod::SetAudioSamplesCallback(const AudioCbFcn_t&f, unsigned si, unsigned sc,
        void *AudioProcessor)
    {        m_impl->SetAudioSamplesCallback(f, si, sc, AudioProcessor);    }
    void Ft8Demod::Reset() { m_impl->Reset(); }
    void Ft8Demod::set_nfa(int v) { m_impl->set_nfa(v); }
    int Ft8Demod::get_nfa() { return     m_impl->get_nfa(); }
    void Ft8Demod::set_nfb(int v) { m_impl->set_nfb(v); }
    int Ft8Demod::get_nfb() { return     m_impl->get_nfb(); }
    void Ft8Demod::set_n2pass(int v) { m_impl->set_n2pass(v); }
    int Ft8Demod::get_n2pass() {   return     m_impl->get_n2pass();    }
    void Ft8Demod::set_ndepth(int v) { m_impl->set_ndepth(v); }
    int Ft8Demod::get_ndepth() { return     m_impl->get_ndepth(); }
    void Ft8Demod::set_lft8apon(bool v) { m_impl->set_lft8apon(v); }
    bool Ft8Demod::get_lft8apon() { return     m_impl->get_lft8apon(); }
    void Ft8Demod::set_nfqso(int v) { m_impl->set_nfqso(v); }
    int Ft8Demod::get_nfqso() { return     m_impl->get_nfqso(); }
    void Ft8Demod::set_nftx(int v) { m_impl->set_nftx(v); }
    int Ft8Demod::get_nftx() { return     m_impl->get_nftx(); }
    void Ft8Demod::set_nexp_decode(int v) { m_impl->set_nexp_decode(v); }
    int Ft8Demod::get_nexp_decode() { return     m_impl->get_nexp_decode(); }
    void Ft8Demod::set_mycall(const std::string &v) { m_impl->set_mycall(v); }
    std::string Ft8Demod::get_mycall() { return m_impl->get_mycall(); }
    void Ft8Demod::set_hiscall(const std::string &v) { m_impl->set_hiscall(v); }
    std::string Ft8Demod::get_hiscall() { return m_impl->get_hiscall(); }
}}