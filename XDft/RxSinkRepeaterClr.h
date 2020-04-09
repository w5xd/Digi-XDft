#pragma once
namespace XDft {
    namespace impl {
        class RxAudioSinkRepeater; 
    }

    public ref class RxSinkRepeater
    {
    public:
        RxSinkRepeater();
        ~RxSinkRepeater();
        !RxSinkRepeater();

        System::IntPtr GetRealTimeRxSink();
        void AddSink(System::IntPtr);
    private:
        impl::RxAudioSinkRepeater *m_impl;
    };
}