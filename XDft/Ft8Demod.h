#pragma once
#include "WsjtExe.h"
namespace XDft {
    namespace impl {
    class Ft8DemodImpl;
    typedef std::function<void(const float *, unsigned)> AudioCbFcn_t;
	typedef std::function<void()> AudioCompleteFcn_t;
    typedef std::function<void(const std::string &, int)> DecodeClientFcn_t;
    class Ft8Demod
    {
    public:
        Ft8Demod();
        ~Ft8Demod();
        void Create();
        void SetDiskDat(bool);
        bool AddMonoSoundFrames12KHz(const short *p, unsigned count);

        void Decode(const DecodeClientFcn_t&, WsjtExe)const;
        unsigned Clock(unsigned tickToTrigger, const DecodeClientFcn_t&, WsjtExe, 
            bool &invokedDecode, int &cycleNumber);
        void SetAudioSamplesCallback(const AudioCbFcn_t&, unsigned sampleInterval, unsigned sampleCount,
            void *nativeProcessor);
        unsigned GetSignalSpectrum(float *pSpectrum, int numPoints, float &powerDb);
        void Reset();
		void *GetRxSink(const AudioCompleteFcn_t &completion=AudioCompleteFcn_t());

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
        void set_mycall(const std::string &);
        std::string get_mycall();
        void set_hiscall(const std::string &);
        std::string get_hiscall();

        bool IsValid()const;
    private:
        std::shared_ptr<Ft8DemodImpl> m_impl;
    };
}
}