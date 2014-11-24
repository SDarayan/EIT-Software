namespace EIT
{
    partial class EITMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EITMain));
            this._analysisDisplay = new System.Windows.Forms.PictureBox();
            this._startStopButton = new System.Windows.Forms.Button();
            this._tabControl = new System.Windows.Forms.TabControl();
            this._simulationTab = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this._simContButton = new System.Windows.Forms.RadioButton();
            this._simStaticButton = new System.Windows.Forms.RadioButton();
            this._measurementTab = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this._measContButton = new System.Windows.Forms.RadioButton();
            this._measDiffButton = new System.Windows.Forms.RadioButton();
            this._measSimButton = new System.Windows.Forms.RadioButton();
            this.label3 = new System.Windows.Forms.Label();
            this._measurementPortBox = new System.Windows.Forms.ComboBox();
            this._configurationTab = new System.Windows.Forms.TabPage();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this._configCalibButton = new System.Windows.Forms.RadioButton();
            this.label4 = new System.Windows.Forms.Label();
            this._configPortBox = new System.Windows.Forms.ComboBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this._statusLabel = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this._analysisDisplay)).BeginInit();
            this._tabControl.SuspendLayout();
            this._simulationTab.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this._measurementTab.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this._configurationTab.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // _analysisDisplay
            // 
            this._analysisDisplay.Location = new System.Drawing.Point(15, 12);
            this._analysisDisplay.Name = "_analysisDisplay";
            this._analysisDisplay.Size = new System.Drawing.Size(512, 512);
            this._analysisDisplay.TabIndex = 0;
            this._analysisDisplay.TabStop = false;
            // 
            // _startStopButton
            // 
            this._startStopButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 26F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this._startStopButton.Location = new System.Drawing.Point(651, 403);
            this._startStopButton.Name = "_startStopButton";
            this._startStopButton.Size = new System.Drawing.Size(341, 118);
            this._startStopButton.TabIndex = 4;
            this._startStopButton.Text = "Start/Stop";
            this._startStopButton.UseVisualStyleBackColor = true;
            this._startStopButton.Click += new System.EventHandler(this._startStopButton_Click);
            // 
            // _tabControl
            // 
            this._tabControl.Controls.Add(this._simulationTab);
            this._tabControl.Controls.Add(this._measurementTab);
            this._tabControl.Controls.Add(this._configurationTab);
            this._tabControl.Location = new System.Drawing.Point(647, 12);
            this._tabControl.Name = "_tabControl";
            this._tabControl.SelectedIndex = 0;
            this._tabControl.Size = new System.Drawing.Size(349, 188);
            this._tabControl.TabIndex = 11;
            // 
            // _simulationTab
            // 
            this._simulationTab.Controls.Add(this.groupBox1);
            this._simulationTab.Location = new System.Drawing.Point(4, 22);
            this._simulationTab.Name = "_simulationTab";
            this._simulationTab.Padding = new System.Windows.Forms.Padding(3);
            this._simulationTab.Size = new System.Drawing.Size(341, 162);
            this._simulationTab.TabIndex = 0;
            this._simulationTab.Text = "Simulation";
            this._simulationTab.UseVisualStyleBackColor = true;
            this._simulationTab.Enter += new System.EventHandler(this._simulationTab_Enter);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this._simContButton);
            this.groupBox1.Controls.Add(this._simStaticButton);
            this.groupBox1.Location = new System.Drawing.Point(6, 6);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(154, 100);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Functions";
            // 
            // _simContButton
            // 
            this._simContButton.AutoSize = true;
            this._simContButton.Location = new System.Drawing.Point(6, 42);
            this._simContButton.Name = "_simContButton";
            this._simContButton.Size = new System.Drawing.Size(145, 17);
            this._simContButton.TabIndex = 1;
            this._simContButton.TabStop = true;
            this._simContButton.Text = "Continuous Measurement";
            this._simContButton.UseVisualStyleBackColor = true;
            // 
            // _simStaticButton
            // 
            this._simStaticButton.AutoSize = true;
            this._simStaticButton.Location = new System.Drawing.Point(6, 19);
            this._simStaticButton.Name = "_simStaticButton";
            this._simStaticButton.Size = new System.Drawing.Size(119, 17);
            this._simStaticButton.TabIndex = 0;
            this._simStaticButton.TabStop = true;
            this._simStaticButton.Text = "Static Measurement";
            this._simStaticButton.UseVisualStyleBackColor = true;
            // 
            // _measurementTab
            // 
            this._measurementTab.Controls.Add(this.groupBox2);
            this._measurementTab.Controls.Add(this.label3);
            this._measurementTab.Controls.Add(this._measurementPortBox);
            this._measurementTab.Location = new System.Drawing.Point(4, 22);
            this._measurementTab.Name = "_measurementTab";
            this._measurementTab.Padding = new System.Windows.Forms.Padding(3);
            this._measurementTab.Size = new System.Drawing.Size(341, 162);
            this._measurementTab.TabIndex = 1;
            this._measurementTab.Text = "Measurement";
            this._measurementTab.UseVisualStyleBackColor = true;
            this._measurementTab.Enter += new System.EventHandler(this._measurementTab_Enter);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this._measContButton);
            this.groupBox2.Controls.Add(this._measDiffButton);
            this.groupBox2.Controls.Add(this._measSimButton);
            this.groupBox2.Location = new System.Drawing.Point(9, 46);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(158, 100);
            this.groupBox2.TabIndex = 13;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Functions";
            // 
            // _measContButton
            // 
            this._measContButton.AutoSize = true;
            this._measContButton.Location = new System.Drawing.Point(6, 65);
            this._measContButton.Name = "_measContButton";
            this._measContButton.Size = new System.Drawing.Size(145, 17);
            this._measContButton.TabIndex = 2;
            this._measContButton.TabStop = true;
            this._measContButton.Text = "Continuous Measurement";
            this._measContButton.UseVisualStyleBackColor = true;
            // 
            // _measDiffButton
            // 
            this._measDiffButton.AutoSize = true;
            this._measDiffButton.Location = new System.Drawing.Point(6, 42);
            this._measDiffButton.Name = "_measDiffButton";
            this._measDiffButton.Size = new System.Drawing.Size(142, 17);
            this._measDiffButton.TabIndex = 1;
            this._measDiffButton.TabStop = true;
            this._measDiffButton.Text = "Differential Measurement";
            this._measDiffButton.UseVisualStyleBackColor = true;
            // 
            // _measSimButton
            // 
            this._measSimButton.AutoSize = true;
            this._measSimButton.Location = new System.Drawing.Point(6, 19);
            this._measSimButton.Name = "_measSimButton";
            this._measSimButton.Size = new System.Drawing.Size(138, 17);
            this._measSimButton.TabIndex = 0;
            this._measSimButton.TabStop = true;
            this._measSimButton.Text = "Simulated Measurement";
            this._measSimButton.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 3);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 13);
            this.label3.TabIndex = 12;
            this.label3.Text = "COM Port";
            // 
            // _measurementPortBox
            // 
            this._measurementPortBox.FormattingEnabled = true;
            this._measurementPortBox.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6"});
            this._measurementPortBox.Location = new System.Drawing.Point(9, 19);
            this._measurementPortBox.Name = "_measurementPortBox";
            this._measurementPortBox.Size = new System.Drawing.Size(121, 21);
            this._measurementPortBox.TabIndex = 11;
            // 
            // _configurationTab
            // 
            this._configurationTab.Controls.Add(this.groupBox3);
            this._configurationTab.Controls.Add(this.label4);
            this._configurationTab.Controls.Add(this._configPortBox);
            this._configurationTab.Location = new System.Drawing.Point(4, 22);
            this._configurationTab.Name = "_configurationTab";
            this._configurationTab.Padding = new System.Windows.Forms.Padding(3);
            this._configurationTab.Size = new System.Drawing.Size(341, 162);
            this._configurationTab.TabIndex = 2;
            this._configurationTab.Text = "Configuration";
            this._configurationTab.UseVisualStyleBackColor = true;
            this._configurationTab.Enter += new System.EventHandler(this._configurationTab_Enter);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this._configCalibButton);
            this.groupBox3.Location = new System.Drawing.Point(9, 46);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(121, 100);
            this.groupBox3.TabIndex = 13;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Functions";
            // 
            // _configCalibButton
            // 
            this._configCalibButton.AutoSize = true;
            this._configCalibButton.Location = new System.Drawing.Point(6, 19);
            this._configCalibButton.Name = "_configCalibButton";
            this._configCalibButton.Size = new System.Drawing.Size(103, 17);
            this._configCalibButton.TabIndex = 0;
            this._configCalibButton.TabStop = true;
            this._configCalibButton.Text = "Calibrate System";
            this._configCalibButton.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 3);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 13);
            this.label4.TabIndex = 12;
            this.label4.Text = "COM Port";
            // 
            // _configPortBox
            // 
            this._configPortBox.FormattingEnabled = true;
            this._configPortBox.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6"});
            this._configPortBox.Location = new System.Drawing.Point(9, 19);
            this._configPortBox.Name = "_configPortBox";
            this._configPortBox.Size = new System.Drawing.Size(121, 21);
            this._configPortBox.TabIndex = 11;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this._statusLabel);
            this.groupBox5.Location = new System.Drawing.Point(651, 206);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(341, 191);
            this.groupBox5.TabIndex = 13;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Status";
            // 
            // _statusLabel
            // 
            this._statusLabel.AutoSize = true;
            this._statusLabel.Location = new System.Drawing.Point(13, 26);
            this._statusLabel.Name = "_statusLabel";
            this._statusLabel.Size = new System.Drawing.Size(0, 13);
            this._statusLabel.TabIndex = 0;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(533, 12);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(41, 320);
            this.pictureBox1.TabIndex = 14;
            this.pictureBox1.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(580, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 13);
            this.label1.TabIndex = 15;
            this.label1.Text = "Conductive";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(580, 319);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 13);
            this.label2.TabIndex = 16;
            this.label2.Text = "Resistive";
            // 
            // EITMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1008, 533);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this._tabControl);
            this.Controls.Add(this._startStopButton);
            this.Controls.Add(this._analysisDisplay);
            this.Name = "EITMain";
            this.Text = "Senior Design Project - EIT";
            ((System.ComponentModel.ISupportInitialize)(this._analysisDisplay)).EndInit();
            this._tabControl.ResumeLayout(false);
            this._simulationTab.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this._measurementTab.ResumeLayout(false);
            this._measurementTab.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this._configurationTab.ResumeLayout(false);
            this._configurationTab.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox _analysisDisplay;
        private System.Windows.Forms.Button _startStopButton;
        private System.Windows.Forms.TabControl _tabControl;
        private System.Windows.Forms.TabPage _simulationTab;
        private System.Windows.Forms.TabPage _measurementTab;
        private System.Windows.Forms.TabPage _configurationTab;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox _measurementPortBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox _configPortBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton _simContButton;
        private System.Windows.Forms.RadioButton _simStaticButton;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label _statusLabel;
        private System.Windows.Forms.RadioButton _measSimButton;
        private System.Windows.Forms.RadioButton _measDiffButton;
        private System.Windows.Forms.RadioButton _configCalibButton;
        private System.Windows.Forms.RadioButton _measContButton;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}

