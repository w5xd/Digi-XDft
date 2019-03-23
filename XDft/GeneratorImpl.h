#pragma once
namespace XD {
    struct AudioSink;
}
namespace XDft {
    namespace impl {

        class GeneratorImpl
        {
        public:
            static void Play(const std::vector<int>&itone, int baseFrequency, 
                std::shared_ptr<XD::AudioSink> outputSink);
        };
    }
}

