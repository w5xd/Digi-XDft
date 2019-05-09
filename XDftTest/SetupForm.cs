using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace XDftTest
{
    public partial class SetupForm : Form
    {
        public SetupForm()
        {
            InitializeComponent();
        }

     
        private void buttonAppData_Click(object sender, EventArgs e)
        {
            String start =
                Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
            var fbd = new FolderBrowserDialog();
            fbd.SelectedPath = start;
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                string res = fbd.SelectedPath;
                if (res.StartsWith(start))
                {
                    res = res.Substring(start.Length+1);
                    textBoxAppDataName.Text = res;                }
            }
        }

        private void textBoxSharedMemory_TextChanged(object sender, EventArgs e)
        {
            if (textBoxSharedMemory.Text == "WSJT-X")
            {
                MessageBox.Show("The one name you may NOT choose is WSJT-X because that will interfere with the WSJT-X program");
                textBoxSharedMemory.Text = "";
            } else if (textBoxSharedMemory.Text.Contains(" "))
            {
                MessageBox.Show("The shared memory key may not have a space character");
                textBoxSharedMemory.Text = "";
            }
        }

        public String sharedMemoryKey;
        public String appDataName;
        public String myCall;
        public XDft.DigiMode digiMode = XDft.DigiMode.DIGI_FT8;

        private void SetupForm_Load(object sender, EventArgs e)
        {
            textBoxIntro.Text = "Copyright (c) 2019 by Wayne Wright, W5XD\r\n\r\n" +
                "This program is based on wsjtx-2.0.1 which is licensed software. " +
                "See the file COPYING.\r\n\r\n" +

"The algorithms, source code, look-and-feel of WSJT-X and related programs, and" +
" protocol specifications for the modes FSK441, FT8, JT4, JT6M JT9, JT65, JTMS, QRA64," +
" ISCAT, MSK144 are Copyright (C) 2001-2018 by one or more of the following authors:" +
" Joseph Taylor, K1JT; Bill Somerville, G4WJS; Steven Franke, K9AN; Nico Palermo," +
" IV3NWV; Grea Bream, KI7MT; Michael Black, W9MDB; Edson Pereira, PY2SDR; Philip Karn," +
" KA9Q; and other members of the WSJT Development Group.\r\n\r\n" +

"The assembly under test, named XDft.dll, is authored and published by Wayne, W5XD. " +
"This assembly presents a progammer's interface into a subset of the wsjtx feature set. In particular, " +
"it presents features that \r\n(a) modulate text into an audio stream and \r\n(b) demodulate an" +
" audio stream into text.\r\n" +
"Such a thing is also known as a \"modem\". \r\n\r\nThe assembly XDft.dll, in turns, depends on " +
"XDwsjtFT8.dll which is also by Wayne, W5XD. It is a recompile of selected unmodified " +
"sources from the published WSJT-X source download--just enough for a modem.\r\n\r\n" +
"This program, XDft8Test, is a test program who's target audience is software developers " +
"of Windows ham radio software that would like to add FT8 capability to their own product. " +
"It is in no way a complete ham radio application. " +
"The purpose of this kit is to demonstrate and test the underlying " +
".NET assemly, XDft.dll along with its support dlls. That assembly, XDft.dll is licensed under the " +
"same GPL as wsjtx-2.0.1 itself. That grants only certain permissions for software developers " +
"to distribute it.\r\n\r\n" +
"See COPYING. " +
"If you create a new program using XDft.dll, you should read this:\r\n" +
"https://www.gnu.org/licenses/gpl-faq.html#IfLibraryIsGPL";

            textBoxIntro.ReadOnly = true;

            if (!String.IsNullOrEmpty(sharedMemoryKey))
                textBoxSharedMemory.Text = sharedMemoryKey;
            if (!String.IsNullOrEmpty(appDataName))
                textBoxAppDataName.Text = appDataName;
            if (!String.IsNullOrEmpty(myCall))
                textBoxMyCall.Text = myCall;
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            sharedMemoryKey = textBoxSharedMemory.Text;        
            appDataName = textBoxAppDataName.Text;
            myCall = textBoxMyCall.Text;
            if  (appDataName.IndexOfAny("\\/|".ToCharArray()) != -1)
            {
                DialogResult = DialogResult.None;
                MessageBox.Show("-FolderName- may not contain any of these characters: \\/|");
                return;
            }
            if (String.IsNullOrEmpty(appDataName))
            {
                DialogResult = DialogResult.None;
                MessageBox.Show("Name to user for folder in AppData directory must be setup.");
                return;
            }
            if (radioButtonFt4.Checked)
                digiMode = XDft.DigiMode.DIGI_FT4;
        }
    }
}
