#include "stdafxClr.h"
#include "RxSinkRepeaterClr.h"
#include "AudioSink.h"

namespace XDft {

namespace impl {
#pragma managed(push, off)

    class RxAudioSinkRepeater : public XD::AudioSink
	{
    public:
		bool __stdcall AddMonoSoundFrames(const short *p, unsigned count) override
		{
            bool ret = false;
            for (auto q : m_sinks)
                ret |= q->AddMonoSoundFrames(p, count);
			return ret;
		}

		void __stdcall AudioComplete() override
		{
            for (auto p : m_sinks)
                p->AudioComplete();
		}

		void __stdcall ReleaseSink() override
		{
			delete this;
		}

        void AddSink(XD::AudioSink *p)
        {
            std::shared_ptr<XD::AudioSink> sp(p, [](XD::AudioSink*p) 
                {p->ReleaseSink(); }
            );
            m_sinks.push_back(sp);
        }
	private:
        std::vector<std::shared_ptr<XD::AudioSink>> m_sinks;
	};
#pragma managed(pop)
}

    RxSinkRepeater::RxSinkRepeater()
    {
    }
    RxSinkRepeater::~RxSinkRepeater()
    {
        this->!RxSinkRepeater();
    }
    RxSinkRepeater::!RxSinkRepeater()
    {
        delete m_impl;
        m_impl = 0;
    }
    System::IntPtr RxSinkRepeater::GetRealTimeRxSink()
    {   // caller takes over ownership
        auto ret = System::IntPtr(m_impl);
        m_impl = 0;
        return ret;
    }
    void RxSinkRepeater::AddSink(System::IntPtr v)
    {
        if (!m_impl)
                    m_impl = new impl::RxAudioSinkRepeater();
        m_impl->AddSink(reinterpret_cast<XD::AudioSink*>(v.ToPointer()));
    }

}

