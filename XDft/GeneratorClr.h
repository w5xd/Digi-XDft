#pragma once
namespace XDft {
    ref class WaveDeviceSink;

    public ref class Generator
	{
	public:
		static void genft8(System::String ^msg, // to send
			System::String ^%msgSent,	// after encoding to 77 bits and decoding
			array<int> ^%itone,		// what to transmit
			array<bool> ^%ft8bits);	// info. 

        // ensure unpack77 has a hashed copy of our call
        static void setpack77mycall(System::String ^call);
        static void setpack77dxcall(System::String ^call);
        // read 77bit.txt in the wsjtx source to see the significance of i3 and n3
        static void pack77(System::String ^msg, int %i3, int %n3, array<bool>^%c77);
        static bool unpack77(array<bool>^c77, int forRx, System::String^%msg);
        static bool checkCall(System::String ^call, System::String ^%basecall);
        static void Play(array<int>^itone, int baseFrequency, System::IntPtr outputSink);

    protected:
	};
}

