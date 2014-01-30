namespace SrvCfg
{
    partial class AddParser
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
            this.comboBoxProto = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.textBoxPort = new System.Windows.Forms.NumericUpDown();
            this.textBoxTimeout = new System.Windows.Forms.NumericUpDown();
            this.textBoxLibrary = new System.Windows.Forms.TextBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.libOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.fileNameButton = new System.Windows.Forms.Button();
            this.comboBoxParser = new System.Windows.Forms.ComboBox();
            this.comboBoxCallback = new System.Windows.Forms.ComboBox();
            ((System.ComponentModel.ISupportInitialize)(this.textBoxPort)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.textBoxTimeout)).BeginInit();
            this.SuspendLayout();
            // 
            // comboBoxProto
            // 
            this.comboBoxProto.FormattingEnabled = true;
            this.comboBoxProto.Items.AddRange(new object[] {
            "TCP"});
            this.comboBoxProto.Location = new System.Drawing.Point(64, 89);
            this.comboBoxProto.Name = "comboBoxProto";
            this.comboBoxProto.Size = new System.Drawing.Size(45, 21);
            this.comboBoxProto.TabIndex = 0;
            this.comboBoxProto.Text = "TCP";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 92);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(46, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Protocol";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(129, 92);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(26, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Port";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 9);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 13);
            this.label3.TabIndex = 3;
            this.label3.Text = "Library";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(78, 13);
            this.label4.TabIndex = 4;
            this.label4.Text = "Parser function";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 61);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(89, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "Callback function";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(239, 92);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(47, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "TimeOut";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(350, 92);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(14, 13);
            this.label7.TabIndex = 7;
            this.label7.Text = "S";
            // 
            // textBoxPort
            // 
            this.textBoxPort.Location = new System.Drawing.Point(161, 90);
            this.textBoxPort.Maximum = new decimal(new int[] {
            65535,
            0,
            0,
            0});
            this.textBoxPort.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.textBoxPort.Name = "textBoxPort";
            this.textBoxPort.Size = new System.Drawing.Size(58, 20);
            this.textBoxPort.TabIndex = 9;
            this.textBoxPort.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // textBoxTimeout
            // 
            this.textBoxTimeout.Location = new System.Drawing.Point(292, 89);
            this.textBoxTimeout.Maximum = new decimal(new int[] {
            86400,
            0,
            0,
            0});
            this.textBoxTimeout.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.textBoxTimeout.Name = "textBoxTimeout";
            this.textBoxTimeout.Size = new System.Drawing.Size(58, 20);
            this.textBoxTimeout.TabIndex = 11;
            this.textBoxTimeout.Value = new decimal(new int[] {
            120,
            0,
            0,
            0});
            // 
            // textBoxLibrary
            // 
            this.textBoxLibrary.Location = new System.Drawing.Point(107, 6);
            this.textBoxLibrary.Name = "textBoxLibrary";
            this.textBoxLibrary.Size = new System.Drawing.Size(222, 20);
            this.textBoxLibrary.TabIndex = 15;
            // 
            // btnOK
            // 
            this.btnOK.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(184, 121);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 16;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(265, 121);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 17;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // libOpenFileDialog
            // 
            this.libOpenFileDialog.Filter = "\"libraries (*.dll)|*.dll|All files (*.*)|*.*\"";
            // 
            // fileNameButton
            // 
            this.fileNameButton.Location = new System.Drawing.Point(335, 6);
            this.fileNameButton.Name = "fileNameButton";
            this.fileNameButton.Size = new System.Drawing.Size(27, 20);
            this.fileNameButton.TabIndex = 18;
            this.fileNameButton.Text = "...";
            this.fileNameButton.UseVisualStyleBackColor = true;
            this.fileNameButton.Click += new System.EventHandler(this.fileNameButton_Click);
            // 
            // comboBoxParser
            // 
            this.comboBoxParser.FormattingEnabled = true;
            this.comboBoxParser.Location = new System.Drawing.Point(107, 31);
            this.comboBoxParser.Name = "comboBoxParser";
            this.comboBoxParser.Size = new System.Drawing.Size(257, 21);
            this.comboBoxParser.TabIndex = 19;
            // 
            // comboBoxCallback
            // 
            this.comboBoxCallback.FormattingEnabled = true;
            this.comboBoxCallback.Location = new System.Drawing.Point(107, 58);
            this.comboBoxCallback.Name = "comboBoxCallback";
            this.comboBoxCallback.Size = new System.Drawing.Size(257, 21);
            this.comboBoxCallback.TabIndex = 20;
            // 
            // AddParser
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(374, 153);
            this.Controls.Add(this.comboBoxCallback);
            this.Controls.Add(this.comboBoxParser);
            this.Controls.Add(this.fileNameButton);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.textBoxLibrary);
            this.Controls.Add(this.textBoxTimeout);
            this.Controls.Add(this.textBoxPort);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comboBoxProto);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AddParser";
            this.ShowIcon = false;
            this.Text = "Add parser";
            ((System.ComponentModel.ISupportInitialize)(this.textBoxPort)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.textBoxTimeout)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.ComboBox comboBoxProto;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        public System.Windows.Forms.TextBox textBoxLibrary;
        public System.Windows.Forms.NumericUpDown textBoxPort;
        public System.Windows.Forms.NumericUpDown textBoxTimeout;
        private System.Windows.Forms.OpenFileDialog libOpenFileDialog;
        private System.Windows.Forms.Button fileNameButton;
        public System.Windows.Forms.ComboBox comboBoxCallback;
        public System.Windows.Forms.ComboBox comboBoxParser;

    }
}