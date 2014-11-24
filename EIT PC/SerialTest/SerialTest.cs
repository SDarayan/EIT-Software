using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace EIT
{
    public partial class SerialTest : Form
    {
        private SerialConnector _connector;
        public SerialTest()
        {
            InitializeComponent();

            _connector = new SerialConnector();
            _portBox.SelectedIndex = 0;
        }

        private void _sendButton_Click(object sender, EventArgs e)
        {
            byte command = 0x00;
            if (_pingButton.Checked)
                command = (byte)'p';
            if (_junkButton.Checked)
                command = (byte)'g';
            //byte command = BaseToDecimal(_commandBox.Text, 2);
            try
            {
                byte[] resp = _connector.SendDataWaitResponse(command);
                _responseLabel.Text = "";
                int count = 0;
                foreach (byte b in resp)
                {
                    count++;
                    _responseLabel.Text += b.ToString("X2") +" ";
                    if (count % 32 == 0)
                    {
                        _responseLabel.Text += "\r\n";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error in serial communication. Check COM port number");
            }
        }
        //taken from http://www.codeproject.com/KB/cs/balamurali_balaji.aspx
        const int base10 = 10;
        char[] cHexa = new char[] { 'A', 'B', 'C', 'D', 'E', 'F' };
        int[] iHexaNumeric = new int[] { 10, 11, 12, 13, 14, 15 };
        int[] iHexaIndices = new int[] { 0, 1, 2, 3, 4, 5 };
        const int asciiDiff = 48;
        private byte BaseToDecimal(string sBase, int numbase)
        {
            byte dec = 0;
            int b;
            int iProduct = 1;
            string sHexa = "";
            if (numbase > base10)
                for (int i = 0; i < cHexa.Length; i++)
                    sHexa += cHexa.GetValue(i).ToString();
            for (int i = sBase.Length - 1; i >= 0; i--, iProduct *= numbase)
            {
                string sValue = sBase[i].ToString();
                if (sValue.IndexOfAny(cHexa) >= 0)
                    b = iHexaNumeric[sHexa.IndexOf(sBase[i])];
                else
                    b = (int)sBase[i] - asciiDiff;
                dec += (byte)(b * iProduct);
            }
            return dec;
        }

        private void radioButton_CheckedChanged(object sender, EventArgs e)
        {
            //_sendButton.Enabled = true;
        }

        private void _portBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _connector.Port = (string)(_portBox.SelectedItem);
        }
    }
}