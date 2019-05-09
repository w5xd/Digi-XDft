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

        class GeneratorContext
        {
        public:
             virtual ~GeneratorContext();
             static GeneratorContext *getFt8Context();
             static GeneratorContext *getFt4Context();
        };

       
        class GeneratorImpl
        {
        public:
            static void Play(GeneratorContext*,const std::vector<int>&itone, int baseFrequency, 
                std::shared_ptr<XD::AudioSink> outputSink);
            static void Play(GeneratorContext*,const std::vector<Tone>&tones, 
                std::shared_ptr<XD::AudioSink> outputSink);
        };

       
    }
}

