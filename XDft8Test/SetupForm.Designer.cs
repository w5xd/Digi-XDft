namespace XDft8Test
{
    partial class SetupForm
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
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonOK = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxAppDataName = new System.Windows.Forms.TextBox();
            this.buttonAppData = new System.Windows.Forms.Button();
            this.textBoxSharedMemory = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.textBoxIntro = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxMyCall = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonCancel
            // 
            this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonCancel.Location = new System.Drawing.Point(504, 575);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 4;
            this.buttonCancel.Text = "&Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            // 
            // buttonOK
            // 
            this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonOK.Location = new System.Drawing.Point(603, 575);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 5;
            this.buttonOK.Text = "&OK";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 85);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(112, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Shared memory name:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 125);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(148, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "AppData/Local/-FolderName-";
            // 
            // textBoxAppDataName
            // 
            this.textBoxAppDataName.Location = new System.Drawing.Point(176, 125);
            this.textBoxAppDataName.Name = "textBoxAppDataName";
            this.textBoxAppDataName.Size = new System.Drawing.Size(100, 20);
            this.textBoxAppDataName.TabIndex = 4;
            this.textBoxAppDataName.Text = "XDft8Test-1";
            // 
            // buttonAppData
            // 
            this.buttonAppData.Location = new System.Drawing.Point(292, 125);
            this.buttonAppData.Name = "buttonAppData";
            this.buttonAppData.Size = new System.Drawing.Size(36, 23);
            this.buttonAppData.TabIndex = 5;
            this.buttonAppData.Text = "...";
            this.buttonAppData.UseVisualStyleBackColor = true;
            this.buttonAppData.Click += new System.EventHandler(this.buttonAppData_Click);
            // 
            // textBoxSharedMemory
            // 
            this.textBoxSharedMemory.Location = new System.Drawing.Point(158, 85);
            this.textBoxSharedMemory.Name = "textBoxSharedMemory";
            this.textBoxSharedMemory.Size = new System.Drawing.Size(100, 20);
            this.textBoxSharedMemory.TabIndex = 2;
            this.textBoxSharedMemory.Text = "XDft8Test-1";
            this.textBoxSharedMemory.TextChanged += new System.EventHandler(this.textBoxSharedMemory_TextChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.textBoxSharedMemory);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.buttonAppData);
            this.groupBox1.Controls.Add(this.textBoxAppDataName);
            this.groupBox1.Location = new System.Drawing.Point(12, 390);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(475, 208);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Multiple instance names";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(15, 155);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(412, 30);
            this.label5.TabIndex = 6;
            this.label5.Text = "A couple of directories in AppData/Local will be created by this program, and som" +
    "e files left in them when it runs.";
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(6, 36);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(444, 42);
            this.label4.TabIndex = 0;
            this.label4.Text = "Multiple concurrent instances of XdFt8Test must have unique names for the shared " +
    "memory segment and the folder in AppData/Local";
            // 
            // textBoxIntro
            // 
            this.textBoxIntro.Location = new System.Drawing.Point(12, 12);
            this.textBoxIntro.Multiline = true;
            this.textBoxIntro.Name = "textBoxIntro";
            this.textBoxIntro.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBoxIntro.Size = new System.Drawing.Size(666, 361);
            this.textBoxIntro.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(504, 439);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(43, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "My call:";
            // 
            // textBoxMyCall
            // 
            this.textBoxMyCall.Location = new System.Drawing.Point(553, 436);
            this.textBoxMyCall.Name = "textBoxMyCall";
            this.textBoxMyCall.Size = new System.Drawing.Size(68, 20);
            this.textBoxMyCall.TabIndex = 3;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(493, 400);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(185, 33);
            this.label6.TabIndex = 1;
            this.label6.Text = "In certain situations the FT8 decoder uses My call. It may be left blank.\r\n";
            // 
            // SetupForm
            // 
            this.AcceptButton = this.buttonOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonCancel;
            this.ClientSize = new System.Drawing.Size(690, 610);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.textBoxMyCall);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBoxIntro);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.buttonOK);
            this.Controls.Add(this.buttonCancel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetupForm";
            this.Text = "XDft8 Setup Parameters";
            this.Load += new System.EventHandler(this.SetupForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxAppDataName;
        private System.Windows.Forms.Button buttonAppData;
        private System.Windows.Forms.TextBox textBoxSharedMemory;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxIntro;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxMyCall;
        private System.Windows.Forms.Label label6;
    }
}