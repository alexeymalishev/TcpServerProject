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
    public partial class SrvCfg : Form
    {
        [DllImport("GetExportsNames.dll",
                 EntryPoint = "_Signal@0",
                 ExactSpelling = true,
                 CallingConvention = CallingConvention.StdCall)]
        public static extern void Signal();

        public SrvCfg()
        {
            InitializeComponent();
            try
            {
                Microsoft.Win32.RegistryKey key;
                key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters");
                //key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters");

                if (null != key)
                {
                    int nNumberOfParsers = (int)key.GetValue("NumberOfParsers", 0);

                    key.Close();

                    for (int ii = 0; ii < nNumberOfParsers; ++ii)
                    {
                        key = Microsoft.Win32.Registry.LocalMachine.OpenSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters\\Parser_" + ii);
                        //key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters\\Parser_" + ii);
                        if (null != key)
                        {
                            string lpValue;
                            lpValue = (string)key.GetValue("Protocol", "");
                            System.Windows.Forms.ListViewItem lvi = listViewParsers.Items.Add(lpValue);

                            lpValue = (string)key.GetValue("PortNumber", "");
                            lvi.SubItems.Add(lpValue);

                            lpValue = (string)key.GetValue("LibraryName", "");
                            lvi.SubItems.Add(lpValue);

                            lpValue = (string)key.GetValue("ParserName", "");
                            lvi.SubItems.Add(lpValue);

                            lpValue = (string)key.GetValue("CallbackName", "");
                            lvi.SubItems.Add(lpValue);

                            lpValue = (string)key.GetValue("TimeOut", "");
                            lvi.SubItems.Add(lpValue);
                            key.Close();
                        }
                    }
                }
                else
                {
                    System.Windows.Forms.MessageBox.Show("Section HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\TcpServer\\parameters not exist", "Registry error",
                        System.Windows.Forms.MessageBoxButtons.OK, System.Windows.Forms.MessageBoxIcon.Error);
                }

            }
            catch (System.Exception e)
            {
                // AAAAAAAAAAARGH, an error!
                System.Windows.Forms.MessageBox.Show(e.Message,
                            "Reading registry"
                            , System.Windows.Forms.MessageBoxButtons.OK
                            , System.Windows.Forms.MessageBoxIcon.Error);
                //                return null;
            }
        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            DialogResult dr = new DialogResult();
            AddParser addParser = new AddParser();

            addParser.comboBoxProto.Text = "TCP";

            dr = addParser.ShowDialog();
            if (dr == System.Windows.Forms.DialogResult.OK) 
            { 
                string lpValue = addParser.comboBoxProto.Text;
                System.Windows.Forms.ListViewItem lvi = listViewParsers.Items.Add(lpValue);

                lpValue = addParser.textBoxPort.Value.ToString();
                lvi.SubItems.Add(lpValue);

                lpValue = addParser.textBoxLibrary.Text;
                lvi.SubItems.Add(lpValue);

                lpValue = addParser.comboBoxParser.Text;
                lvi.SubItems.Add(lpValue);

                lpValue = addParser.comboBoxCallback.Text;
                lvi.SubItems.Add(lpValue);

                lpValue = addParser.textBoxTimeout.Value.ToString();
                lvi.SubItems.Add(lpValue);
            }
        }

        private void buttonChange_Click(object sender, EventArgs e)
        {
            DialogResult dr = new DialogResult(); 
            AddParser addParser = new AddParser();
            addParser.Text = "Change parser";

            ListView.SelectedListViewItemCollection slvi = listViewParsers.SelectedItems;

            if (0 != slvi.Count )
            {
                try
                {
                    addParser.comboBoxProto.Text = slvi[0].Text;
                    addParser.textBoxPort.Value = int.Parse(slvi[0].SubItems[1].Text);
                    addParser.textBoxLibrary.Text = slvi[0].SubItems[2].Text;
                    addParser.comboBoxParser.Text = slvi[0].SubItems[3].Text;
                    addParser.comboBoxCallback.Text = slvi[0].SubItems[4].Text;
                    addParser.textBoxTimeout.Value = int.Parse(slvi[0].SubItems[5].Text);

                    addParser.getExports();

                    dr = addParser.ShowDialog();

                    if (dr == System.Windows.Forms.DialogResult.OK)
                    {
                        string lpValue = addParser.comboBoxProto.Text;
                        slvi[0].Text = lpValue;

                        lpValue = addParser.textBoxPort.Value.ToString();
                        slvi[0].SubItems[1].Text = lpValue;

                        lpValue = addParser.textBoxLibrary.Text;
                        slvi[0].SubItems[2].Text = lpValue;

                        lpValue = addParser.comboBoxParser.Text;
                        slvi[0].SubItems[3].Text = lpValue;

                        lpValue = addParser.comboBoxCallback.Text;
                        slvi[0].SubItems[4].Text = lpValue;

                        lpValue = addParser.textBoxTimeout.Value.ToString();
                        slvi[0].SubItems[5].Text = lpValue;
                    }
                }
                catch (System.Exception ex)
                {
                    // AAAAAAAAAAARGH, an error!
                    System.Windows.Forms.MessageBox.Show(ex.Message,
                                "Entry parse"
                                , System.Windows.Forms.MessageBoxButtons.OK
                                , System.Windows.Forms.MessageBoxIcon.Error);
                    //                return null;
                }
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("Item not selected",
                            "Entry parse"
                            , System.Windows.Forms.MessageBoxButtons.OK
                            , System.Windows.Forms.MessageBoxIcon.Error);
            }

        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            ListView.SelectedListViewItemCollection slvi = listViewParsers.SelectedItems;

            if (0 != slvi.Count)
            {
                slvi[0].Remove();
            }
            else
            {
                System.Windows.Forms.MessageBox.Show("Item not selected",
                            "Entry parse"
                            , System.Windows.Forms.MessageBoxButtons.OK
                            , System.Windows.Forms.MessageBoxIcon.Error);
            }
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            Microsoft.Win32.RegistryKey key;
            key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters");
            //key = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters");

            key.SetValue("NumberOfParsers", listViewParsers.Items.Count);

            key.Close();

            for (int ii = 0; ii < listViewParsers.Items.Count; ++ii)
            {
                key = Microsoft.Win32.Registry.LocalMachine.CreateSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters\\Parser_" + ii);
                //key = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("System\\CurrentControlSet\\Services\\TcpServer\\parameters\\Parser_" + ii);

                key.SetValue("Protocol", listViewParsers.Items[ii].Text);
                key.SetValue("PortNumber", listViewParsers.Items[ii].SubItems[1].Text);
                key.SetValue("LibraryName", listViewParsers.Items[ii].SubItems[2].Text);
                key.SetValue("ParserName", listViewParsers.Items[ii].SubItems[3].Text);
                key.SetValue("CallbackName", listViewParsers.Items[ii].SubItems[4].Text);
                key.SetValue("TimeOut", listViewParsers.Items[ii].SubItems[5].Text);

                key.Close();
            }

            //send signal to restart service
            Signal();

            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
