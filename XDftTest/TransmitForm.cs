using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

/* Copyright (C) 2019 by Wayne E. Wright, W5XD
 * Round Rock, Texas  */

namespace XDftTest
{
    /* Note to programmer reading this for the first time.
     * Transmitting FT8 is far simpler than receiving it.
     * That is reflected in the difference between TransmitForm.cs here
     * and what's in ReceiveForm.cs
     * 
     * For transmit, all you do is run the text through Generator.genft8
     * and send the resulting itone array to an XDft.WaveDeviceTx.
     */
    public partial class TransmitForm : Form
    {
        public TransmitForm()
        { InitializeComponent();  }

        private void TransmitForm_Load(object sender, EventArgs e)
        {
            labelXmit.Text = "";

            var wavesOut = XD.WaveDeviceEnumerator.waveOutDevices();
            foreach (var s in wavesOut)
                comboBoxWaveOut.Items.Add(s);
            comboBoxWaveOut.SelectedIndex = 0;
            ushort msecDelay = 0;
            switch (digiMode)
            {
                case XDft.DigiMode.DIGI_FT8:
                     generatorContext = XDft.GeneratorContext.getFt8Context(msecDelay);
                    break;
                case XDft.DigiMode.DIGI_FT4:
                    generatorContext = XDft.GeneratorContext.getFt4Context(msecDelay);
                    break;
            }
        }

        public XDft.DigiMode digiMode = XDft.DigiMode.DIGI_FT8;
        private int[] itone = null;
        public string mycall;

        private void textBoxXmit_TextChanged(object sender, EventArgs e)
        {
            String sent = null;
            bool[] ft8bits = null;
            String msgText = textBoxXmit.Text.ToUpper();// the ft8 generator behaves more naturally with ToUpper
#if DEBUG
            // this code's only purpose is to make it easy to interactively test
            //  ReceivedMessage.CreateFromPacked
            XDft.Generator.setpack77mycall(mycall); // probably not needed, but sets Fortran static
            int n28=0;
            bool isHashed = XDft.Generator.pack28(mycall, ref n28);
            int i3 = 0; int n3 = 0;
            bool[] c77 = null;
            XDft.Generator.pack77(msgText, ref i3, ref n3, ref c77);
            String outs=null;
            int nrx = 1;
            XDft.Generator.unpack77(c77, nrx, ref outs);
            XDpack77.Pack77Message.Message m =
                XDpack77.Pack77Message.ReceivedMessage.CreateFromPacked(i3, n3, msgText);
#endif
            // turn the text into a wsjtx itone array
            switch (digiMode)
            {
                case XDft.DigiMode.DIGI_FT8:
                    XDft.Generator.genft8(
                        msgText,
                        ref sent, ref itone, ref ft8bits);
                    break;
                case XDft.DigiMode.DIGI_FT4:
                    XDft.Generator.genft4(
                        msgText,
                        ref sent, ref itone, ref ft8bits);
                    break;
            }
            labelXmit.Text = sent;
        }

        private XD.WaveDeviceTx deviceTx = null;
        private int selectedChannel = 0;
        private int selectedDevice = 0;
        private void disposeTx()
        {
            if (null != deviceTx)
                deviceTx.Dispose();
            deviceTx = null;
        }
        private XDft.GeneratorContext generatorContext;

        private void initializeDeviceTx()
        {
            if (null == deviceTx)
            {
                deviceTx = new XD.WaveDeviceTx();
                deviceTx.Open((uint)selectedDevice, (uint)selectedChannel);
                deviceTx.SoundSyncCallback = new XD.SoundBeginEnd(AudioBeginEnd);
            }
        }

        void SendSoundFromTextToDevice(XD.Transmit_Cycle ft)
        {
            // given text that has been through genft8, send it to Generator.Play
            if (null == itone)
                return;
            buttonXmitNow.Enabled = false;
            buttonOdd.Enabled = false;
            buttonEven.Enabled = false;
            initializeDeviceTx();
            deviceTx.TransmitCycle = ft;
            XDft.Generator.Play(generatorContext, itone, (int)numericUpDownFrequency.Value,
                 deviceTx.GetRealTimeAudioSink());
        }

        private void buttonXmitNow_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(XD.Transmit_Cycle.PLAY_NOW);
        }
        private void buttonOdd_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(digiMode == XDft.DigiMode.DIGI_FT8 ?  XD.Transmit_Cycle.PLAY_ODD_15S : XD.Transmit_Cycle.PLAY_ODD_6S);
        }
        private void buttonEven_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(digiMode== XDft.DigiMode.DIGI_FT8 ?  XD.Transmit_Cycle.PLAY_EVEN_15S : XD.Transmit_Cycle.PLAY_EVEN_6S);
        }

        private void AudioBeginEnd(bool isBeginning)
        {   // get back on the form's thread
            BeginInvoke(new Action<bool>(OnAudioComplete), isBeginning);
        }

        private void OnAudioComplete(bool isBegin)
        {
            if (!isBegin)
            {
                buttonXmitNow.Enabled = true;
                buttonOdd.Enabled = true;
                buttonEven.Enabled = true;
            }
        }

        private void TransmitForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            disposeTx();
        }

        private void comboBoxWaveOut_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxWaveOut.SelectedIndex != selectedDevice)
            {
                disposeTx();
                selectedDevice = comboBoxWaveOut.SelectedIndex;
            }
        }

        private void radioButtonOutput_CheckedChanged(object sender, EventArgs e)
        {
            int tab = ((RadioButton)sender).TabIndex - 1;
            if (tab != selectedChannel)
            {
                disposeTx();
                selectedChannel = tab;
            }
        }

        private void buttonAbort_Click(object sender, EventArgs e)
        {
            if (null != deviceTx)
                deviceTx.Abort();
            buttonXmitNow.Enabled = true;
            buttonOdd.Enabled = true;
            buttonEven.Enabled = true;
        }

        private void buttonToFile_Click(object sender, EventArgs e)
        {
            // The resulting wav file is 48KHz sampling. This is
            // for evaluation. And it happens to not be useful 
            // for testing ReceiveForm.cs, which deals in 12Khz audio.
            var fd = new SaveFileDialog();
            fd.Title = "Select .wav file of audio to transmit.";
            fd.Filter = "Wave Files (*.wav)|*.wav";
            if (fd.ShowDialog() == DialogResult.OK)
            {
                XDft.Generator.Play(generatorContext, itone, (int)numericUpDownFrequency.Value,
                    XD.FileDeviceTx.Open(fd.FileName));
            }
        }

        private void buttonMultistreamTest_Click(object sender, EventArgs e)
        {
            var res = MessageBox.Show("Play to device? else to file?", "XDftTest Multi stream test message", MessageBoxButtons.YesNoCancel);
            IntPtr sink = IntPtr.Zero;
            if (res == DialogResult.Yes)
            {
                initializeDeviceTx();
                sink = deviceTx.GetRealTimeAudioSink();
                deviceTx.TransmitCycle = digiMode == XDft.DigiMode.DIGI_FT4 ? XD.Transmit_Cycle.PLAY_EVEN_6S : XD.Transmit_Cycle.PLAY_EVEN_15S;
            }
            else if (res == DialogResult.No)
            {
                var fd = new SaveFileDialog();
                fd.Title = "Select .wav file of audio to transmit.";
                fd.Filter = "Wave Files (*.wav)|*.wav";
                if (fd.ShowDialog() == DialogResult.OK)
                    sink = XD.FileDeviceTx.Open(fd.FileName);
            }
            if (sink != IntPtr.Zero)
            {
                List<XDft.Tone> tones = new List<XDft.Tone>();
                const int NUMBER_OF_STREAMS = 20;
                const int LOWEST_FREQUENCY = 400;
                const ushort START_DELAY_FT4 = (ushort)5400;
                const ushort START_DELAY_FT8 = (ushort)14500;

                int frequency = LOWEST_FREQUENCY;
                ushort delayMsec = digiMode == XDft.DigiMode.DIGI_FT4 ? START_DELAY_FT4 : START_DELAY_FT8;
                Random rand = new Random();

                int [] frequencies = new int[NUMBER_OF_STREAMS];
                for (int i = 0; i < NUMBER_OF_STREAMS; i++)
                {
                    frequencies[i] = frequency;
                    frequency += digiMode == XDft.DigiMode.DIGI_FT4 ? 102 : 61;
                }

                for (int i = 0; i < 100; i++)
                {   // shuffle
                    int idx1 = rand.Next(0, NUMBER_OF_STREAMS-1);
                    int idx2 = rand.Next(0, NUMBER_OF_STREAMS - 1);
                    int temp = frequencies[idx1];
                    frequencies[idx1] = frequencies[idx2];
                    frequencies[idx2] = temp;
                }

                for (int i = 0; i < NUMBER_OF_STREAMS; i++)
                {
                    string msgText = String.Format("Test num {0}", i+1);
                    string sent = "";
                    bool[] ftbits = null;
                    int[] testTones = null;
                    float amplitude = (float)rand.NextDouble();
                    amplitude *= amplitude * amplitude;
                    switch (digiMode)
                    {
                        case XDft.DigiMode.DIGI_FT8:
                            XDft.Generator.genft8(
                                msgText,
                                ref sent, ref testTones, ref ftbits);
                            break;
                        case XDft.DigiMode.DIGI_FT4:
                            XDft.Generator.genft4(
                                msgText,
                                ref sent, ref testTones, ref ftbits);
                            break;
                    }
                    tones.Add(new XDft.Tone(testTones, amplitude, frequencies[i], delayMsec));
                    delayMsec += 202;
                }

                XDft.Generator.Play(generatorContext, tones.ToArray(), sink);
            }
        }
    }
}
