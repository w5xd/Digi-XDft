#include "stdafx.h"
#include "Ft8DemodImpl.h"
#include <commons.h>
#include <XDxmitFT8.h>
#include "StringConvert.h"
namespace XDft { namespace impl {
    // Ft8DemoImpl's main job is to pass parameters 
    // and waveform data
    // to the FT8 decoder via its common block
	Ft8DemodImpl::Ft8DemodImpl()
		: m_FortranData(new struct dec_data) 
		, m_TRperiod(15)
        , m_decSamplesWritten(0)
        , m_resetThisInterval(true)
        , m_timepointToTruncate({})
        , m_clipSoundToTimepoint(false)
        , m_lastCalledBackIndex(0)
        , m_sampleCbInterval(0)
        , m_sampleCbCount(0)
        , m_Ft8CycleNumber(-1)
		, m_audioSampleCbThreadStop(false)
	{ 
        memset(m_FortranData.get(), 0, sizeof(struct dec_data));
        m_FortranData->params.n2pass = 2;   // from spying
        m_FortranData->params.npts8 = 21600; // from spying
        m_FortranData->params.ndepth = 1;
        m_FortranData->params.lft8apon = false;
        m_FortranData->params.nzhsym = 50; // setup in mainwindow.cpp
        m_FortranData->params.nagain = false;
        memset(m_FortranData->params.mycall, ' ', sizeof(m_FortranData->params.mycall));
        memset(m_FortranData->params.hiscall, ' ', sizeof(m_FortranData->params.hiscall));
        memset(m_FortranData->params.mygrid, ' ', sizeof(m_FortranData->params.mygrid));
        memset(m_FortranData->params.hisgrid, ' ', sizeof(m_FortranData->params.hisgrid));
        memset(m_FortranData->params.datetime, ' ', sizeof(m_FortranData->params.datetime));
    }

	Ft8DemodImpl::~Ft8DemodImpl()
	{CancelCallbackThread();  }

	void Ft8DemodImpl::CancelCallbackThread()
	{
		lock_t l(m_mutex);
		if (!m_audioSampleCbThread.joinable())
			return;
		m_audioSampleCbThreadStop = true;
		m_audioSampleCbQueue.clear();
		m_cond.notify_all();
		l.unlock();
		m_audioSampleCbThread.join();
	}

    // We set up a native thread to host 
    // audio processing callbacks. The thread is to prevent
    // that callback process from interfering with our ability
    // to aquire audio from the sound card. The XDft8Test application
    // demonstrates how to set up such a callback, but does not
    // otherwise use it.
	void Ft8DemodImpl::AudioSampleCbThread()
	{
		for (;;)
		{
			std::function<void()> f;
			lock_t l(m_mutex);
			while (!m_audioSampleCbThreadStop && m_audioSampleCbQueue.empty())
				m_cond.wait(l);
			if (m_audioSampleCbThreadStop)
			{
				m_audioSampleCbThreadStop = false;
				break;
			}
			if (!m_audioSampleCbQueue.empty())
			{
				f = m_audioSampleCbQueue.front();
				m_audioSampleCbQueue.erase(m_audioSampleCbQueue.begin());
			}
			l.unlock();
			if (f)
				f();
		}
	}

     // the ft8 protocol calls for signals synchronized to
    // the global UTC clock. There are four 15-second intervals
    // in a UTC minute. A signal must begin at one of those
    // four, starting at 0 seconds, 15 seconds, 30s and 45s.
    // Reset() is called at that start. 
	void Ft8DemodImpl::Reset()
	{
        lock_t l(m_mutex);
        m_FortranData->params.kin = 0; // supposedly the number of audio samples available
		m_FortranData->params.nmode = 8;
		m_FortranData->params.napwid = 50;
        m_FortranData->params.newdat = false;
        m_FortranData->params.ntrperiod = m_TRperiod;
        // seems that the kin value in the common block is
        // ignored by the fortran ft8 decoder. 
        // That means it reads EVERYTHING in the
        // common block even if its old. So zero the data at the
        // beginning of the 15 seconds.
        memset(&m_FortranData->d2[0], 0,  sizeof(m_FortranData->d2));
        m_decSamplesWritten = 0;
        m_lastCalledBackIndex = 0;
	}

	void Ft8DemodImpl::SetDiskDat(bool v)
	{   // have no clue why ft9.exe cares, but this
        // bit says whether we're reading real time data
        // or a playback from disk
        lock_t l(m_mutex);
        m_FortranData->params.ndiskdat = v ? 1 : 0;
        m_FortranData->params.nutc = 0;
	}

    // client must call here periodically. We check the UTC
    // clock and synchronize the ft8 decoder to the 15 second interval.
    unsigned Ft8DemodImpl::Clock(unsigned tickToTriggerDecode, const DecodeClientFcn_t&f,
		WsjtExe jt9, bool &invokedDecode, int &cycle)
    {
        invokedDecode = false;
        SYSTEMTIME st;
        GetSystemTime(&st);
        unsigned ft8SecondNumber = st.wSecond % m_TRperiod;
        m_Ft8CycleNumber = st.wSecond / m_TRperiod;
        cycle = m_Ft8CycleNumber;
        if (ft8SecondNumber >= tickToTriggerDecode)
            invokedDecode = Decode(f, jt9);

        if (ft8SecondNumber == tickToTriggerDecode - 1)
        {   // the second before we trigger decode, reset the decoder...
            // ...should not be needed, but if it fails once, we'll never
            // invoke it again without this.
            jt9.SetRunEnable(false);
            jt9.SetDecodeLineFcn();
        }

        static const unsigned LISTEN_STARTUP_SECONDS = 1; // reset listen audio buffer this much early
        if ((ft8SecondNumber == m_TRperiod - LISTEN_STARTUP_SECONDS) && !m_resetThisInterval)
        {
            // Clock() is not called on any particular accurate time sequence.
            // Here we have noticed that we're in the final LISTEN_STARTUP_SECONDS of FT8's
            // 15 second sequence.
            //
            // setup m_timepointToTruncate to mark when we want the "real" recording to start.
            FILETIME ft;
            ::SystemTimeToFileTime(&st, &ft); // convert to FILETIME to do arithmetic
            ULARGE_INTEGER calc;
            calc.HighPart = ft.dwHighDateTime;
            calc.LowPart = ft.dwLowDateTime;
            static unsigned long long Nsec100PerSecond = 10000000ull;
            calc.QuadPart += LISTEN_STARTUP_SECONDS * Nsec100PerSecond; // go to FT8 startup second on the clock
            ft.dwHighDateTime = calc.HighPart;
            ft.dwLowDateTime = calc.LowPart;
            SYSTEMTIME toTruncate;
            ::FileTimeToSystemTime(&ft, &toTruncate);
            toTruncate.wMilliseconds = 0;   // take away the fraction to be on the exact second
            ::SystemTimeToFileTime(&toTruncate, &ft);
            // ft is now the exact second specified by FT8...but lets start the sound a little early.
            calc.HighPart = ft.dwHighDateTime;
            calc.LowPart = ft.dwLowDateTime;
            static const unsigned MSEC_BEFORE = 100;
            static const unsigned long long FtimePerMsec = 10000ull;
            calc.QuadPart -= MSEC_BEFORE * FtimePerMsec; // 
            m_timepointToTruncate = calc;
            m_clipSoundToTimepoint = true;

            // we have toTruncate set to the time want. use it to calculate nutc;
            int nutc = toTruncate.wHour;
            nutc *= 100;
            nutc += toTruncate.wMinute;
            nutc *= 100;
            nutc += toTruncate.wSecond;
            m_FortranData->params.nutc = nutc;

            Reset();  // FT8 signals in sync with the UTC clock
            m_resetThisInterval = true;
        }
        else
            m_resetThisInterval = false;

        return ft8SecondNumber;
    }

    unsigned Ft8DemodImpl::GetSignalSpectrum(float *pSpectrum, int numPoints, float &powerDb)
    {
        if (numPoints < NSMAX)
            throw std::runtime_error("GetSignalSpectrum requires at least 6827 points");
        // lots of undocumented magic required to call sysmspec...
        int ihsym(0);
        int nsps(6912);
        int inGain(0);
        int smoothYellow(0);
        float df3(0);
        int	npts8(0);
        float pxmax(0);

        lock_t l(m_mutex);
        int trmin = m_TRperiod / 60;
        int k = m_decSamplesWritten;
        if (k > 0)
        {
            std::unique_ptr<struct dec_data> copy(new struct dec_data(*m_FortranData));
            l.unlock();
            // note that the underlying FORTAN here is NOT re-entrant.
            // It "remembers" what k was every time you call and does stuff based
            // on that. Note that this code is linked directly into the XDft8.dll
            // assembly and does not benefit from the process separation that
            // hosts the ft8 decoder. Bottom line: if you want to run multiple
            // radios concurrent, don't call here from within a single OS process.
            symspec_(copy.get(), &k, &trmin, &nsps, &inGain, &smoothYellow,
                &powerDb, pSpectrum, &df3, &ihsym, &npts8, &pxmax);
        }
        return static_cast<unsigned>(k);
    }

    // wav file playback or real time audio calls here.
	bool Ft8DemodImpl::AddMonoSoundFrames12KHz(const short *p, unsigned nSamples)
	{
		static const unsigned SAMPLES_PER_SECOND = 12000;
		static const unsigned capacitySamples = sizeof(dec_data.d2) / sizeof(dec_data.d2[0]);
		lock_t l(m_mutex);
		unsigned availableSamples = capacitySamples - m_decSamplesWritten;
		bool retval = availableSamples >= nSamples;
		if (!retval)
			nSamples = availableSamples;
		if (availableSamples > 0)
		{
			// append incoming data into (local) common block
			memcpy(&m_FortranData->d2[m_decSamplesWritten], p, nSamples * sizeof(*p));
			m_decSamplesWritten += nSamples;
			if (m_clipSoundToTimepoint)
			{   // we're close to the begging of a 15 second interval. If we're past the
                // beginning, then truncate recently received audio to that exat time.
				FILETIME ftNow;
				GetSystemTimeAsFileTime(&ftNow);
				ULARGE_INTEGER ftTarget;
				ftTarget.HighPart = ftNow.dwHighDateTime;
				ftTarget.LowPart = ftNow.dwLowDateTime;
				if (ftTarget.QuadPart >= m_timepointToTruncate.QuadPart)
				{
					unsigned long long toRemain100nsec = ftTarget.QuadPart - m_timepointToTruncate.QuadPart;
					static const unsigned long long HUNDREDS_OF_NSEC_PER_SEC = 10000000;
					unsigned samplesToRemain = static_cast<unsigned>(
						toRemain100nsec * SAMPLES_PER_SECOND / HUNDREDS_OF_NSEC_PER_SEC);
					if (samplesToRemain < static_cast<unsigned>(m_decSamplesWritten))
					{
						if (samplesToRemain > 0)
							memmove(&m_FortranData->d2[0],
								&m_FortranData->d2[m_decSamplesWritten - samplesToRemain],
								samplesToRemain * sizeof(dec_data.d2[0]));
						m_decSamplesWritten = samplesToRemain;
						m_lastCalledBackIndex = 0;
					}
					m_clipSoundToTimepoint = false;
				}
			}
			m_FortranData->params.newdat = true;
			m_FortranData->params.kin = m_decSamplesWritten;
		}
		// queue any requested callbacks.
		auto cbFcn = m_audioSamplesCbFcn;
		auto nativeFcn = m_nativeAudioProcessor;
		if (nativeFcn || cbFcn)  // two variations on callback
		{	// both require calculating the next buffer size and position
			while (m_decSamplesWritten >= static_cast<int>(m_lastCalledBackIndex + m_sampleCbInterval))
			{
				int idxFirst = m_lastCalledBackIndex + m_sampleCbInterval;
				idxFirst -= m_sampleCbCount;
				if (idxFirst < 0)
					idxFirst = 0;
				m_lastCalledBackIndex += m_sampleCbInterval;
				unsigned cbLen = m_lastCalledBackIndex - idxFirst;
				// having a nativeFcn is one variant
				if (nativeFcn)
				{
					auto cbParam = std::make_shared<std::vector<short>>(cbLen);
					for (unsigned i = 0; i < cbLen; i++)
						cbParam->at(i) = m_FortranData->d2[i + idxFirst];
					m_audioSampleCbQueue.push_back(
						[nativeFcn, cbFcn, cbParam]()
						{
							const float *pForward = 0;
							unsigned numToForward = 0;
							if (nativeFcn->ProcessAudio(	//nativeFcn gets called...
								&cbParam->at(0), static_cast<unsigned>(cbParam->size()), &pForward, &numToForward) > 0)
							{
								if (cbFcn) // and if there is a cbFcn, call it too.
									cbFcn(pForward, numToForward);
							}
						});
				}
				else {	// having only a cbFcn is the other variant
					auto cbParameter = std::make_shared<std::vector<float>>(cbLen);
					for (unsigned i = 0; i < cbLen; i++)
						cbParameter->at(i) = static_cast<float>((1.0 / 0x7FFF) * m_FortranData->d2[i + idxFirst]);
					m_audioSampleCbQueue.push_back(
						[cbParameter, cbFcn]()
						{
							cbFcn(&cbParameter->at(0), static_cast<unsigned>(cbParameter->size()));
						});
				}
				m_cond.notify_all();
			}
		}
		return retval;
	}

    // invoke the FT8 decoder in the subprocess executable.
    // it responds with <DecoderFinished>
    // return early if the previous one hasn't finished.
	bool Ft8DemodImpl::Decode(const DecodeClientFcn_t&f, WsjtExe jt9)
	{
		if (!jt9.IsValid())
			return false;
        if (jt9.DecodeInProgress())
            return false;
        int cycleNumber = m_Ft8CycleNumber;
		jt9.SetDecodeLineFcn(
            [this, f, jt9, cycleNumber](const std::string &s)
		    {   // on stdout line from jt9, this lambda is called
			    if (s.find("<DecodeFinished>") != s.npos)
			    {
				    jt9.SetRunEnable(false);
				    if (f)
					    f("", cycleNumber);
                    jt9.DecodeFinished();
                }
			    else if (f)
				    f(s, cycleNumber);
		    });

        // from here to end is the magic incantation to get FT8 decoder to actually process the data
        {
            lock_t l(m_mutex);
            struct dec_data *commonBlock = jt9.GetSharedMemory().GetWsjtCommonBlock();
            memcpy(commonBlock, m_FortranData.get(), sizeof(struct dec_data));
            m_FortranData->params.newdat = false;
        }

		jt9.SetRunEnable(true);
        return true;
	}

    void Ft8DemodImpl::SetAudioSamplesCallback(const AudioCbFcn_t&f,
        unsigned sampleInterval, unsigned sampleCount,
        void *audioProcessor)
    {
		CancelCallbackThread();
		auto toReleaseOutsideLock = m_nativeAudioProcessor;
		{
            lock_t l(m_mutex);
			m_nativeAudioProcessor.reset();
			if (audioProcessor)
				m_nativeAudioProcessor =
					std::shared_ptr<XD::NativeAudioProcessor>(
						reinterpret_cast<XD::NativeAudioProcessor*>(audioProcessor),
						[](XD::NativeAudioProcessor *p) { p->ReleaseProcessor(); });
            m_audioSamplesCbFcn = f;
            m_sampleCbCount = sampleCount;
            m_sampleCbInterval = sampleInterval;
            // the thread startup is deferred to the point the client
            // does something that requires it...
			if (f || audioProcessor)
				m_audioSampleCbThread = std::thread(std::bind(&Ft8DemodImpl::AudioSampleCbThread, this));
        }
		toReleaseOutsideLock.reset();
        if (m_nativeAudioProcessor && m_nativeAudioProcessor->Initialize(m_FortranData.get()) <= 0)
            throw std::runtime_error("Initialize Audio Processor failed");
    }
	
    // we do NOT bother to lock our m_mutex below
    // under the assumption that the caller is going to use one (GUI-based) thread
    // to set all the decoding parameters.
    int Ft8DemodImpl::get_nfa()
    {        return m_FortranData->params.nfa;    }

    void Ft8DemodImpl::set_nfa(int v)
    {        m_FortranData->params.nfa = v;    }

    int Ft8DemodImpl::get_nfb()
    {        return m_FortranData->params.nfb;    }

    void Ft8DemodImpl::set_nfb(int v)
    {        m_FortranData->params.nfb = v;    }

    int Ft8DemodImpl::get_n2pass()
    {        return m_FortranData->params.n2pass;    }

    void Ft8DemodImpl::set_n2pass(int v)
    {        m_FortranData->params.n2pass = v;    }

    int Ft8DemodImpl::get_ndepth()
    {        return m_FortranData->params.ndepth;    }

    void Ft8DemodImpl::set_ndepth(int v)
    {        m_FortranData->params.ndepth = v;    }

    int Ft8DemodImpl::get_nfqso()
    {        return m_FortranData->params.nfqso;    }

    void Ft8DemodImpl::set_nfqso(int v)
    {        m_FortranData->params.nfqso = v;    }

    int Ft8DemodImpl::get_nftx()
    {        return m_FortranData->params.nftx;    }

    void Ft8DemodImpl::set_nftx(int v)
    {        m_FortranData->params.nftx = v;    }

    bool Ft8DemodImpl::get_lft8apon()
    {        return m_FortranData->params.lft8apon;    }

    void Ft8DemodImpl::set_lft8apon(bool v)
    {        m_FortranData->params.lft8apon = v;    }

    int Ft8DemodImpl::get_nexp_decode()
    {  return m_FortranData->params.nexp_decode; }

    void Ft8DemodImpl::set_nexp_decode(int v)
    {  m_FortranData->params.nexp_decode = v; }

    void Ft8DemodImpl::set_mycall(const std::string &v)
    {
        memset(m_FortranData->params.mycall, ' ', sizeof(m_FortranData->params.mycall));
        memcpy(m_FortranData->params.mycall, v.c_str(), 
            std::min(v.size(), sizeof(m_FortranData->params.mycall)));
    }

    std::string Ft8DemodImpl::get_mycall()
    {
        std::string ret;
        ret.assign(m_FortranData->params.mycall, 
            m_FortranData->params.mycall + sizeof(m_FortranData->params.mycall));
        while (!ret.empty() &&
            ((ret[ret.size()-1] == ' ') || ret[ret.size()-1] == 0))
            ret.resize(ret.size() - 1);
        return ret;
    }

    void Ft8DemodImpl::set_hiscall(const std::string &v)
    {
        memset(m_FortranData->params.hiscall, ' ', sizeof(m_FortranData->params.hiscall));
        memcpy(m_FortranData->params.hiscall, v.c_str(), 
            std::min(v.size(), sizeof(m_FortranData->params.hiscall)));
    }

    std::string Ft8DemodImpl::get_hiscall()
    {
        std::string ret;
        ret.assign(m_FortranData->params.hiscall, 
            m_FortranData->params.hiscall + sizeof(m_FortranData->params.hiscall));
        while (!ret.empty() &&
            ((ret[ret.size() - 1] == ' ') || ret[ret.size() - 1] == 0))
            ret.resize(ret.size() - 1);
        return ret;
    }
}}

