namespace Leet.UI.DuiKit.Compiler.WinForms
{
    partial class MainForm
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
            this.buttonBinToXml = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.labelStatus = new System.Windows.Forms.Label();
            this.buttonXmlToBin = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonBinToXml
            // 
            this.buttonBinToXml.Location = new System.Drawing.Point(12, 12);
            this.buttonBinToXml.Name = "buttonBinToXml";
            this.buttonBinToXml.Size = new System.Drawing.Size(165, 23);
            this.buttonBinToXml.TabIndex = 0;
            this.buttonBinToXml.Text = "Convert DUIB to DUIXML";
            this.buttonBinToXml.UseVisualStyleBackColor = true;
            this.buttonBinToXml.Click += new System.EventHandler(this.buttonBinToXml_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(183, 17);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(40, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Status:";
            // 
            // labelStatus
            // 
            this.labelStatus.AutoSize = true;
            this.labelStatus.Location = new System.Drawing.Point(183, 40);
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Size = new System.Drawing.Size(24, 13);
            this.labelStatus.TabIndex = 2;
            this.labelStatus.Text = "Idle";
            // 
            // buttonXmlToBin
            // 
            this.buttonXmlToBin.Location = new System.Drawing.Point(12, 40);
            this.buttonXmlToBin.Name = "buttonXmlToBin";
            this.buttonXmlToBin.Size = new System.Drawing.Size(165, 23);
            this.buttonXmlToBin.TabIndex = 3;
            this.buttonXmlToBin.Text = "Convert DUIXML to DUIB";
            this.buttonXmlToBin.UseVisualStyleBackColor = true;
            this.buttonXmlToBin.Click += new System.EventHandler(this.buttonXmlToBin_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(382, 29);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(0, 13);
            this.label2.TabIndex = 4;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.ClientSize = new System.Drawing.Size(450, 91);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.buttonXmlToBin);
            this.Controls.Add(this.labelStatus);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonBinToXml);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Form1";
            this.Padding = new System.Windows.Forms.Padding(12);
            this.ShowIcon = false;
            this.Text = "Leet\'s DUI compiler";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonBinToXml;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label labelStatus;
        private System.Windows.Forms.Button buttonXmlToBin;
        private System.Windows.Forms.Label label2;
    }
}

