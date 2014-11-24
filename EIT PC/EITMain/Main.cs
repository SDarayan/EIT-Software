using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace EIT
{
    public partial class EITMain : Form
    {
        private const int STIM_PATTERN = 1;
        private bool _isInCont = false;
        private bool _buttonFlag = false;
        public EITMain()
        {
            InitializeComponent();
        }

        private void _startStopButton_Click(object sender, EventArgs e)
        {
            if (_isInCont)
            {
                _buttonFlag = true;
                return;
            }
            //check what mode is selected, what function is selected, then what parameters are entered
            if (_tabControl.SelectedIndex == 0)
            {
                //Simulation
                SerialConnector.IsSimulation = true;
                if (_simContButton.Checked)
                {
                    //Simulation, Continuous
                    _isInCont = true;
                    while (true)
                    {
                        try
                        {
                            _statusLabel.Text = "Processing...";
                            Refresh();
                            Bitmap bmp = Commander.CreateSimulatedImage(0);
                            _analysisDisplay.Image = bmp;
                            int count = 0;
                            /*_responseLabel.Text = "";
                            foreach (byte b in Commander.LastResult)
                            {
                                count++;
                                _responseLabel.Text += b.ToString("X2") + " ";
                                if (count % 48 == 0)
                                {
                                    _responseLabel.Text += "\r\n";
                                }
                            }*/
                            _statusLabel.Text = "Idle";
                            Refresh();
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            _isInCont = false;
                            _buttonFlag = false;
                            return;
                        }
                        if (_buttonFlag)
                        {
                            _isInCont = false;
                            _buttonFlag = false;
                            return;
                        }
                        SerialConnector.BaseSample++;
                        Application.DoEvents();
                    }
                }
                else if (_simStaticButton.Checked)
                {
                    //Simulation, Static
                    try
                    {
                        _statusLabel.Text = "Processing...";
                        Refresh();
                        Bitmap bmp = Commander.CreateSimulatedImage(0);
                        _analysisDisplay.Image = bmp;
                        int count = 0;
                        /*_responseLabel.Text = "";
                        foreach (byte b in Commander.LastResult)
                        {
                            count++;
                            _responseLabel.Text += b.ToString("X2") + " ";
                            if (count % 48 == 0)
                            {
                                _responseLabel.Text += "\r\n";
                            }
                        }*/
                        _statusLabel.Text = "Idle";
                        Refresh();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }

                }
                else
                {
                    //error: the user needs to select at least one
                }
            }
            else if (_tabControl.SelectedIndex == 1)
            {
                if (_measSimButton.Checked)
                {
                    //Measurement
                    SerialConnector.IsSimulation = false;
                    //Serial Port
                    try
                    {
                        _statusLabel.Text = "Processing...";
                        Refresh();
                        Bitmap bmp = Commander.CreateSimulatedImage(_measurementPortBox.SelectedIndex + 1);
                        _analysisDisplay.Image = bmp;
                        int count = 0;
                        /*_responseLabel.Text = "";
                        foreach (byte b in Commander.LastResult)
                        {
                            count++;
                            _responseLabel.Text += b.ToString("X2") + " ";
                            if (count % 48 == 0)
                            {
                                _responseLabel.Text += "\r\n";
                            }
                        }*/
                        _statusLabel.Text = "Idle";
                        Refresh();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                else if (_measDiffButton.Checked)
                {
                    //Measurement
                    SerialConnector.IsSimulation = false;
                    //Serial Port
                    try
                    {
                        _statusLabel.Text = "Processing...";
                        Refresh();
                        Bitmap bmp = Commander.CreateImage(_measurementPortBox.SelectedIndex + 1);
                        _analysisDisplay.Image = bmp;
                        int count = 0;
                        /*_responseLabel.Text = "";
                        foreach (byte b in Commander.LastResult)
                        {
                            count++;
                            _responseLabel.Text += b.ToString("X2") + " ";
                            if (count % 48 == 0)
                            {
                                _responseLabel.Text += "\r\n";
                            }
                        }*/
                        _statusLabel.Text = "Idle";
                        Refresh();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                else if (_measContButton.Checked)
                {
                    _isInCont = true;
                    try
                    {

                       Commander.StartStream(_measurementPortBox.SelectedIndex + 1);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        _isInCont = false;
                        _buttonFlag = false;
                        return;
                    }
                    while (true)
                    {
                        try
                        {
                            _statusLabel.Text = "Processing...";
                            Refresh();
                            Bitmap bmp = Commander.GetStreamImage(_measurementPortBox.SelectedIndex+1);
                            _analysisDisplay.Image = bmp;
                            int count = 0;
                           /* _responseLabel.Text = "";
                            foreach (byte b in Commander.LastResult)
                            {
                                count++;
                                _responseLabel.Text += b.ToString("X2") + " ";
                                if (count % 48 == 0)
                                {
                                    _responseLabel.Text += "\r\n";
                                }
                            }*/
                            _statusLabel.Text = "Idle";
                            Refresh();
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            _isInCont = false;
                            _buttonFlag = false;
                            return;
                        }
                        if (_buttonFlag)
                        {
                            _isInCont = false;
                            _buttonFlag = false;
                            return;
                        }
                        //SerialConnector.BaseSample++;
                        Application.DoEvents();
                    }
                }

            }
            else
            {
                //Configuration
                SerialConnector.IsSimulation = false;
                //Serial Port
                if (_configCalibButton.Checked)
                {
                    try
                    {
                        //run calibration routine
                        double[][,] datas = new double[5][,];
                        for (int x = 0; x < 5; x++)
                        {
                            byte[] cur = Commander.CreateDataSet(_configPortBox.SelectedIndex + 1);
                            int[] parsedData = EIT.SerialConnector.ParseToDataset(cur, STIM_PATTERN==1?208:192);
                            double[,] finalData = new double[parsedData.Length, 1];
                            for (int y = 0; y < parsedData.Length; y++)
                            {
                                finalData[y, 0] = parsedData[y] / (Math.Pow(2, 12) - 1.0);
                            }
                            datas[x] = finalData;
                        }
                        double[,] final = new double[datas[0].Length, 1];
                        for (int x = 0; x < datas[0].Length; x++)
                        {
                            double max = 0;
                            int maxindex = 0;
                            double min = double.MaxValue;
                            int minindex = 0;
                            for (int y = 0; y < 5; y++)
                            {
                                if (datas[y][x, 0] > max)
                                {
                                    maxindex = y;
                                    max = datas[y][x, 0];
                                }
                                if (datas[y][x, 0] <= min)
                                {
                                    minindex = y;
                                    min = datas[y][x, 0];
                                }
                            }
                            double avg = 0;
                            for (int y = 0; y < 5; y++)
                            {
                                if (y == minindex || y == maxindex)
                                    continue;
                                else
                                    avg += datas[y][x, 0];
                            }
                            final[x, 0] = avg / 3;
                        }
                        Parser.DiffBase = final;
                        _statusLabel.Text = "Processing...";
                        Refresh();
                        Bitmap bmp = Commander.CreateImage(this._configPortBox.SelectedIndex + 1);
                        _analysisDisplay.Image = bmp;
                        int count = 0;
                        /*_responseLabel.Text = "";
                        foreach (byte b in Commander.LastResult)
                        {
                            count++;
                            _responseLabel.Text += b.ToString("X2") + " ";
                            if (count % 48 == 0)
                            {
                                _responseLabel.Text += "\r\n";
                            }
                        }*/
                        _statusLabel.Text = "Idle";
                        Refresh();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Exception occurred: " + ex.ToString(), "Exception", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        _isInCont = false;
                        _buttonFlag = false;
                        return;
                    }
                }
            }
            
        }

        private void _simulationTab_Enter(object sender, EventArgs e)
        {
            SerialConnector.IsSimulation = true;
        }

        private void _measurementTab_Enter(object sender, EventArgs e)
        {
            SerialConnector.IsSimulation = false;
        }

        private void _configurationTab_Enter(object sender, EventArgs e)
        {
            SerialConnector.IsSimulation = false;
        }

      

        
    }
}