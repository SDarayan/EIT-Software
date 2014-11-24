namespace EIT
{
    partial class SerialTest
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
            this.label1 = new System.Windows.Forms.Label();
            this._sendButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this._responseLabel = new System.Windows.Forms.Label();
            this._pingButton = new System.Windows.Forms.RadioButton();
            this._junkButton = new System.Windows.Forms.RadioButton();
            this.label3 = new System.Windows.Forms.Label();
            this._portBox = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Enter Command";
            // 
            // _sendButton
            // 
            this._sendButton.Location = new System.Drawing.Point(417, 62);
            this._sendButton.Name = "_sendButton";
            this._sendButton.Size = new System.Drawing.Size(75, 23);
            this._sendButton.TabIndex = 2;
            this._sendButton.Text = "Send Command";
            this._sendButton.UseVisualStyleBackColor = true;
            this._sendButton.Click += new System.EventHandler(this._sendButton_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 86);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Response";
            // 
            // _responseLabel
            // 
            this._responseLabel.AutoSize = true;
            this._responseLabel.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._responseLabel.Location = new System.Drawing.Point(12, 115);
            this._responseLabel.Name = "_responseLabel";
            this._responseLabel.Size = new System.Drawing.Size(0, 14);
            this._responseLabel.TabIndex = 4;
            // 
            // _pingButton
            // 
            this._pingButton.AutoSize = true;
            this._pingButton.Location = new System.Drawing.Point(15, 25);
            this._pingButton.Name = "_pingButton";
            this._pingButton.Size = new System.Drawing.Size(46, 17);
            this._pingButton.TabIndex = 5;
            this._pingButton.TabStop = true;
            this._pingButton.Text = "Ping";
            this._pingButton.UseVisualStyleBackColor = true;
            // 
            // _junkButton
            // 
            this._junkButton.AutoSize = true;
            this._junkButton.Location = new System.Drawing.Point(15, 48);
            this._junkButton.Name = "_junkButton";
            this._junkButton.Size = new System.Drawing.Size(85, 17);
            this._junkButton.TabIndex = 6;
            this._junkButton.TabStop = true;
            this._junkButton.Text = "Ping w/Junk";
            this._junkButton.UseVisualStyleBackColor = true;
            this._junkButton.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(382, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "COM Port";
            // 
            // _portBox
            // 
            this._portBox.FormattingEnabled = true;
            this._portBox.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6"});
            this._portBox.Location = new System.Drawing.Point(385, 24);
            this._portBox.Name = "_portBox";
            this._portBox.Size = new System.Drawing.Size(121, 21);
            this._portBox.TabIndex = 8;
            this._portBox.SelectedIndexChanged += new System.EventHandler(this._portBox_SelectedIndexChanged);
            // 
            // SerialTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(697, 361);
            this.Controls.Add(this._portBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._junkButton);
            this.Controls.Add(this._pingButton);
            this.Controls.Add(this._responseLabel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._sendButton);
            this.Controls.Add(this.label1);
            this.Name = "SerialTest";
            this.Text = "SerialTest";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button _sendButton;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label _responseLabel;
        private System.Windows.Forms.RadioButton _pingButton;
        private System.Windows.Forms.RadioButton _junkButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox _portBox;
    }
}

