#pragma once
namespace XDft {
    ref class WaveDeviceSink;

    public ref class Tone {
    public:
        Tone();
        Tone(array<int>^ tones, float coef, int frequency, unsigned short delayMsec);
        float coefficient;
        array<int>^itone;
        int frequency;
        unsigned short delayMsec;
    };

    namespace impl {
        class GeneratorContext;
    }

    public ref class GeneratorContext
    {
    public:
        // zero argument to msecSilent does not get zero. It gets default
        static GeneratorContext ^getFt8Context(unsigned short msecSilent);
        static GeneratorContext ^getFt4Context(unsigned short msecSilent);
        ~GeneratorContext();
        impl::GeneratorContext *getImpl() { return m_impl; }
    protected:
        GeneratorContext(impl::GeneratorContext*);
        impl::GeneratorContext *m_impl;
    };

    public ref class Generator
	{
	public:
		static void genft8(System::String ^msg, // to send
			System::String ^%msgSent,	// after encoding to 77 bits and decoding
			array<int> ^%itone,		// what to transmit
			array<bool> ^%ft8bits);	// info. 
		static void genft4(System::String ^msg, // to send
			System::String ^%msgSent,	// after encoding to 77 bits and decoding
			array<int> ^%itone,		// what to transmit
			array<bool> ^%ft4bits);	// info. 

        // ensure unpack77 has a hashed copy of our call
        static void setpack77mycall(System::String ^call);
        static void setpack77dxcall(System::String ^call);
        // read 77bit.txt in the wsjtx source to see the significance of i3 and n3
        static void pack77(System::String ^msg, int %i3, int %n3, array<bool>^%c77);
        static bool unpack77(array<bool>^c77, int forRx, System::String^%msg);
        static bool checkCall(System::String ^call, System::String ^%basecall);
        static bool pack28(System::String ^call, int %n28);
        static void Play(GeneratorContext ^context, array<int>^itone, int baseFrequency, System::IntPtr outputSink);
        static void Play(GeneratorContext ^context, array<Tone^> ^itones, System::IntPtr outputSink);

    protected:
	};
}

