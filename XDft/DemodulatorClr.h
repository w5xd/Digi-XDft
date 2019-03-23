#pragma once
namespace XDft {
    namespace impl {
        class WsjtExe;
        class Ft8Demod;
    }
    ref class WsjtExeBase;
    
    // The wsjtx-2.0.0 FT8 demodulator has its own scheme for presenting a string
    // from incoming FT8. That string is presented here.
    // cycleNumber is 0 through 3, else -1 for wav file playback
	public delegate void DemodResult(System::String^ stdoutLine, int cycleNumber);
	// regarding DemodResult
	// There is NO guarantee of any callback (cuz the jt9.exe might or might not
	// send anything to its stdout). 
	// An empty string on callback indicates that the decoder is finished with
	// a decoding of the audio it was sent.

    // The FT8 spectrum calculator cannot be multi-instanced in a process
    // because of its use of a FORTRAN common. XDft8 presents
    // a for an alternative implementation. The end result is an AudioCallback
    public delegate void AudioCallback(array<float>^);

    // class Demodulator. accepts audio input from either a device or a file and 
    // sends it to the wsjtx ft8 decoder.
	public ref class Demodulator
	{
	public:
        Demodulator();
        ~Demodulator();
        !Demodulator();

		// Playback is for playing back a file.
		// GetRealTimeRxSink is for real time decoding of an audio stream.
		// It is up to the caller to only Decode one stream at a time...
		// and to pass the IntPtr to a native RxAudioSink client calls ReleaseSink.
		System::IntPtr Playback(WsjtExeBase ^decoderExe);
		System::IntPtr GetRealTimeRxSink();
		
		// During wave file reading, call here to trigger the ft8 decoder.
        // call at least once per second. Returns the second number for
        // the ft8 receiver (0 through 14. 15 or higher on error)
        unsigned Clock(unsigned tickToTriggerDecode, 
            WsjtExeBase^wsjtExe, 
            bool %invokedDecode,
            int %cycleNumber);

        /* This call uses a FORTRAN common block in the wsjtx-2.0.0 code. 
        ** It is not reentrant, which means you may only call this function from ONE instance
        ** of a Demodulator. 
        ** Must call in with at least 6827 points in spectrum. returns number of points used to calculate it
        ** Those 6827 points are the amplitude across zero Hz (at sample 0) to 5000Hz (at sample 6826) */
        unsigned GetSignalSpectrum(array<float> ^spectrum, float %powerMeterDB);

        /* callback with unprocessed audio samples (about) every sampleInterval giving sampleCount samples
        ** sampleInterval. Get call every sampleInterval on input data stream.
        ** sampleCount. Call carries this number of samples. may be equal, or larger or 
		** smaller than sampleInterval.       */
        void SetAudioSamplesCallback(AudioCallback^callback, 
            unsigned sampleInterval, 
            unsigned sampleCount,
            System::IntPtr nativeProcessor // NativeAudioProcessor*. Caller accepts responsibity 
			// to free the pointer with a call to ReleaseProcessor
        );

        property DemodResult^ DemodulatorResultCallback { DemodResult^ get(); void set(DemodResult^); }
        
        // parameters in FORTRAN common block that appear to affect FT8 decoding. names in wsjtx source
        property int nfa { int get(); void set(int); } // low frequency Hz limit
        property int nfb { int get(); void set(int); }  // high frequency Hz limit
        property int n2pass { int get(); void set(int); }
        property int ndepth { int get(); void set(int); }
        property int nfqso { int get(); void set(int); }
        property int nftx { int get(); void set(int); }
        property bool lft8apon { bool get(); void set(bool); }
        property int nexp_decode { int get(); void set(int); }
        property System::String^ mycall {System::String^get(); void set(System::String^); }
        property System::String^ hiscall {System::String^get(); void set(System::String^); }


	private:
        impl::Ft8Demod *m_Ft8Demod;
        DemodResult^ m_demodResult;
        AudioCallback^ m_audioSamplesCallback;
	};
}
