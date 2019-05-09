namespace XDftTest
{
    partial class TransmitForm
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
            this.textBoxXmit = new System.Windows.Forms.TextBox();
            this.labelXmit = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButtonOutputMono = new System.Windows.Forms.RadioButton();
            this.buttonAbort = new System.Windows.Forms.Button();
            this.radioButtonOutputRight = new System.Windows.Forms.RadioButton();
            this.radioButtonOutputLeft = new System.Windows.Forms.RadioButton();
            this.comboBoxWaveOut = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.numericUpDownFrequency = new System.Windows.Forms.NumericUpDown();
            this.buttonXmitNow = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.buttonToFile = new System.Windows.Forms.Button();
            this.buttonEven = new System.Windows.Forms.Button();
            this.buttonOdd = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownFrequency)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // textBoxXmit
            // 
            this.textBoxXmit.Location = new System.Drawing.Point(92, 31);
            this.textBoxXmit.MaxLength = 37;
            this.textBoxXmit.Name = "textBoxXmit";
            this.textBoxXmit.Size = new System.Drawing.Size(153, 20);
            this.textBoxXmit.TabIndex = 1;
            this.textBoxXmit.WordWrap = false;
            this.textBoxXmit.TextChanged += new System.EventHandler(this.textBoxXmit_TextChanged);
            // 
            // labelXmit
            // 
            this.labelXmit.Location = new System.Drawing.Point(258, 35);
            this.labelXmit.Name = "labelXmit";
            this.labelXmit.Size = new System.Drawing.Size(332, 13);
            this.labelXmit.TabIndex = 2;
            this.labelXmit.Text = "label1";
            this.labelXmit.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButtonOutputMono);
            this.groupBox1.Controls.Add(this.buttonAbort);
            this.groupBox1.Controls.Add(this.radioButtonOutputRight);
            this.groupBox1.Controls.Add(this.radioButtonOutputLeft);
            this.groupBox1.Controls.Add(this.comboBoxWaveOut);
            this.groupBox1.Location = new System.Drawing.Point(12, 120);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(271, 123);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "&Playback device";
            // 
            // radioButtonOutputMono
            // 
            this.radioButtonOutputMono.AutoSize = true;
            this.radioButtonOutputMono.Location = new System.Drawing.Point(162, 64);
            this.radioButtonOutputMono.Name = "radioButtonOutputMono";
            this.radioButtonOutputMono.Size = new System.Drawing.Size(52, 17);
            this.radioButtonOutputMono.TabIndex = 3;
            this.radioButtonOutputMono.Text = "Mono";
            this.radioButtonOutputMono.UseVisualStyleBackColor = true;
            this.radioButtonOutputMono.CheckedChanged += new System.EventHandler(this.radioButtonOutput_CheckedChanged);
            // 
            // buttonAbort
            // 
            this.buttonAbort.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonAbort.Location = new System.Drawing.Point(9, 94);
            this.buttonAbort.Name = "buttonAbort";
            this.buttonAbort.Size = new System.Drawing.Size(75, 23);
            this.buttonAbort.TabIndex = 4;
            this.buttonAbort.Text = "&Abort";
            this.buttonAbort.UseVisualStyleBackColor = true;
            this.buttonAbort.Click += new System.EventHandler(this.buttonAbort_Click);
            // 
            // radioButtonOutputRight
            // 
            this.radioButtonOutputRight.AutoSize = true;
            this.radioButtonOutputRight.Location = new System.Drawing.Point(90, 64);
            this.radioButtonOutputRight.Name = "radioButtonOutputRight";
            this.radioButtonOutputRight.Size = new System.Drawing.Size(50, 17);
            this.radioButtonOutputRight.TabIndex = 2;
            this.radioButtonOutputRight.Text = "Right";
            this.radioButtonOutputRight.UseVisualStyleBackColor = true;
            this.radioButtonOutputRight.CheckedChanged += new System.EventHandler(this.radioButtonOutput_CheckedChanged);
            // 
            // radioButtonOutputLeft
            // 
            this.radioButtonOutputLeft.AutoSize = true;
            this.radioButtonOutputLeft.Checked = true;
            this.radioButtonOutputLeft.Location = new System.Drawing.Point(25, 64);
            this.radioButtonOutputLeft.Name = "radioButtonOutputLeft";
            this.radioButtonOutputLeft.Size = new System.Drawing.Size(43, 17);
            this.radioButtonOutputLeft.TabIndex = 1;
            this.radioButtonOutputLeft.TabStop = true;
            this.radioButtonOutputLeft.Text = "Left";
            this.radioButtonOutputLeft.UseVisualStyleBackColor = true;
            this.radioButtonOutputLeft.CheckedChanged += new System.EventHandler(this.radioButtonOutput_CheckedChanged);
            // 
            // comboBoxWaveOut
            // 
            this.comboBoxWaveOut.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxWaveOut.FormattingEnabled = true;
            this.comboBoxWaveOut.Location = new System.Drawing.Point(6, 29);
            this.comboBoxWaveOut.Name = "comboBoxWaveOut";
            this.comboBoxWaveOut.Size = new System.Drawing.Size(245, 21);
            this.comboBoxWaveOut.TabIndex = 0;
            this.comboBoxWaveOut.SelectedIndexChanged += new System.EventHandler(this.comboBoxWaveOut_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(18, 75);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "&Frequency:";
            // 
            // numericUpDownFrequency
            // 
            this.numericUpDownFrequency.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.numericUpDownFrequency.Location = new System.Drawing.Point(92, 71);
            this.numericUpDownFrequency.Maximum = new decimal(new int[] {
            5000,
            0,
            0,
            0});
            this.numericUpDownFrequency.Minimum = new decimal(new int[] {
            200,
            0,
            0,
            0});
            this.numericUpDownFrequency.Name = "numericUpDownFrequency";
            this.numericUpDownFrequency.Size = new System.Drawing.Size(66, 20);
            this.numericUpDownFrequency.TabIndex = 4;
            this.numericUpDownFrequency.Value = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            // 
            // buttonXmitNow
            // 
            this.buttonXmitNow.Location = new System.Drawing.Point(9, 19);
            this.buttonXmitNow.Name = "buttonXmitNow";
            this.buttonXmitNow.Size = new System.Drawing.Size(110, 23);
            this.buttonXmitNow.TabIndex = 0;
            this.buttonXmitNow.Text = "&Now";
            this.buttonXmitNow.UseVisualStyleBackColor = true;
            this.buttonXmitNow.Click += new System.EventHandler(this.buttonXmitNow_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.buttonToFile);
            this.groupBox2.Controls.Add(this.buttonEven);
            this.groupBox2.Controls.Add(this.buttonOdd);
            this.groupBox2.Controls.Add(this.buttonXmitNow);
            this.groupBox2.Location = new System.Drawing.Point(299, 80);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(157, 168);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Transmit message audio";
            // 
            // buttonToFile
            // 
            this.buttonToFile.Location = new System.Drawing.Point(70, 135);
            this.buttonToFile.Name = "buttonToFile";
            this.buttonToFile.Size = new System.Drawing.Size(75, 23);
            this.buttonToFile.TabIndex = 3;
            this.buttonToFile.Text = "To File";
            this.buttonToFile.UseVisualStyleBackColor = true;
            this.buttonToFile.Click += new System.EventHandler(this.buttonToFile_Click);
            // 
            // buttonEven
            // 
            this.buttonEven.Location = new System.Drawing.Point(9, 77);
            this.buttonEven.Name = "buttonEven";
            this.buttonEven.Size = new System.Drawing.Size(110, 23);
            this.buttonEven.TabIndex = 2;
            this.buttonEven.Text = "On &Even interval";
            this.buttonEven.UseVisualStyleBackColor = true;
            this.buttonEven.Click += new System.EventHandler(this.buttonEven_Click);
            // 
            // buttonOdd
            // 
            this.buttonOdd.Location = new System.Drawing.Point(9, 48);
            this.buttonOdd.Name = "buttonOdd";
            this.buttonOdd.Size = new System.Drawing.Size(110, 23);
            this.buttonOdd.TabIndex = 1;
            this.buttonOdd.Text = "On &Odd interval";
            this.buttonOdd.UseVisualStyleBackColor = true;
            this.buttonOdd.Click += new System.EventHandler(this.buttonOdd_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(18, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Try too &send:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(255, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(172, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "After pack and unpack at receiver:";
            // 
            // TransmitForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonAbort;
            this.ClientSize = new System.Drawing.Size(592, 255);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.numericUpDownFrequency);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.labelXmit);
            this.Controls.Add(this.textBoxXmit);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "TransmitForm";
            this.Text = "XDft8Test Transmitter";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TransmitForm_FormClosed);
            this.Load += new System.EventHandler(this.TransmitForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownFrequency)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxXmit;
        private System.Windows.Forms.Label labelXmit;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox comboBoxWaveOut;
        private System.Windows.Forms.RadioButton radioButtonOutputMono;
        private System.Windows.Forms.RadioButton radioButtonOutputRight;
        private System.Windows.Forms.RadioButton radioButtonOutputLeft;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown numericUpDownFrequency;
        private System.Windows.Forms.Button buttonXmitNow;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button buttonEven;
        private System.Windows.Forms.Button buttonOdd;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonAbort;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button buttonToFile;
    }
}