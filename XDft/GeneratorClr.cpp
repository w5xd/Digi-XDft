#include "stdafxClr.h"
#include <AudioSink.h>
#include <commons.h>
#include <XDxmitFT8.h>
#include "GeneratorClr.h"
#include "GeneratorImpl.h"

#define NUM_ISCAT_SYMBOLS 1291             //30*11025/256
namespace XDft {

	const unsigned NUM_FT8_SYMBOLS = 79;
	const unsigned NUM_FT8_BITS = 77;
    const int NUM_MSG_CHARS = 37;
    const int ThirtySeven = 37;
    const int SeventySeven = 77;

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

    void Generator::Play(array<int>^itone, int baseFrequency, System::IntPtr outputSink)
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
            impl::GeneratorImpl::Play(itoneNative, baseFrequency, pSink);
        }
        catch (const std::exception &e)
        {   // convert c++ to clr exception
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
    }

}