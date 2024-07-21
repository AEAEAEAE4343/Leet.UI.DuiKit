using System;
using System.IO;
using System.Windows.Forms;

namespace Leet.UI.DuiKit.Compiler.WinForms
{
    public partial class MainForm : Form
    {
        DuiBinarySerializer binarySerializer = new DuiBinarySerializer();
        DuiXmlSerializer xmlSerializer = new DuiXmlSerializer();

        public MainForm()
        {
            InitializeComponent();
        }

        private void buttonBinToXml_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Title = "Select input DUIB file...";
                ofd.Filter = "DUIB files (*.duib;*.bin)|*.duib;*.bin|All files (*.*)|*.*";
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    using (SaveFileDialog sfd = new SaveFileDialog())
                    {
                        sfd.Title = "Select output DUIXML file...";
                        sfd.Filter = "DUIXML file (*.duixml)|*.duixml|XML file (*.xml)|*.xml|All files (*.*)|*.*";
                        if (sfd.ShowDialog() == DialogResult.OK)
                        {
                            FileStream input = File.OpenRead(ofd.FileName);
                            FileStream output = File.OpenWrite(sfd.FileName);
                            try
                            {
                                DuiBinaryData data = binarySerializer.Deserialize(input);
                                xmlSerializer.Serialize(data, output);
                                labelStatus.Text = $"Successfully converted {ofd.FileName} to DUIXML!";
                            }
                            catch (Exception exception)
                            {
                                labelStatus.Text = $"Conversion failed.";
                                MessageBox.Show(exception.Message, "An error occurred");
                            }
                            input.Close();
                            output.Close();
                        }
                    }
                }
            }
        }

        private void buttonXmlToBin_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Title = "Select input DUIXML file...";
                ofd.Filter = "DUIXML files (*.duixml)|*.duixml|XML files (*.xml)|*.xml|All files (*.*)|*.*";
                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    using (SaveFileDialog sfd = new SaveFileDialog())
                    {
                        sfd.Title = "Select output DUIB file...";
                        sfd.Filter = "DUIB file (*.duib;*.bin)|*.duib;*.bin|All files (*.*)|*.*";
                        if (sfd.ShowDialog() == DialogResult.OK)
                        {
                            FileStream input = File.OpenRead(ofd.FileName);
                            FileStream output = File.OpenWrite(sfd.FileName);
                            try
                            {
                                DuiBinaryData data = xmlSerializer.Deserialize(input);
                                binarySerializer.Serialize(data, output);
                                labelStatus.Text = $"Successfully converted {ofd.FileName} to DUIXML!";
                            }
                            catch (Exception exception)
                            {
                                labelStatus.Text = $"Conversion failed.";
                                MessageBox.Show(exception.Message, "An error occurred");
                            }
                            input.Close();
                            output.Close();
                        }
                    }
                }
            }
        }
    }
}
