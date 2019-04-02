#include "stdafx.h"
#include <cstdlib>
#include "AudioSink.h"
#include "GeneratorImpl.h"
namespace XDft {
    namespace impl {
       namespace {
            const double TWO_PI = atan(1) * 8.;
            // why 48000Hz? Well, 12000 would work. Even 11025 (1/4 CD
            // speed) has the necessary integral number of samples per
            // the necessary msec timing intervals. So the answer is...
            // wsjt-x does it for the case its generating the fox
            // signal for fox&hound operation. It costs 4x more
            // to use it for one-output-at-a-time than 12KHz, but
            // 4x of something very small is still small enough for me.
            const unsigned AUDIO_SAMPLES_PER_SEC_OUT = 48000;
            const float GENERATOR_MAX_AMPLITUDE = 0.95f;
            const unsigned MSEC_SILENT_BEGIN_AND_END = 10;
            const unsigned MSEC_RAMPUPDOWN = 10;
            /* There is code below that assumes that the number
            ** of samples in AUDIO_SAMPLES_PER_SEC_OUT is an
            ** integral number of all the MSEC_xyz's in this file.
            ** In particular, that means that the values of 10msec above
            ** can be used for 44100Hz output, but NOT for 11025.
            ** ...however...the inaccuracies due to integer
            ** truncations for those features (silence at ends
            ** and ramp up/down rates) won't do much harm.
            ** On the other hand, do NOT attempt to generate FT8
            ** if 160msec is not an integral number of cycles
            ** of your AUDIO_SAMPLES_PER_SEC_OUT
            */
        }

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
            const double OFFSET_HZ_PER_ITONE = 12000. / 1920.;
            const unsigned MSEC_PER_SYMBOL = 160;
            // there is also a definiton of number of symbols, 79, ...but caller provides that.
        }

        static const unsigned TARGET_BUFFER_SAMPLE_COUNT = 0x1000;
        static const RaisedCosineProfile rampUpDown((AUDIO_SAMPLES_PER_SEC_OUT * MSEC_RAMPUPDOWN) / 1000);
        static const unsigned SAMPLES_PER_SYMBOL = (FT8::MSEC_PER_SYMBOL * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;

        void GeneratorImpl::Play(const std::vector<int>&itones, int baseFrequency, 
                std::shared_ptr<XD::AudioSink> outputSink)
        {
            unsigned numberOfSamples = static_cast<unsigned>(itones.size()) * FT8::MSEC_PER_SYMBOL * AUDIO_SAMPLES_PER_SEC_OUT / 1000;
            unsigned silentSamples = MSEC_SILENT_BEGIN_AND_END * AUDIO_SAMPLES_PER_SEC_OUT / 1000;

            double angle = 0; // radians, of course
            unsigned i = silentSamples;
            std::vector<short> audioSamples(std::max(TARGET_BUFFER_SAMPLE_COUNT, silentSamples));
            unsigned rampUpIdx = 0;
            unsigned rampDownIdx = numberOfSamples;
            for (unsigned itoneIdx = 0; itoneIdx < itones.size(); itoneIdx++)
            {
                double toneHz = baseFrequency;
                toneHz += itones[itoneIdx] * FT8::OFFSET_HZ_PER_ITONE;
                double deltaAngle = TWO_PI * toneHz / AUDIO_SAMPLES_PER_SEC_OUT;
                for (unsigned j = 0; j < SAMPLES_PER_SYMBOL; j++)
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
                    angle += deltaAngle;
                    if (angle >= TWO_PI)//sine function will do this too, but its cheaper if we help
                        angle -= TWO_PI;
                }
            }
            audioSamples.insert(audioSamples.end(), silentSamples, 0);
            outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
            outputSink->AudioComplete();
         }

        // output multiple concurrent tone sequences
        void GeneratorImpl::Play(const std::vector<Tone>&tones,
            std::shared_ptr<XD::AudioSink> outputSink)
        {
            struct PerSignal {
                PerSignal() : numberOfSamples(0),  angle(0), rampUpIdx(0), rampDownIdx(0), normalizedCoefficient(1.f)
                    ,itoneIdx(0), j(0)
                    // randomize start times up to 1/3 second late
                    ,silentSamples(rand() * AUDIO_SAMPLES_PER_SEC_OUT / (3 * RAND_MAX)) 
                {}
                unsigned numberOfSamples;
                unsigned silentSamples;
                double angle;
                unsigned rampUpIdx;
                unsigned rampDownIdx;
                double normalizedCoefficient;
                unsigned itoneIdx;
                unsigned j;
            };

            double coefTotal = 0;
            unsigned minSilentSamples(0x7ffffff); // large unsigned
            unsigned maxNumberOfSamples(0);
            unsigned maxToneLength(0);
            std::vector<PerSignal> signals(tones.size());
            for (unsigned i = 0; i < tones.size(); i++)
            {
                signals[i].numberOfSamples = static_cast<unsigned>(
                    tones[i].itone.size()) * (FT8::MSEC_PER_SYMBOL * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;
                signals[i].silentSamples += (MSEC_SILENT_BEGIN_AND_END * AUDIO_SAMPLES_PER_SEC_OUT) / 1000;
                if (signals[i].silentSamples < minSilentSamples)
                    minSilentSamples = signals[i].silentSamples;
                signals[i].rampDownIdx = signals[i].numberOfSamples;
                signals[i].normalizedCoefficient = tones[i].coefficient;
                if (tones[i].itone.size() > maxToneLength)
                    maxToneLength = tones[i].itone.size();
                coefTotal += fabs(tones[i].coefficient);
                if (signals[i].numberOfSamples > maxNumberOfSamples)
                    maxNumberOfSamples = signals[i].numberOfSamples;
            }

            /* back through the tone sequences to 
            ** adjust coefficients for unity gain and
            ** skip over the slientSamples
            */
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
                        double toneHz = tones[sig].frequency;
                        toneHz += tones[sig].itone[s.itoneIdx] * FT8::OFFSET_HZ_PER_ITONE;
                        double deltaAngle = TWO_PI * toneHz / AUDIO_SAMPLES_PER_SEC_OUT;
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
                            s.angle += deltaAngle;
                            if (s.angle >= TWO_PI)//sine function will do this too, but its cheaper if we help
                                s.angle -= TWO_PI;
                            s.j += 1;
                            if (s.j >= SAMPLES_PER_SYMBOL)
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
            audioSamples.insert(audioSamples.end(), (MSEC_SILENT_BEGIN_AND_END * AUDIO_SAMPLES_PER_SEC_OUT) / 1000, 0);
            outputSink->AddMonoSoundFrames(&audioSamples[0], static_cast<unsigned>(audioSamples.size()));
            outputSink->AudioComplete();
        }
    }
}
