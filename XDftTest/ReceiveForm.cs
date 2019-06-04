using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

/* Copyright (C) 2019 by Wayne E. Wright, W5XD
 * Round Rock, Texas  */

namespace XDftTest
{
    /* MainForm
     * presents controls for invoking the FT8/FT4 modem for receive.
     * This form (and the entire application) is for demonstrating
     * how to call into the XDft8 to exercise its modem functions.
     * 
     * This source is arranged by the author in the order of 
     * importance that a programmer new to XDft8 would need to understand.
     */
    public partial class ReceiveForm : Form
    {
     
        // These are the object's needed to decode.
        private XDft.Demodulator demodulator; 
        private XDft.WsjtSharedMemory wsjtSharedMem;
        private XDft.WsjtExe wsjtExe;
        private XD.WaveDevicePlayer waveDevicePlayer;
        private XDft.DigiMode digiMode;

        private string mycall;
        private TransmitForm xmitForm;

        /* Note to programmers reading this for the first time:
         * Please do NOT be intimidated by all the comments in InitDemodulator!
         * They describe everything in gory detail. All you MUST know is this:
         * Instance these three objects from the XDft8 namespace:
         *      Demodulator
         *      WsjtSharedMemory
         *      WsjtExe
         *      
         *  For the last two, choose the names in their properties carefully IF you 
         *  are going to  run multiple instances of the Demodulator on the same PC.
         *  
         *  Set the Demodulator's callback delegate for decoded messages.
         *  ...and do nothing more at initialization time. 
         *  
         *  You also need a timer to call Demodulator.Clock(). See below.
         */
        private void InitDemodulator(SetupForm sf)
        {
            // The objects implement IDisposable. If you fail to
            // dispose of one you quit using, its Windows resources
            // remain allocated until garbage collection.
            if (null != demodulator)
                demodulator.Dispose();
            demodulator = null;
            if (null != wsjtSharedMem)
                wsjtSharedMem.Dispose();
            wsjtSharedMem = null;
            if (null != wsjtExe)
                wsjtExe.Dispose();
            wsjtExe = null;
            if (null != waveDevicePlayer)
                waveDevicePlayer.Dispose();
            waveDevicePlayer = null;

            // The demodulator invokes the wsjtx decoder
            demodulator = new XDft.Demodulator();
            // the names of its parameters are verbatim from the wsjt-x source code.
            // Don't ask this author what they mean.
            demodulator.nftx = 1500;
            demodulator.nfqso = 1500;
            demodulator.nQSOProgress = 5;
            demodulator.nzhsym = digiMode == XDft.DigiMode.DIGI_FT8 ? 50 : 18;
            mycall = sf.myCall.ToUpper();
            demodulator.mycall = mycall;
            if (xmitForm != null)
                xmitForm.mycall = mycall;

            demodulator.digiMode = digiMode;
            // When the decoder finds an FT8 message, it calls us back...
            // ...on a foreign thread. Call BeginInvoke to get back on this one. See below.
            demodulator.DemodulatorResultCallback = new XDft.DemodResult(Decoded);

            // The wsjt-x code is run in a subprocess and we communicate with it using
            // cross-process shared memory. Such a construct requires the parent/sub-process
            // to agree on a name. That name must be unique to all the parent/sub's running
            // concurrently. That means that whatever name you give here must NOT
            // also be given to a copy of this programming running concurrently on the 
            // same PC. For the test program, we leave it up to the user in the setup form.
            // that is a VERY bad idea for production...you have been warned.
            wsjtSharedMem = new XDft.WsjtSharedMemory(sf.sharedMemoryKey, false);
            if (!wsjtSharedMem.CreateWsjtSharedMem())
            {
                MessageBox.Show("Failed to create Shared Memory from " + sf.sharedMemoryKey);
                return;
            }

            // The subprocess itself is managed by the XDft
            wsjtExe = new XDft.WsjtExe();
            wsjtExe.AppDataName = sf.appDataName;

            if (!wsjtExe.CreateWsjtProcess(wsjtSharedMem))
            {
                MessageBox.Show("Failed to launch wsjt exe");
                demodulator.Dispose();
                wsjtExe.Dispose();
                wsjtExe = null;
                wsjtSharedMem.Dispose();
                wsjtSharedMem = null;
                demodulator = null;
                return;
            }
            checkBoxEnableAP.Checked = demodulator.lft8apon;
            comboBoxnDepth.SelectedIndex = demodulator.ndepth - 1;

#if DEBUG
            /* demonstrate how to use a native C++ dll to process 
           ** incoming audio. Processing does not affect decoding. This is useful if you want to
           ** multi-instance XDft.Demodulator. Only one of XDft.Demodulator
           ** can call GetSignalSpectrum. The AudioProcessor interface can be instanced
           ** for multiple decodes concurrently and you can, for example, compute
           ** a spectrum in this processor .*/
            IntPtr ip = XDft8RxAudioProcess.RxAudio.GetAudioProcessor();
            demodulator.SetAudioSamplesCallback(new XDft.AudioCallback(SamplesCallback), 1000, 512, ip);
#endif
        }

        private bool StartedRecordFileThisCycle = false;
        uint TRIGGER_DECODE_TENTH = 120; // At second #12 and later, see if we see any messages
        uint FINAL_TENTH = 140;

        // The XDft8 decoder, when operating in real time
        // invokes the FT8 demodulator on our clock. If we 
        // don't clock it, we won't get any received data.
        // It has some internal chores it does that are not visible to us
        // here. But its threading model is such that it works properly
        // only if we call all its methods from this same thread. So...we
        // provide its timers. The interval is NOT critical, except that it 
        // MUST be < 1000 msec.
        private void timerFt8Clock_Tick(object sender, EventArgs e)
        {
            labelClock.Text = "";
            if ((null != demodulator) && (null != waveDevicePlayer))
            {
                // TRIGGER_DECODE tells the demodulator whether to actually demodulate
                bool invokedDecode = false; int cycleNumber = 0;
                var intervalTenth = demodulator.Clock(TRIGGER_DECODE_TENTH, wsjtExe, ref invokedDecode, ref cycleNumber);
                // invokedDecode tells us whether it actually was able to invoke the wsjtx decoder.
                // Some reasons it might not: interval is less than TRIGGER_DECODE.
                // We have recently called into Clock which did invoke a decode, and that one isn't finished yet.
                labelClock.Text = (intervalTenth/10).ToString();

                if ((intervalTenth >= FINAL_TENTH) && !String.IsNullOrEmpty(RecordFileRootName) && RecordFileIteration > 0)
                {   // make a string of wav files if the user wants them
                    if (!StartedRecordFileThisCycle)
                    {
                        StartedRecordFileThisCycle = true;
                        waveDevicePlayer.StopRecording();
                        waveDevicePlayer.StartRecordingFile(
                            String.Format("{0}-{1:D3}.wav", RecordFileRootName, RecordFileIteration++));
                    }
                }
                else
                    StartedRecordFileThisCycle = false;

                if (intervalTenth <= 20)
                    filterDecodes.Clear();
            }
        }

        // Because we invoke Clock() multiple times during a 15second cycle,
        // we will likely get multiple OnReceived for the same message.
        // This filterDecodes HELPs remove them, but it is not 100% effect!
        // Why? Because there is some randomness in the signal strength report
        // that can change when you add more data onto the end of the decode cycle.
        // Because filterDecodes does a strict string compare, a change in the signal
        // level fools it. You are invited to make your application smarter than that.
        private Dictionary<String, bool> filterDecodes = new Dictionary<string, bool>();

        private void OnReceived(String s, int cycle)
        {   // When the FT8 decoder is invoked, it may find 
            // multiple signals in the stream. Each is notified by
            // a separate string here. An empty string is sent
            // at the end of the decoding session.
            if (!String.IsNullOrEmpty(s))
            {
                bool redundant = checkBoxPresentRedundant.Checked;
                bool val = false;
                if (redundant || !filterDecodes.TryGetValue(s, out val))
                {
                    listBoxReceived.Items.Add(s);
                    if (!redundant)
                        filterDecodes[s] = true;
                    int visibleItems = listBoxReceived.ClientSize.Height / listBoxReceived.ItemHeight;
                    listBoxReceived.TopIndex = Math.Max(listBoxReceived.Items.Count - visibleItems + 1, 0);
                    int v = s.IndexOf("~  ");
                    if (v >= 0)
                    {   // if the message has been through FT8's 77bit pack/unpack
                        string msg = s.Substring(v+3);
                        int i3 = 0; int n3 = 0;
                        bool[] c77 = null;
                        XDft.Generator.pack77(msg, ref i3, ref n3, ref c77);
                        // have a look at the packing type. i3 and n3
                    }
                }
            }
        }

        private void timerSpectrum_Tick(object sender, EventArgs e)
        {
            DisplaySpectrum();
        }

        /* This spectrum display is cheap and dirty. Enough to demonstrate
         * how and when to call into GetSignalSpectrum. The display is
         * "cheap" because it uses the Windows Form DataVisualization chart
         * that is not a big deal to install.
         */
        const int NUM_TO_AVERAGE = 5;
        // this magic number comes from deep inside the wsjtx sources.
        // Hint: stop at the bin for 5000Hz when doing a 2**14 bin FFT on 12000KHz data.
        const int NUM_FREQUENCY_BINS = 6827; // each bin is 12000/(2**14) Hz
        private float[] spectrum = new float[NUM_FREQUENCY_BINS];
        private float[][] averagedSpectrum = new float[NUM_TO_AVERAGE][];
        private int numAveragedSpectrum = 0;
        private int movingAverageIdx = 0;
        private void DisplaySpectrum()
        {
            for (int j = 0; j < NUM_TO_AVERAGE; j++)
            {
                if (null == averagedSpectrum[j])
                    averagedSpectrum[j] = new float[NUM_FREQUENCY_BINS];
            }
            float power = 0;

            /* wsjtx-2.0.0 implements its spectrum calculation in a non-reentrant
            ** way. The result is that ONLY one instance of XDft.Decoder
            ** can call GetSignalSpectrum else you won't get results that are
            ** of any use. You may switch from one instance to another only
            ** if you're willing to put up with an undefined length of time
            ** it takes for the new one to properly synchronize (a full 15sec
            ** cycle is enough, but before that, you don't know what you're getting.) */
            uint samples = demodulator.GetSignalSpectrum(spectrum, ref power);

            for (int i = 0; i < NUM_FREQUENCY_BINS; i++)
                averagedSpectrum[movingAverageIdx][i] = spectrum[i];
            movingAverageIdx += 1;
            if (movingAverageIdx >= NUM_TO_AVERAGE)
                movingAverageIdx = 0;
            numAveragedSpectrum += 1;
            if (numAveragedSpectrum >= NUM_TO_AVERAGE)
                numAveragedSpectrum = NUM_TO_AVERAGE;

            var series = chartSpectrum.Series[0];
            series.Points.Clear();
            for (int i = 0; i < 4096; i += 1)
            {
                double sample = 0;
                for (int j = 0; j < numAveragedSpectrum; j++)
                    sample += averagedSpectrum[j][i];
                series.Points.AddXY((5000 * i) / NUM_FREQUENCY_BINS, sample / numAveragedSpectrum);
            }
            labelPower.Text = power.ToString();
        }

        public ReceiveForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            labelClock.Text = "";
            labelInputState.Text = "";
            labelPower.Text = "";
#if DEBUG
            // for setting a debugger to inspect the shared
            // memory between WSJT-X and its .exe
            buttonSpy.Visible = true;
#endif
            SetupForm sf = new SetupForm();
            if (sf.ShowDialog() != DialogResult.OK)
            {
                Close();
                return;
            }
            digiMode = sf.digiMode;
            TRIGGER_DECODE_TENTH = digiMode == XDft.DigiMode.DIGI_FT8 ? 130u : 40u;
            FINAL_TENTH = digiMode == XDft.DigiMode.DIGI_FT8 ? 140u: 65u;
            InitDemodulator(sf);
            var wavesIn = XD.WaveDeviceEnumerator.waveInDevices();
            foreach (var s in wavesIn)
                comboBoxWaveIn.Items.Add(s);
            comboBoxWaveIn.SelectedIndex = 0;
            xmitForm = new TransmitForm();
            xmitForm.digiMode = digiMode;
            xmitForm.mycall = mycall;
            xmitForm.Show();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            /* Failure to call Dispose will keep certain Windows handles
             * on, for example, shared memory, active for an interminate amount
             * of time before garbage collection. Its OK to fail if this process
             * is exiting: the OS will clean up. But if your process instances
             * multiple forms, then you had better do the Dispose's
             */
            if (demodulator != null)
                demodulator.Dispose();
            demodulator = null;

            if (wsjtExe != null)
                wsjtExe.Dispose();
            wsjtExe = null;

            if (wsjtSharedMem != null)
                wsjtSharedMem.Dispose();
            wsjtSharedMem = null;

            if (waveDevicePlayer != null)
                waveDevicePlayer.Dispose();
            waveDevicePlayer = null;
        }

        private void buttonSetup_Click(object sender, EventArgs e)
        {
            SetupForm sf = new SetupForm();
            if (null != wsjtSharedMem)
                sf.sharedMemoryKey = wsjtSharedMem.SharedMemoryKey;
            if (null != wsjtExe)
                sf.appDataName = wsjtExe.AppDataName;
            if (null != demodulator)
                sf.myCall = demodulator.mycall;
            if (sf.ShowDialog() == DialogResult.OK)
                InitDemodulator(sf);
        }

        private void buttonInputClose_Click(object sender, EventArgs e)
        {
            timerFt8Clock.Enabled = false;
            timerSpectrum.Enabled = false;
            if (waveDevicePlayer != null)
                waveDevicePlayer.Close();
            buttonOpenWaveIn.Enabled = true;
            buttonInputClose.Enabled = false;
            buttonInputPause.Enabled = false;
            buttonInputResume.Enabled = false;
            buttonPlayFile.Enabled = true;
            labelInputState.Text = "";
            labelClock.Text = "";
        }

        private void buttonInputPause_Click(object sender, EventArgs e)
        {
            timerFt8Clock.Enabled = false;
            timerSpectrum.Enabled = false;
            if (waveDevicePlayer != null)
                waveDevicePlayer.Pause();
            labelInputState.Text = "Paused";
            buttonInputResume.Text = "Resume";
            buttonInputResume.Enabled = true;
            buttonInputPause.Enabled = false;
        }

        private void buttonInputResume_Click(object sender, EventArgs e)
        {
            timerFt8Clock.Enabled = true;
            timerSpectrum.Enabled = true;
            buttonInputResume.Enabled = false;
            buttonInputPause.Enabled = true;
            SetDecodeFrequencyRange();
            if (waveDevicePlayer != null)
                waveDevicePlayer.Resume();
            labelInputState.Text = "Decoding";
        }

        private void buttonInputOpen_Click(object sender, EventArgs e)
        {
            uint devIndex = (uint)comboBoxWaveIn.SelectedIndex;
            uint channel = radioButtonInputLeft.Checked ? 0u : 1u;
            if (waveDevicePlayer != null)
                waveDevicePlayer.Dispose();
            waveDevicePlayer = new XD.WaveDevicePlayer();
            if (!waveDevicePlayer.Open(devIndex, channel, demodulator.GetRealTimeRxSink()))
            {
                MessageBox.Show("Failed to open wave input");
                waveDevicePlayer.Dispose();
                waveDevicePlayer = null;
            }
            else
            {
                labelInputState.Text = "Paused";
                buttonPlayFile.Enabled = false;
                buttonOpenWaveIn.Enabled = false;
                buttonInputClose.Enabled = true;
                buttonInputResume.Enabled = true;
                var series = chartSpectrum.Series[0];
                series.Points.Clear();
           }
        }

        private void buttonPlayFile_Click(object sender, EventArgs e)
        {
            var fd = new OpenFileDialog();
            fd.Title = "Select .WAV file. 15 seconds length is appropriate";
            fd.Filter = "Wave Files (*.wav)|*.wav";
            if (fd.ShowDialog() == DialogResult.OK)
            {
                uint channel = radioButtonInputLeft.Checked ? 0u : 1u;
                SetDecodeFrequencyRange();
                XD.WaveFilePlayer.Play(fd.FileName, channel, demodulator.Playback(wsjtExe));
            }
        }

        private void buttonInputClear_Click(object sender, EventArgs e)
        {
            listBoxReceived.Items.Clear();
            filterDecodes.Clear();
        }

        private void numericMinMaxFreq_Changed(object sender, EventArgs e)
        {
            SetDecodeFrequencyRange();
        }

        private void SetDecodeFrequencyRange()
        {
            demodulator.nfa = (int)numericUpDownMinFreq.Value;
            demodulator.nfb = (int)numericUpDownMaxFreq.Value;
        }

        private void comboBoxnDepth_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (null != demodulator)
                demodulator.ndepth = comboBoxnDepth.SelectedIndex + 1;
        }

        private void checkBoxEnableAP_CheckedChanged(object sender, EventArgs e)
        {
            if (null != demodulator)
                demodulator.lft8apon = checkBoxEnableAP.Checked;
        }

        private string RecordFileRootName = null;
        private int RecordFileIteration = 0;
        private void checkBoxRecord_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxRecord.Checked)
            {
                var fd = new SaveFileDialog();
                fd.Title = "Select root of .WAV file. Will append digits each 15 seconds.";
                fd.Filter = "Wave Files (*.wav)|*.wav";
                if (fd.ShowDialog() == DialogResult.OK)
                {
                    RecordFileIteration = 1;
                    RecordFileRootName = fd.FileName;
                    if (RecordFileRootName.ToUpper().EndsWith(".WAV"))
                        RecordFileRootName = RecordFileRootName.Substring(0,RecordFileRootName.Length - 4);
                    // undo our file naming convention, if possible, so write over old files if clicked on one
                    int rootNameLen = RecordFileRootName.Length;
                    if ((rootNameLen  >= 4) && (RecordFileRootName[rootNameLen-4] == '-') && 
                        RecordFileRootName.Substring(rootNameLen-3).All(c => c>='0' && c <= '9'))
                        RecordFileRootName = RecordFileRootName.Substring(0, rootNameLen - 4);
                    return;
                }
                else
                    checkBoxRecord.Checked = false;
            }
            RecordFileRootName = null;
        }

        private void SamplesCallback(float[] audio)
        {   // audio is whatever the NativeAudioProcessor produces.
        }
        
        // The XDft8 assembly invokes our delegate on a foreign thread.
        private void Decoded(String s, int cycle)
        {   // BeginInvoke back onto form's thread
            BeginInvoke(new Action<String, int>((String x, int c) => OnReceived(x,c)), s, cycle);
        }

        // If you want to see what WSJT-X is putting in its common block
        // for its jt9.exe, then this button is useful
        // Set a break down in the native code it calls and the
        // debugger will show you the FORTRAN common block.
        private void buttonSpy_Click(object sender, EventArgs e)
        {
            var sharedMem = new XDft.WsjtSharedMemory("WSJT-X", true);
            sharedMem.Spy();
            sharedMem.Dispose();
        }

     }
}
