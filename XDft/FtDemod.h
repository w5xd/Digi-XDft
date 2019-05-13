#pragma once
#include "WsjtExe.h"
namespace XDft {
    namespace impl {
    class FtDemodImpl;
    typedef std::function<void(const float *, unsigned)> AudioCbFcn_t;
	typedef std::function<void()> AudioCompleteFcn_t;
    typedef std::function<void(const std::string &, int)> DecodeClientFcn_t;
    enum DigiMode { DIGI_FT8, DIGI_FT4 }; // Keep same as in DemodulatorClr.h
    class FtDemod
    {
    public:
        FtDemod();
        ~FtDemod();
        void Create();
        void SetDiskDat(bool);
        bool AddMonoSoundFrames12KHz(const short *p, unsigned count);

        void Decode(const DecodeClientFcn_t&, WsjtExe)const;
        unsigned Clock(unsigned tickToTrigger, const DecodeClientFcn_t&, WsjtExe, 
            bool &invokedDecode, int &cycleNumber);
        bool DecodeAgain(const DecodeClientFcn_t&, WsjtExe, int cycleNumber, unsigned short msecOffset);
        void SetAudioSamplesCallback(const AudioCbFcn_t&, unsigned sampleInterval, unsigned sampleCount,
            void *nativeProcessor);
        unsigned GetSignalSpectrum(float *pSpectrum, int numPoints, float &powerDb);
        void Reset();
		void *GetRxSink(const AudioCompleteFcn_t &completion=AudioCompleteFcn_t());
        void set_DemodPreZeroMsec(short);
        short get_DemodPreZeroMsec();
        void set_DefaultDecodeShiftMsec(unsigned short);
        unsigned short get_DefaultDecodeShiftMsec();

        void set_nfa(int);
        int get_nfa();
        void set_nfb(int);
        int get_nfb();
        void set_n2pass(int);
        int get_n2pass();
        void set_ndepth(int);
        int get_ndepth();
        void set_lft8apon(bool);
        bool get_lft8apon();
        void set_nfqso(int);
        int get_nfqso();
        void set_nftx(int);
        int get_nftx();
        void set_nexp_decode(int);
        int get_nexp_decode();
        void set_nQSOProgress(int);
        int get_nQSOProgress();
        int get_nzhsym();
        void set_nzhsym(int);
        int get_npts8();
        void set_npts8(int);
        void set_mycall(const std::string &);
        std::string get_mycall();
        void set_hiscall(const std::string &);
        std::string get_hiscall();
        DigiMode get_digiMode();
        void set_digiMode(DigiMode);

        bool IsValid()const;
    private:
        std::shared_ptr<FtDemodImpl> m_impl;
    };
}
}