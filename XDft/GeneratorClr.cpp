#include "stdafxClr.h"
#include <AudioSink.h>
#include <commons.h>
#include <XDencode.h>
#include "GeneratorClr.h"
#include "GeneratorImpl.h"

#define NUM_ISCAT_SYMBOLS 1291             //30*11025/256
namespace XDft {

	const unsigned NUM_FT8_SYMBOLS = 79;
    const unsigned NUM_FT4_SYMBOLS = 103;
	const unsigned NUM_FT8_BITS = 77;
    const int NUM_MSG_CHARS = 37;
    const int ThirtySeven = 37;
    const int SeventySeven = 77;

    Tone::Tone() : coefficient(1.f)
        , itone(nullptr)
        , frequency(1000)
        , delayMsec(0)
    { }

    Tone::Tone(array<int>^ tones, float coef, int f, unsigned short delay)
        : coefficient(coef)
        , itone(tones)
        , frequency(f)
        , delayMsec(delay)
    {}

	void Generator::genft8(System::String ^msg, System::String ^%msgSent, 
		array<int> ^%itoneOut, array<bool> ^%ft8bits)
	{
		int i3 = 0;
		int n3 = 0;
		std::vector<char> msgsent(NUM_MSG_CHARS, ' ');
		std::vector<char> ft8msgbits(NUM_FT8_BITS, ' ');
		std::unique_ptr<int[]>  itone (new int [NUM_ISCAT_SYMBOLS]);

		// c to fortran
        std::vector<char> message(NUM_MSG_CHARS, ' ');
        memcpy(&message[0], msclr::interop::marshal_as<std::string>(msg).c_str(), std::min(msg->Length, NUM_MSG_CHARS));

		genft8_(&message[0], &i3, &n3, &msgsent[0],	&ft8msgbits[0],	&itone[0], message.size(), msgsent.size());
        std::string msgSentStr;
        msgSentStr.assign(msgsent.begin(), msgsent.end());
		msgSent = msclr::interop::marshal_as<System::String^>(msgSentStr);
		itoneOut = gcnew array<int>(NUM_FT8_SYMBOLS);
		for (unsigned i = 0; i < NUM_FT8_SYMBOLS; i++)
			itoneOut[i] = itone[i];
		ft8bits = gcnew array<bool>(NUM_FT8_BITS);
		for (unsigned i = 0; i < NUM_FT8_BITS; i++)
			ft8bits[i] = ft8msgbits[i] != 0;
	}

	void Generator::genft4(System::String ^msg, System::String ^%msgSent, 
		array<int> ^%itoneOut, array<bool> ^%ft4bits)
	{
        int ichk = 0;
		std::vector<char> msgsent(NUM_MSG_CHARS, ' ');
		std::vector<char> ft4msgbits(NUM_FT8_BITS, ' ');
		std::unique_ptr<int[]>  itone (new int [NUM_ISCAT_SYMBOLS]);

		// c to fortran
        std::vector<char> message(NUM_MSG_CHARS, ' ');
        memcpy(&message[0], msclr::interop::marshal_as<std::string>(msg).c_str(), std::min(msg->Length, NUM_MSG_CHARS));

		genft4_(&message[0], &ichk, &msgsent[0], &ft4msgbits[0], &itone[0], message.size(), msgsent.size());
        std::string msgSentStr;
        msgSentStr.assign(msgsent.begin(), msgsent.end());
		msgSent = msclr::interop::marshal_as<System::String^>(msgSentStr);
        // wsjtx adds the extra begin/end symbols at wave generation time.
        // In our wave generator, ft8 and ft4 are identical. so its consistent to add extras here.
        // The extra symbols accommodate ramp up/down times.
		itoneOut = gcnew array<int>(NUM_FT4_SYMBOLS+2);
		for (unsigned i = 0; i < NUM_FT4_SYMBOLS; i++)
			itoneOut[i+1] = itone[i];
        // copy first and last symbols
        itoneOut[0] = itoneOut[1];
        itoneOut[NUM_FT4_SYMBOLS+1] = itone[NUM_FT4_SYMBOLS-1];
		ft4bits = gcnew array<bool>(NUM_FT8_BITS);
		for (unsigned i = 0; i < NUM_FT8_BITS; i++)
			ft4bits[i] = ft4msgbits[i] != 0;
	}
    
    void Generator::setpack77mycall(System::String ^mycall)
    {   // copy value into wsjt fortran statics
        std::string mycallc = msclr::interop::marshal_as<std::string>(mycall);
        std::vector<char> mycallv(13, ' ');
        memcpy(&mycallv[0], mycallc.c_str(), std::min(mycallv.size(), mycallc.size()));
        xdsetpack77mycall_(&mycallv[0]);
    }

    void Generator::setpack77dxcall(System::String ^call)
    {   // copy value into wsjt fortran statics
        std::string dxcallc = msclr::interop::marshal_as<std::string>(call);
        std::vector<char> dxcallv(13, ' ');
        memcpy(&dxcallv[0], dxcallc.c_str(), std::min(dxcallv.size(), dxcallc.size()));
        xdsetpack77mycall_(&dxcallv[0]);
    }

    void Generator::pack77(System::String ^msg, int %i3, int %n3, array<bool>^%c77)
    {
        std::string toPack = msclr::interop::marshal_as<std::string>(msg);
        std::vector<char> msgFortran(ThirtySeven, ' ');
        if (toPack.size() > ThirtySeven)
            toPack.resize(ThirtySeven);
        memcpy(&msgFortran[0], toPack.c_str(), toPack.size());
        std::vector<char> c77Fortran(SeventySeven, '0');
        int i3f = i3;
        int n3f = n3;
        c77 = nullptr;
        xdpack77_(&msgFortran[0], &i3f, &n3f, &c77Fortran[0]);
        // beware. pack77's c77 comes back '0' and '1'
        // genft8 77bit output is \0 and \1
        i3 = i3f;
        n3 = n3f;
        c77 = gcnew array<bool>(SeventySeven);
        for (unsigned i = 0; i < SeventySeven; i++)
        {
            if ((c77Fortran[i] != '0') && (c77Fortran[i] != '1'))
            {
                c77 = nullptr;
                i3 = 0;
                n3 = 0;
                return;
            }
            c77[i] = c77Fortran[i] != '0';
        }
    }

    bool Generator::unpack77(array<bool>^c77clr, int forRx, System::String^%msgclr)
    {
        msgclr = nullptr;
        if ((c77clr == nullptr) || (c77clr->Length < SeventySeven))
            return false;
        std::vector<char> c77(SeventySeven, '0');
        std::vector<char> msg(ThirtySeven, ' ');
        for (unsigned i = 0; i < std::min(c77.size(), (size_t)c77clr->Length); i++)
            c77[i] = c77clr[i] ? '1' : '0';
        char stat = 0;
        int nrx = forRx;
        xdunpack77_(&c77[0], &nrx, &msg[0], &stat);
        std::string msgstr;
        msgstr.assign(msg.begin(), msg.end());
        msgclr = gcnew System::String(msgstr.c_str());
        return stat != 0;
    }

    bool Generator::checkCall(System::String ^call, System::String ^%basecall)
    {
        std::string cppcall = msclr::interop::marshal_as<std::string>(call);
        basecall = nullptr;
        std::vector<char> cCallFortran(13, ' ');
        memcpy(&cCallFortran[0], cppcall.c_str(), std::min(cCallFortran.size(), cppcall.length()));
        std::vector<char> cBaseFortran(6, ' ');
        int ok = 0;
        ::chkcall_(&cCallFortran[0], &cBaseFortran[0], &ok, cCallFortran.size(), cBaseFortran.size());
        if (ok != 0)
        {
            std::string baseCallreturned;
            baseCallreturned.assign(cBaseFortran.begin(), cBaseFortran.end());
            while (!baseCallreturned.empty() && baseCallreturned[baseCallreturned.size() - 1] == ' ')
                baseCallreturned.resize(baseCallreturned.size() - 1);
            basecall = msclr::interop::marshal_as<System::String^>(baseCallreturned);
        }
        return ok != 0;
    }

    bool Generator::pack28(System::String ^call, int %n28)
    {
        std::string cppcall = msclr::interop::marshal_as<std::string>(call);
        std::vector<char> cCallFortran(13, ' ');
        memcpy(&cCallFortran[0], cppcall.c_str(), std::min(cCallFortran.size(), cppcall.length()));
        int n=0;
        ::__packjt77_MOD_pack28(&cCallFortran[0], &n, cCallFortran.size());
        n28 = n;
        static const int MAX22 = 4194304;
        static const int NTOKENS = 2063592;
        return n < NTOKENS + MAX22; // see pack28 source code NTOKENS + MAX22
    }

    void Generator::Play(GeneratorContext ^context, array<int>^itone, int baseFrequency, System::IntPtr outputSink)
    {
        std::shared_ptr<XD::AudioSink> pSink(
            reinterpret_cast<XD::AudioSink*>(outputSink.ToPointer()), [](XD::AudioSink *p)
        {
            p->ReleaseSink();
        });

        try {
            std::vector<int> itoneNative(itone->Length);
            {
                pin_ptr<int> pPin = &itone[0];
                memcpy(&itoneNative[0], pPin, sizeof(itoneNative[0]) * itoneNative.size());
            }
            impl::GeneratorImpl::Play(context->getImpl(),itoneNative, baseFrequency, pSink);
        }
        catch (const std::exception &e)
        {   // convert c++ to clr exception
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    void Generator::Play(GeneratorContext ^context, array<Tone^> ^itones, System::IntPtr outputSink)
    {
        std::shared_ptr<XD::AudioSink> pSink(
            reinterpret_cast<XD::AudioSink*>(outputSink.ToPointer()), [](XD::AudioSink *p)
        {
            p->ReleaseSink();
        });

        try {
            std::vector<impl::Tone> toPlay(itones->Length);
            for (unsigned i = 0; i < static_cast<unsigned>(itones->Length); i++)
            {
                toPlay[i].coefficient = itones[i]->coefficient;
                toPlay[i].frequency = itones[i]->frequency;
                toPlay[i].delayMsec = itones[i]->delayMsec;
                std::vector<int> &tones=toPlay[i].itone;
                tones.resize(itones[i]->itone->Length);
                pin_ptr<int> pPin = &itones[i]->itone[0];
                memcpy(&tones[0], pPin, sizeof(tones[0]) * tones.size());
            }
            impl::GeneratorImpl::Play(context->getImpl(),toPlay, pSink);
        }
        catch (const std::exception &e)
        {   // convert c++ to clr exception
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

    GeneratorContext ^GeneratorContext::getFt4Context(unsigned short msecSilent)
    { return gcnew GeneratorContext(impl::GeneratorContext::getFt4Context(msecSilent));   }

    GeneratorContext ^GeneratorContext::getFt8Context(unsigned short msecSilent)
    {     return gcnew GeneratorContext(impl::GeneratorContext::getFt8Context(msecSilent));   }

    GeneratorContext::~GeneratorContext()
    {   delete m_impl;   }

    GeneratorContext::GeneratorContext(impl::GeneratorContext *p)
    {        m_impl = p; }
}