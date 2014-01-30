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

namespace SrvCfg
{
    public partial class AddParser : Form
    {
        [DllImport("GetExportsNames.dll",
                        EntryPoint = "_GetDLLFileExports@8",
                        ExactSpelling = true,
                        CallingConvention = CallingConvention.StdCall)]
        public static extern int GetDLLFileExports(string szFileName, ref IntPtr data);
        

        public AddParser()
        {
            InitializeComponent();
        }

        public void getExports()
        {
            this.comboBoxParser.Items.Clear();
            this.comboBoxCallback.Items.Clear();

            IntPtr ptr_start = IntPtr.Zero;
            int count = GetDLLFileExports(this.textBoxLibrary.Text, ref ptr_start);

            IntPtr[] ptr_strings = new IntPtr[count];

            Marshal.Copy(ptr_start, ptr_strings, 0, count);
            for (int i = 0; i < count; i++)
            {
                this.comboBoxParser.Items.Add(Marshal.PtrToStringAnsi(ptr_strings[i]));
                this.comboBoxCallback.Items.Add(Marshal.PtrToStringAnsi(ptr_strings[i]));
            }


        }

        private void fileNameButton_Click(object sender, EventArgs e)
        {
            libOpenFileDialog.FileName = this.textBoxLibrary.Text;

            if (libOpenFileDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                this.textBoxLibrary.Text = libOpenFileDialog.FileName;
                getExports();
            }
        }
    }
}
