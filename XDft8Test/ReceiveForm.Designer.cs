namespace XDft8Test
{
    partial class ReceiveForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.buttonSetup = new System.Windows.Forms.Button();
            this.buttonOpenWaveIn = new System.Windows.Forms.Button();
            this.buttonInputResume = new System.Windows.Forms.Button();
            this.buttonInputPause = new System.Windows.Forms.Button();
            this.buttonInputClose = new System.Windows.Forms.Button();
            this.timerFt8Clock = new System.Windows.Forms.Timer(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.labelPower = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.checkBoxRecord = new System.Windows.Forms.CheckBox();
            this.labelClock = new System.Windows.Forms.Label();
            this.labelInputState = new System.Windows.Forms.Label();
            this.radioButtonInputRight = new System.Windows.Forms.RadioButton();
            this.radioButtonInputLeft = new System.Windows.Forms.RadioButton();
            this.comboBoxWaveIn = new System.Windows.Forms.ComboBox();
            this.buttonInputClear = new System.Windows.Forms.Button();
            this.listBoxReceived = new System.Windows.Forms.ListBox();
            this.buttonPlayFile = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonSpy = new System.Windows.Forms.Button();
            this.comboBoxnDepth = new System.Windows.Forms.ComboBox();
            this.checkBoxEnableAP = new System.Windows.Forms.CheckBox();
            this.checkBoxPresentRedundant = new System.Windows.Forms.CheckBox();
            this.chartSpectrum = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.timerSpectrum = new System.Windows.Forms.Timer(this.components);
            this.numericUpDownMinFreq = new System.Windows.Forms.NumericUpDown();
            this.numericUpDownMaxFreq = new System.Windows.Forms.NumericUpDown();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartSpectrum)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMinFreq)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMaxFreq)).BeginInit();
            this.SuspendLayout();
            // 
            // buttonSetup
            // 
            this.buttonSetup.Location = new System.Drawing.Point(12, 606);
            this.buttonSetup.Name = "buttonSetup";
            this.buttonSetup.Size = new System.Drawing.Size(75, 23);
            this.buttonSetup.TabIndex = 10;
            this.buttonSetup.Text = "Setup...";
            this.buttonSetup.UseVisualStyleBackColor = true;
            this.buttonSetup.Click += new System.EventHandler(this.buttonSetup_Click);
            // 
            // buttonOpenWaveIn
            // 
            this.buttonOpenWaveIn.Location = new System.Drawing.Point(246, 18);
            this.buttonOpenWaveIn.Name = "buttonOpenWaveIn";
            this.buttonOpenWaveIn.Size = new System.Drawing.Size(75, 23);
            this.buttonOpenWaveIn.TabIndex = 3;
            this.buttonOpenWaveIn.Text = "&Open";
            this.buttonOpenWaveIn.UseVisualStyleBackColor = true;
            this.buttonOpenWaveIn.Click += new System.EventHandler(this.buttonInputOpen_Click);
            // 
            // buttonInputResume
            // 
            this.buttonInputResume.Enabled = false;
            this.buttonInputResume.Location = new System.Drawing.Point(246, 51);
            this.buttonInputResume.Name = "buttonInputResume";
            this.buttonInputResume.Size = new System.Drawing.Size(75, 23);
            this.buttonInputResume.TabIndex = 6;
            this.buttonInputResume.Text = "Start";
            this.buttonInputResume.UseVisualStyleBackColor = true;
            this.buttonInputResume.Click += new System.EventHandler(this.buttonInputResume_Click);
            // 
            // buttonInputPause
            // 
            this.buttonInputPause.Enabled = false;
            this.buttonInputPause.Location = new System.Drawing.Point(333, 51);
            this.buttonInputPause.Name = "buttonInputPause";
            this.buttonInputPause.Size = new System.Drawing.Size(75, 23);
            this.buttonInputPause.TabIndex = 7;
            this.buttonInputPause.Text = "Pause";
            this.buttonInputPause.UseVisualStyleBackColor = true;
            this.buttonInputPause.Click += new System.EventHandler(this.buttonInputPause_Click);
            // 
            // buttonInputClose
            // 
            this.buttonInputClose.Enabled = false;
            this.buttonInputClose.Location = new System.Drawing.Point(246, 84);
            this.buttonInputClose.Name = "buttonInputClose";
            this.buttonInputClose.Size = new System.Drawing.Size(75, 23);
            this.buttonInputClose.TabIndex = 8;
            this.buttonInputClose.Text = "Close";
            this.buttonInputClose.UseVisualStyleBackColor = true;
            this.buttonInputClose.Click += new System.EventHandler(this.buttonInputClose_Click);
            // 
            // timerFt8Clock
            // 
            this.timerFt8Clock.Interval = 500;
            this.timerFt8Clock.Tick += new System.EventHandler(this.timerFt8Clock_Tick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.labelPower);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.checkBoxRecord);
            this.groupBox1.Controls.Add(this.labelClock);
            this.groupBox1.Controls.Add(this.labelInputState);
            this.groupBox1.Controls.Add(this.radioButtonInputRight);
            this.groupBox1.Controls.Add(this.radioButtonInputLeft);
            this.groupBox1.Controls.Add(this.comboBoxWaveIn);
            this.groupBox1.Controls.Add(this.buttonOpenWaveIn);
            this.groupBox1.Controls.Add(this.buttonInputClose);
            this.groupBox1.Controls.Add(this.buttonInputResume);
            this.groupBox1.Controls.Add(this.buttonInputPause);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(416, 141);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "FT8 real time decoder";
            // 
            // labelPower
            // 
            this.labelPower.AutoSize = true;
            this.labelPower.Location = new System.Drawing.Point(59, 117);
            this.labelPower.Name = "labelPower";
            this.labelPower.Size = new System.Drawing.Size(35, 13);
            this.labelPower.TabIndex = 11;
            this.labelPower.Text = "label4";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(16, 117);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(37, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "Power";
            // 
            // checkBoxRecord
            // 
            this.checkBoxRecord.AutoSize = true;
            this.checkBoxRecord.Location = new System.Drawing.Point(246, 117);
            this.checkBoxRecord.Name = "checkBoxRecord";
            this.checkBoxRecord.Size = new System.Drawing.Size(140, 17);
            this.checkBoxRecord.TabIndex = 9;
            this.checkBoxRecord.Text = "Record 15sec .wav files";
            this.checkBoxRecord.UseVisualStyleBackColor = true;
            this.checkBoxRecord.CheckedChanged += new System.EventHandler(this.checkBoxRecord_CheckedChanged);
            // 
            // labelClock
            // 
            this.labelClock.AutoSize = true;
            this.labelClock.Location = new System.Drawing.Point(194, 56);
            this.labelClock.Name = "labelClock";
            this.labelClock.Size = new System.Drawing.Size(35, 13);
            this.labelClock.TabIndex = 5;
            this.labelClock.Text = "label3";
            // 
            // labelInputState
            // 
            this.labelInputState.AutoSize = true;
            this.labelInputState.Location = new System.Drawing.Point(341, 23);
            this.labelInputState.Name = "labelInputState";
            this.labelInputState.Size = new System.Drawing.Size(12, 13);
            this.labelInputState.TabIndex = 4;
            this.labelInputState.Text = "x";
            // 
            // radioButtonInputRight
            // 
            this.radioButtonInputRight.AutoSize = true;
            this.radioButtonInputRight.Location = new System.Drawing.Point(16, 76);
            this.radioButtonInputRight.Name = "radioButtonInputRight";
            this.radioButtonInputRight.Size = new System.Drawing.Size(50, 17);
            this.radioButtonInputRight.TabIndex = 2;
            this.radioButtonInputRight.Text = "Right";
            this.radioButtonInputRight.UseVisualStyleBackColor = true;
            // 
            // radioButtonInputLeft
            // 
            this.radioButtonInputLeft.AutoSize = true;
            this.radioButtonInputLeft.Checked = true;
            this.radioButtonInputLeft.Location = new System.Drawing.Point(16, 52);
            this.radioButtonInputLeft.Name = "radioButtonInputLeft";
            this.radioButtonInputLeft.Size = new System.Drawing.Size(43, 17);
            this.radioButtonInputLeft.TabIndex = 1;
            this.radioButtonInputLeft.TabStop = true;
            this.radioButtonInputLeft.Text = "Left";
            this.radioButtonInputLeft.UseVisualStyleBackColor = true;
            // 
            // comboBoxWaveIn
            // 
            this.comboBoxWaveIn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxWaveIn.FormattingEnabled = true;
            this.comboBoxWaveIn.Location = new System.Drawing.Point(6, 19);
            this.comboBoxWaveIn.Name = "comboBoxWaveIn";
            this.comboBoxWaveIn.Size = new System.Drawing.Size(224, 21);
            this.comboBoxWaveIn.TabIndex = 0;
            // 
            // buttonInputClear
            // 
            this.buttonInputClear.Location = new System.Drawing.Point(293, 606);
            this.buttonInputClear.Name = "buttonInputClear";
            this.buttonInputClear.Size = new System.Drawing.Size(75, 23);
            this.buttonInputClear.TabIndex = 11;
            this.buttonInputClear.Text = "&Clear";
            this.buttonInputClear.UseVisualStyleBackColor = true;
            this.buttonInputClear.Click += new System.EventHandler(this.buttonInputClear_Click);
            // 
            // listBoxReceived
            // 
            this.listBoxReceived.Font = new System.Drawing.Font("Lucida Console", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.listBoxReceived.FormattingEnabled = true;
            this.listBoxReceived.Location = new System.Drawing.Point(12, 251);
            this.listBoxReceived.Name = "listBoxReceived";
            this.listBoxReceived.Size = new System.Drawing.Size(384, 342);
            this.listBoxReceived.TabIndex = 2;
            // 
            // buttonPlayFile
            // 
            this.buttonPlayFile.Location = new System.Drawing.Point(451, 30);
            this.buttonPlayFile.Name = "buttonPlayFile";
            this.buttonPlayFile.Size = new System.Drawing.Size(127, 23);
            this.buttonPlayFile.TabIndex = 1;
            this.buttonPlayFile.Text = "&Decode WAV file...";
            this.buttonPlayFile.UseVisualStyleBackColor = true;
            this.buttonPlayFile.Click += new System.EventHandler(this.buttonPlayFile_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(411, 250);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Min freq (Hz)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(411, 279);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(70, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Max freq (Hz)";
            // 
            // buttonSpy
            // 
            this.buttonSpy.Location = new System.Drawing.Point(464, 606);
            this.buttonSpy.Name = "buttonSpy";
            this.buttonSpy.Size = new System.Drawing.Size(75, 23);
            this.buttonSpy.TabIndex = 12;
            this.buttonSpy.Text = "Spy";
            this.buttonSpy.UseVisualStyleBackColor = true;
            this.buttonSpy.Visible = false;
            this.buttonSpy.Click += new System.EventHandler(this.buttonSpy_Click);
            // 
            // comboBoxnDepth
            // 
            this.comboBoxnDepth.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxnDepth.FormattingEnabled = true;
            this.comboBoxnDepth.Items.AddRange(new object[] {
            "Fast",
            "Normal",
            "Deep"});
            this.comboBoxnDepth.Location = new System.Drawing.Point(414, 308);
            this.comboBoxnDepth.Name = "comboBoxnDepth";
            this.comboBoxnDepth.Size = new System.Drawing.Size(126, 21);
            this.comboBoxnDepth.TabIndex = 7;
            this.comboBoxnDepth.SelectedIndexChanged += new System.EventHandler(this.comboBoxnDepth_SelectedIndexChanged);
            // 
            // checkBoxEnableAP
            // 
            this.checkBoxEnableAP.AutoSize = true;
            this.checkBoxEnableAP.Location = new System.Drawing.Point(414, 345);
            this.checkBoxEnableAP.Name = "checkBoxEnableAP";
            this.checkBoxEnableAP.Size = new System.Drawing.Size(76, 17);
            this.checkBoxEnableAP.TabIndex = 8;
            this.checkBoxEnableAP.Text = "Enable AP";
            this.checkBoxEnableAP.UseVisualStyleBackColor = true;
            this.checkBoxEnableAP.CheckedChanged += new System.EventHandler(this.checkBoxEnableAP_CheckedChanged);
            // 
            // checkBoxPresentRedundant
            // 
            this.checkBoxPresentRedundant.AutoSize = true;
            this.checkBoxPresentRedundant.Location = new System.Drawing.Point(414, 574);
            this.checkBoxPresentRedundant.Name = "checkBoxPresentRedundant";
            this.checkBoxPresentRedundant.Size = new System.Drawing.Size(148, 17);
            this.checkBoxPresentRedundant.TabIndex = 9;
            this.checkBoxPresentRedundant.Text = "Show redundant decodes";
            this.checkBoxPresentRedundant.UseVisualStyleBackColor = true;
            // 
            // chartSpectrum
            // 
            this.chartSpectrum.BorderlineColor = System.Drawing.Color.Black;
            chartArea1.AxisX.Interval = 1000D;
            chartArea1.AxisX.IsLabelAutoFit = false;
            chartArea1.AxisX.Maximum = 3000D;
            chartArea1.AxisX.Minimum = 0D;
            chartArea1.AxisX.MinorGrid.Enabled = true;
            chartArea1.AxisY.IsLogarithmic = true;
            chartArea1.AxisY.LabelStyle.Enabled = false;
            chartArea1.AxisY.MajorGrid.Interval = 2D;
            chartArea1.AxisY.Maximum = 100000D;
            chartArea1.AxisY.Minimum = 0.01D;
            chartArea1.Name = "ChartArea1";
            this.chartSpectrum.ChartAreas.Add(chartArea1);
            this.chartSpectrum.Location = new System.Drawing.Point(12, 162);
            this.chartSpectrum.Name = "chartSpectrum";
            series1.ChartArea = "ChartArea1";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Line;
            series1.IsVisibleInLegend = false;
            series1.Name = "Amp";
            series1.SmartLabelStyle.Enabled = false;
            this.chartSpectrum.Series.Add(series1);
            this.chartSpectrum.Size = new System.Drawing.Size(560, 79);
            this.chartSpectrum.TabIndex = 13;
            // 
            // timerSpectrum
            // 
            this.timerSpectrum.Interval = 300;
            this.timerSpectrum.Tick += new System.EventHandler(this.timerSpectrum_Tick);
            // 
            // numericUpDownMinFreq
            // 
            this.numericUpDownMinFreq.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownMinFreq.Location = new System.Drawing.Point(499, 250);
            this.numericUpDownMinFreq.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.numericUpDownMinFreq.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownMinFreq.Name = "numericUpDownMinFreq";
            this.numericUpDownMinFreq.Size = new System.Drawing.Size(63, 20);
            this.numericUpDownMinFreq.TabIndex = 14;
            this.numericUpDownMinFreq.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownMinFreq.ValueChanged += new System.EventHandler(this.numericMinMaxFreq_Changed);
            // 
            // numericUpDownMaxFreq
            // 
            this.numericUpDownMaxFreq.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownMaxFreq.Location = new System.Drawing.Point(499, 279);
            this.numericUpDownMaxFreq.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.numericUpDownMaxFreq.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownMaxFreq.Name = "numericUpDownMaxFreq";
            this.numericUpDownMaxFreq.Size = new System.Drawing.Size(63, 20);
            this.numericUpDownMaxFreq.TabIndex = 15;
            this.numericUpDownMaxFreq.Value = new decimal(new int[] {
            3000,
            0,
            0,
            0});
            this.numericUpDownMaxFreq.ValueChanged += new System.EventHandler(this.numericMinMaxFreq_Changed);
            // 
            // ReceiverForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(585, 646);
            this.Controls.Add(this.numericUpDownMaxFreq);
            this.Controls.Add(this.numericUpDownMinFreq);
            this.Controls.Add(this.chartSpectrum);
            this.Controls.Add(this.checkBoxPresentRedundant);
            this.Controls.Add(this.checkBoxEnableAP);
            this.Controls.Add(this.comboBoxnDepth);
            this.Controls.Add(this.buttonSpy);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonPlayFile);
            this.Controls.Add(this.buttonInputClear);
            this.Controls.Add(this.listBoxReceived);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.buttonSetup);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "ReceiverForm";
            this.Text = "XDft8Test";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartSpectrum)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMinFreq)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownMaxFreq)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonSetup;
        private System.Windows.Forms.Button buttonOpenWaveIn;
        private System.Windows.Forms.Button buttonInputResume;
        private System.Windows.Forms.Button buttonInputPause;
        private System.Windows.Forms.Button buttonInputClose;
        private System.Windows.Forms.Timer timerFt8Clock;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox comboBoxWaveIn;
        private System.Windows.Forms.RadioButton radioButtonInputRight;
        private System.Windows.Forms.RadioButton radioButtonInputLeft;
        private System.Windows.Forms.Label labelInputState;
        private System.Windows.Forms.Button buttonInputClear;
        private System.Windows.Forms.ListBox listBoxReceived;
        private System.Windows.Forms.Button buttonPlayFile;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label labelClock;
        private System.Windows.Forms.Button buttonSpy;
        private System.Windows.Forms.ComboBox comboBoxnDepth;
        private System.Windows.Forms.CheckBox checkBoxEnableAP;
        private System.Windows.Forms.CheckBox checkBoxRecord;
        private System.Windows.Forms.CheckBox checkBoxPresentRedundant;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartSpectrum;
        private System.Windows.Forms.Label labelPower;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Timer timerSpectrum;
        private System.Windows.Forms.NumericUpDown numericUpDownMinFreq;
        private System.Windows.Forms.NumericUpDown numericUpDownMaxFreq;
    }
}

