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

namespace XDft8Test
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
        {
            InitializeComponent();
        }

        private void TransmitForm_Load(object sender, EventArgs e)
        {
            labelXmit.Text = "";

            var wavesOut = XD.WaveDeviceEnumerator.waveOutDevices();
            foreach (var s in wavesOut)
                comboBoxWaveOut.Items.Add(s);
            comboBoxWaveOut.SelectedIndex = 0;
        }

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
            string mybasecall = null;
            XDft.Generator.checkCall(mycall, ref mybasecall);
            int i3 = 0; int n3 = 0;
            bool[] c77 = null;
            XDft.Generator.pack77(msgText, ref i3, ref n3, ref c77);
            String outs=null;
            int nrx = 1;
            XDft.Generator.unpack77(c77, nrx, ref outs);
            XDpack77.Pack77Message.Message m =
                XDpack77.Pack77Message.ReceivedMessage.CreateFromPacked(i3, n3, msgText);
#endif
            // here is 99% of the magic--turn the text into a wsjtx itone array
            XDft.Generator.genft8(
                msgText, 
                ref sent, ref itone, ref ft8bits);
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

        void SendSoundFromTextToDevice(XD.Transmit_Cycle ft)
        {
            // given text that has been through genft8, send it to Generator.Play
            if (null == itone)
                return;
            buttonXmitNow.Enabled = false;
            buttonOdd.Enabled = false;
            buttonEven.Enabled = false;
            if (null == deviceTx)
            {
                deviceTx = new XD.WaveDeviceTx();
                deviceTx.Open((uint)selectedDevice, (uint)selectedChannel);
                deviceTx.SoundSyncCallback = new XD.SoundBeginEnd(AudioBeginEnd);
            }
            deviceTx.TransmitCycle = ft;
#if !DEBUG
            XDft.Generator.Play(itone, (int)numericUpDownFrequency.Value,
                 deviceTx.GetRealTimeAudioSink());
#else
            List<XDft.Tone> tones = new List<XDft.Tone>();
            tones.Add(new XDft.Tone(itone, 1, (int)numericUpDownFrequency.Value));

            String sent = null;
            bool[] ft8bits = null;
            int[] it = null;
            XDft.Generator.genft8(
                            "TEST",
                            ref sent, ref it, ref ft8bits);
            tones.Add(new XDft.Tone(it, 1, (int)numericUpDownFrequency.Value + 100));
            XDft.Generator.Play(tones.ToArray(), deviceTx.GetRealTimeAudioSink());
#endif
        }

        private void buttonXmitNow_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(XD.Transmit_Cycle.PLAY_NOW);
        }
        private void buttonOdd_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(XD.Transmit_Cycle.PLAY_ODD_15S);
        }
        private void buttonEven_Click(object sender, EventArgs e)
        {
            SendSoundFromTextToDevice(XD.Transmit_Cycle.PLAY_EVEN_15S);
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
#if !DEBUG
                XDft.Generator.Play(itone, (int)numericUpDownFrequency.Value,
                    XD.FileDeviceTx.Open(fd.FileName));
#else
                List<XDft.Tone> tones = new List<XDft.Tone>();
                tones.Add(new XDft.Tone(itone, 1, (int)numericUpDownFrequency.Value));
                tones.Add(new XDft.Tone(itone, 1, (int)numericUpDownFrequency.Value + 100));
                XDft.Generator.Play(tones.ToArray(), XD.FileDeviceTx.Open(fd.FileName));
#endif
            }
        }

    }
}
