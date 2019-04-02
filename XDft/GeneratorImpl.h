#pragma once
namespace XD {
    struct AudioSink;
}
namespace XDft {
    namespace impl {

        struct Tone {
            std::vector<int>itone;
            float coefficient;
            int frequency;
        };

        class GeneratorImpl
        {
        public:
            static void Play(const std::vector<int>&itone, int baseFrequency, 
                std::shared_ptr<XD::AudioSink> outputSink);
            static void Play(const std::vector<Tone>&tones, 
                std::shared_ptr<XD::AudioSink> outputSink);
        };
    }
}

