#include "stdafx.h"
#include <cstdlib>
#include "AudioSink.h"
#include "GeneratorImpl.h"
#include <XDencode.h>
namespace XDft {
    namespace impl {
       namespace {
            const double TWO_PI = atan(1) * 8.;
            // why 48000Hz? Well,
            // ....FT4 has a sybol length of 2048 samples at 48000Hz which
            // is not an integral number of samples at 44100...so there..
            // Comments from when there was no FT4 mode follow.
            // For FT8, 12000 would work. Even 11025 (1/4 CD
            // speed) has the necessary integral number of samples per
            // the necessary msec timing intervals. So the answer is...
            // wsjt-x does it for the case its generating the fox
            // signal for fox&hound operation. It costs 4x more
            // to use it for one-output-at-a-time than 12KHz, but
            // 4x of something very small is still small enough for me.
            const unsigned AUDIO_SAMPLES_PER_SEC_OUT = 48000;
            const float GENERATOR_MAX_AMPLITUDE = 0.95f;
            const unsigned MSEC_SILENT_BEGIN = 10;
            const unsigned MSEC_SILENT_END = 1;
            const unsigned MSEC_RAMPUPDOWN = 20;
            const unsigned BEGINNING_SILENT_SAMPLES = MSEC_SILENT_BEGIN * AUDIO_SAMPLES_PER_SEC_OUT / 1000;
            const unsigned TRAILING_SILENT_SAMPLES = MSEC_SILENT_END * AUDIO_SAMPLES_PER_SEC_OUT / 1000;
        }

        class GeneratorContextImpl : public GeneratorContext
        {
        public:
            unsigned SamplesPerSymbol() { return m_samplesPerSymbol; }
            unsigned BeginningSilentSamples() { return m_beginningSilentSamples; }
            void initialize(unsigned short);
            void computeAngles(const std::vector<int>&itones, int baseFrequency, std::vector<double> &deltaAngles);
        protected:
            enum { PULSE_SYMBOL_WIDTH = 3 };
            GeneratorContextImpl();
            unsigned m_samplesPerSymbol;
            float m_pulseScale;
            unsigned m_beginningSilentSamples;
            std::vector<float> pulse;
        };

        class GeneratorContextFt8Impl : public GeneratorContextImpl
        {
        public:   GeneratorContextFt8Impl();
        };

        class GeneratorContextFt4Impl : public GeneratorContextImpl
        {
        public: GeneratorContextFt4Impl();
        };

       // ramp up and down without clicks using a raised cosine function.
        struct RaisedCosineProfile
        {
            RaisedCosineProfile(unsigned length)
                : profile(length)
            {
                for (unsigned i = 0; i < length; i++)
                {
                    double x = i * TWO_PI / ( 4 * length);
                    profile[i] = 1. - cos(x);
                }
            }
            unsigned length() const {
                return static_cast<unsigned>(profile.size());
            }
            double operator[](unsigned idx)const
            {   return profile[idx];  }
        private:
            std::vector<double> profile;
        };

        namespace FT8 {
            // definition of the on-the-air protocol
            const unsigned MSEC_PER_SYMBOL = 160;
            const unsigned SAMPLES_PER_SYMBOL = (MSEC_PER_SYMBOL * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;
        }

        namespace FT4 {
            // definition of the on-the-air protocol
            const unsigned SAMPLES_PER_SYMBOL = 4 * 576;
        }

        static const unsigned TARGET_BUFFER_SAMPLE_COUNT = 0x1000;
        static const RaisedCosineProfile rampUpDown((AUDIO_SAMPLES_PER_SEC_OUT * MSEC_RAMPUPDOWN) / 1000);

        void GeneratorImpl::Play(GeneratorContext*ctxp,const std::vector<int>&itones, int baseFrequency, 
                std::shared_ptr<XD::AudioSink> outputSink)
        {
            GeneratorContextImpl *ctx = reinterpret_cast<GeneratorContextImpl*>(ctxp);
            unsigned numberOfSamples = static_cast<unsigned>(itones.size()) * ctx->SamplesPerSymbol();
            unsigned i = ctx->BeginningSilentSamples();
            std::vector<short> audioSamples(std::max(TARGET_BUFFER_SAMPLE_COUNT, i));
            unsigned rampUpIdx = 0;
            unsigned rampDownIdx = numberOfSamples;
            std::vector<double> deltaAngle;
            ctx->computeAngles(itones, baseFrequency, deltaAngle);
            double angle = 0; // radians, of course
            unsigned angleIdx = 0;
            for (unsigned itoneIdx = 0; itoneIdx < itones.size(); itoneIdx++)
            {
                for (unsigned j = 0; j < ctx->SamplesPerSymbol(); j++)
                {
                    if (i >= audioSamples.size())
                    {
                        outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
                        i = 0;
                        audioSamples.resize(std::min(TARGET_BUFFER_SAMPLE_COUNT, numberOfSamples - rampUpIdx));
                    }
                    double sample = sin(angle) * GENERATOR_MAX_AMPLITUDE;
                    if (rampUpIdx < rampUpDown.length())
                        sample *= rampUpDown[rampUpIdx];
                    else if (rampDownIdx < rampUpDown.length())
                        sample *= rampUpDown[rampDownIdx];
                    audioSamples[i++] = static_cast<short>(sample * 0x7FFF);
                    rampUpIdx += 1;
                    rampDownIdx -= 1;
                    angle += deltaAngle[angleIdx++];
                    if (angle >= TWO_PI)//sine function will do this too, but its cheaper if we help
                        angle -= TWO_PI;
                }
            }
            audioSamples.insert(audioSamples.end(), TRAILING_SILENT_SAMPLES, 0);
            outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
            outputSink->AudioComplete();
         }

        // output multiple concurrent tone sequences
        void GeneratorImpl::Play(GeneratorContext*ctxp,const std::vector<Tone>&tones,
            std::shared_ptr<XD::AudioSink> outputSink)
        {
            struct PerSignal {
                PerSignal() : numberOfSamples(0),  angle(0), rampUpIdx(0), rampDownIdx(0), normalizedCoefficient(1.f)
                    ,itoneIdx(0), j(0) , angleIdx(0)
                    ,silentSamples(0) 
                {}
                void init(unsigned samplesPerSymbol, unsigned offset, unsigned short delayMsec)
                {   // randomize start times out to a symbol length late
                    silentSamples = offset;   
                    if (delayMsec == 0)
                        silentSamples += (rand() * samplesPerSymbol) / RAND_MAX;
                    else
                        silentSamples += (delayMsec * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;
                }
                unsigned numberOfSamples;
                unsigned silentSamples;
                double angle;
                std::vector<double> deltaAngle;
                unsigned angleIdx;
                unsigned rampUpIdx;
                unsigned rampDownIdx;
                double normalizedCoefficient;
                unsigned itoneIdx;
                unsigned j;
            };

            GeneratorContextImpl *ctx = reinterpret_cast<GeneratorContextImpl*>(ctxp);
            double coefTotal = 0;
            unsigned minSilentSamples(0x7ffffff); // large unsigned
            unsigned maxNumberOfSamples(0);
            unsigned maxToneLength(0);
            std::vector<PerSignal> signals(tones.size());
            for (unsigned i = 0; i < tones.size(); i++)
            {
                signals[i].init(ctx->SamplesPerSymbol(), ctx->BeginningSilentSamples(), tones[i].delayMsec);
                signals[i].numberOfSamples = static_cast<unsigned>(
                    tones[i].itone.size()) * ctx->SamplesPerSymbol();
                if (signals[i].silentSamples < minSilentSamples)
                    minSilentSamples = signals[i].silentSamples;
                signals[i].rampDownIdx = signals[i].numberOfSamples;
                signals[i].normalizedCoefficient = tones[i].coefficient;
                if (tones[i].itone.size() > maxToneLength)
                    maxToneLength = static_cast<unsigned>(tones[i].itone.size());
                coefTotal += fabs(tones[i].coefficient);
                if (signals[i].numberOfSamples > maxNumberOfSamples)
                    maxNumberOfSamples = signals[i].numberOfSamples;
                ctx->computeAngles(tones[i].itone, tones[i].frequency, signals[i].deltaAngle);
            }

            /* back through the tone sequences to 
            ** adjust coefficients for unity gain and
            ** skip over the slientSamples */
            int tonesInProgress = 0;
            double coefTotalRecip = 1. / coefTotal;
            for (unsigned i = 0; i < tones.size(); i++)
            {
                signals[i].normalizedCoefficient *= coefTotalRecip;
                signals[i].silentSamples -= minSilentSamples;
                if (!tones[i].itone.empty()) // empty is special case
                    tonesInProgress += 1;
            }

            std::vector<short> audioSamples(std::max(TARGET_BUFFER_SAMPLE_COUNT, minSilentSamples));
            unsigned i = minSilentSamples; // current index in output buffer
            while (tonesInProgress > 0)    // for each output sample
            {
                unsigned maxSamplesRemaining = 0;
                double thisSample = 0;
                for (unsigned sig = 0; sig < tones.size(); sig++) // for each signal
                {
                    PerSignal &s(signals[sig]);
                    if (s.itoneIdx >= tones[sig].itone.size())
                        continue; // this one is finished
                    if (s.silentSamples > 0)
                        s.silentSamples -= 1; // silence at beginning
                    else {
                        //  this is "like" the above: for (unsigned j = 0; j < SAMPLES_PER_SYMBOL; j++)
                        {
                            double sample = sin(s.angle) * GENERATOR_MAX_AMPLITUDE;
                            if (s.rampUpIdx < rampUpDown.length())
                                sample *= rampUpDown[s.rampUpIdx];
                            else if (s.rampDownIdx < rampUpDown.length())
                                sample *= rampUpDown[s.rampDownIdx];
                            thisSample += sample * s.normalizedCoefficient;
                            s.rampUpIdx += 1;
                            s.rampDownIdx -= 1;
                            s.angle += s.deltaAngle[s.angleIdx++];
                            if (s.angle >= TWO_PI)//sine function will do this too, but its cheaper if we help
                                s.angle -= TWO_PI;
                            s.j += 1;
                            if (s.j >= ctx->SamplesPerSymbol())
                            {
                                s.j = 0;
                                s.itoneIdx += 1;
                                if (s.itoneIdx >= tones[sig].itone.size())
                                    tonesInProgress -= 1;
                            }
                        }
                    }
                    int samplesRemaining = 
                        static_cast<int>(s.numberOfSamples + s.silentSamples) - 
                        static_cast<int>(s.rampUpIdx);
                    if (samplesRemaining > 0)
                        maxSamplesRemaining = std::max(maxSamplesRemaining, static_cast<unsigned>(samplesRemaining));
                }
                if (i >= audioSamples.size())
                {
                    outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
                    i = 0;
                    audioSamples.resize(std::min(TARGET_BUFFER_SAMPLE_COUNT, maxSamplesRemaining));
                    if (maxSamplesRemaining == 0)
                        break;
                }
                audioSamples[i++] = static_cast<short>(thisSample * 0x7FFF);
            }
            audioSamples.insert(audioSamples.end(), TRAILING_SILENT_SAMPLES, 0);
            outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
            outputSink->AudioComplete();
        }
    
        GeneratorContextImpl::GeneratorContextImpl() 
            : m_samplesPerSymbol(0)
            , m_pulseScale(0)
            , m_beginningSilentSamples(BEGINNING_SILENT_SAMPLES)
        {}

        GeneratorContext::~GeneratorContext() 
        {}

        GeneratorContext*GeneratorContext::getFt8Context(unsigned short msecSilent)
        {  
            auto ret = new GeneratorContextFt8Impl();
            ret->initialize(msecSilent);
            return ret;
        }

        GeneratorContext*GeneratorContext::getFt4Context(unsigned short msecSilent)
        { 
            auto ret =new GeneratorContextFt4Impl(); 
            ret->initialize(msecSilent);
            return ret;
        }

        void GeneratorContextImpl::initialize(unsigned short msecSilent)
        {
            pulse.resize(PULSE_SYMBOL_WIDTH * m_samplesPerSymbol);
            float samplesPerSymbolRecip = 1.f / m_samplesPerSymbol;
            for (unsigned i = 0; i < pulse.size(); i++)
            {
                float tt = static_cast<float>(i - PULSE_SYMBOL_WIDTH * 0.5f * m_samplesPerSymbol) * samplesPerSymbolRecip;
                pulse[i] = gfsk_pulse_(&m_pulseScale, &tt);
            }
            if (msecSilent != 0)
                m_beginningSilentSamples = (msecSilent * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;
         }

        void GeneratorContextImpl::computeAngles(const std::vector<int>&itones, int baseFrequency, std::vector<double> &deltaAngle)
        {
            if (itones.empty()) return;
            deltaAngle.resize(itones.size() * m_samplesPerSymbol);
#if 1   // convolve with a gaussian
            double deltaPeak = TWO_PI / m_samplesPerSymbol;
            for (unsigned j = 0; j < itones.size(); j++)
            {   // for each symbol. 
                unsigned begin = j * m_samplesPerSymbol;
                unsigned end = begin + PULSE_SYMBOL_WIDTH * m_samplesPerSymbol;
                unsigned m = 0;
                for (unsigned k = begin; k < end && k < deltaAngle.size(); k++) 
                    deltaAngle[k] += deltaPeak * pulse[m++] * itones[j];
            }
            for (unsigned jj = 0; jj < deltaAngle.size(); jj++)
                deltaAngle[jj] += (TWO_PI * baseFrequency) / AUDIO_SAMPLES_PER_SEC_OUT;
#else   // This is the "old" way as in wsjtx-2.0. abrupt frequency change at the tone boundary.
           double HzPerItone = static_cast<double>(AUDIO_SAMPLES_PER_SEC_OUT) / m_samplesPerSymbol;
           unsigned jj = 0;
           for (unsigned itoneIdx = 0; itoneIdx < itones.size(); itoneIdx++)
           {
               double toneHz = baseFrequency + itones[itoneIdx] * HzPerItone;
               double delta = TWO_PI * toneHz / AUDIO_SAMPLES_PER_SEC_OUT;
               for (unsigned j = 0; j < m_samplesPerSymbol; j++)
                    deltaAngle[jj++] = delta;
           }

#endif
        }

        GeneratorContextFt8Impl::GeneratorContextFt8Impl()
        {
            m_samplesPerSymbol = FT8::SAMPLES_PER_SYMBOL;
            m_pulseScale = 2.;
        }

        GeneratorContextFt4Impl::GeneratorContextFt4Impl()
        {
            m_samplesPerSymbol = FT4::SAMPLES_PER_SYMBOL;
            m_pulseScale = 1.;
        }
    }
}
