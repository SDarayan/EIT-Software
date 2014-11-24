using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.Threading;

namespace EIT
{
    //made with tutorial from http://www.dreamincode.net/forums/showtopic35775.htm
    public class SerialConnector
    {
        public static bool IsSimulation = false;
        public static int BaseSample = 0;
        private const bool IS_SERIAL_DATA_SQUASHED = false;
        private const int DEFAULT_BAUD = 115200;
        private const Parity DEFAULT_PAIR = Parity.None;
        private const StopBits DEFAULT_STOP = StopBits.One;
        private const int DEFAULT_DATA = 8;
        private const string DEFAULT_PORT = "COM1";
        private SerialPort _port;
        private string _portName = DEFAULT_PORT;
        public string Port
        {
            get
            {
               return _portName;
            }
            set
            {
                _portName = value;
            }
        }

        public SerialConnector()
        {
            _port = new SerialPort();
            //InitializePort(DEFAULT_BAUD, DEFAULT_PAIR, DEFAULT_STOP, DEFAULT_DATA, DEFAULT_PORT);
        }
        public void InitializePort(int baudrate, Parity parity, StopBits stop, int data, string port)
        {
            if (_port.IsOpen) _port.Close();
            _port.BaudRate = baudrate;
            _port.Parity = parity;
            _port.StopBits = stop;
            _port.DataBits = data;
            _port.PortName = port;
            _port.Open();
        }
        public byte[] SendDataWaitResponse(byte command)
        {
            //if just testing, send back a sample data set
            if(IsSimulation)
            {
                Parser.DiffBase = Rotation(Parser.SIM_CALIB, ((BaseSample % 16) * 13));
                return Rotation(new byte[] { 15, 42, 6, 141, 3, 228, 2, 198, 2, 60, 1, 252, 1, 232, 1, 250, 2, 60, 2, 202, 3, 242, 6, 160, 15, 87, 15, 59, 6, 166, 3, 253, 2, 214, 2, 70, 1, 254, 1, 230, 1, 247, 2, 57, 2, 198, 3, 230, 6, 151, 15, 68, 15, 42, 15, 93, 6, 192, 4, 11, 2, 225, 2, 77, 2, 5, 1, 239, 2, 3, 2, 68, 2, 201, 3, 226, 6, 126, 6, 141, 15, 59, 15, 90, 6, 180, 4, 11, 2, 226, 2, 82, 2, 15, 1, 251, 2, 13, 2, 72, 2, 201, 3, 220, 3, 228, 6, 165, 15, 93, 15, 58, 6, 181, 4, 10, 2, 230, 2, 87, 2, 20, 1, 253, 2, 9, 2, 66, 2, 194, 2, 197, 3, 253, 6, 192, 15, 89, 15, 55, 6, 177, 4, 13, 2, 231, 2, 88, 2, 16, 1, 245, 2, 2, 2, 59, 2, 60, 2, 214, 4, 11, 6, 180, 15, 58, 15, 83, 6, 190, 4, 12, 2, 230, 2, 83, 2, 10, 1, 241, 1, 254, 1, 252, 2, 70, 2, 225, 4, 11, 6, 181, 15, 55, 15, 82, 6, 176, 4, 9, 2, 226, 2, 82, 2, 12, 1, 241, 1, 232, 1, 255, 2, 77, 2, 226, 4, 10, 6, 177, 15, 83, 15, 54, 6, 179, 4, 9, 2, 229, 2, 84, 2, 9, 1, 250, 1, 229, 2, 5, 2, 82, 2, 230, 4, 13, 6, 190, 15, 82, 15, 54, 6, 177, 4, 13, 2, 229, 2, 77, 2, 60, 1, 247, 1, 239, 2, 15, 2, 88, 2, 231, 4, 12, 6, 176, 15, 53, 15, 84, 6, 191, 4, 12, 2, 221, 2, 202, 2, 57, 2, 3, 1, 251, 2, 20, 2, 88, 2, 230, 4, 9, 6, 179, 15, 54, 15, 86, 6, 179, 4, 3, 3, 242, 2, 198, 2, 68, 2, 13, 1, 253, 2, 16, 2, 83, 2, 226, 4, 9, 6, 177, 15, 84, 15, 61, 6, 178, 6, 160, 3, 230, 2, 201, 2, 72, 2, 9, 1, 245, 2, 10, 2, 82, 2, 230, 4, 13, 6, 191, 15, 86, 15, 65, 15, 87, 6, 151, 3, 226, 2, 201, 2, 66, 2, 2, 1, 241, 2, 12, 2, 84, 2, 229, 4, 12, 6, 178, 15, 61, 15, 68, 6, 126, 3, 220, 2, 194, 2, 59, 1, 254, 1, 241, 2, 9, 2, 77, 2, 221, 4, 3, 6, 178, 15, 66 }, ((BaseSample%16) * 13)*2);
            }
            else
            {
                InitializePort(DEFAULT_BAUD, DEFAULT_PAIR, DEFAULT_STOP, DEFAULT_DATA, _portName);
                DateTime start = DateTime.Now;
                _port.Write(new byte[1] { command }, 0, 1);
                long count = 0;
                while (_port.BytesToRead != 512 && count++<3000)
                    Thread.Sleep(1);
                if (count > 3000)
                {
                    _port.Close();
                    throw new Exception("timeout");
                    
                }
                byte[] resp = new byte[512];
                _port.Read(resp, 0, 512);
                _port.Close();
                DateTime end = DateTime.Now;

                return resp;
            }
        }
        public static int[] ParseToDataset(byte[] data, int numsamples)
        {
            int[] toRet = new int[numsamples];
            int dataind =0;
            for (int x = 0; x < toRet.Length; x++)
            {
                //if serial data is packed as sequential 12 bit values, we need to desquash
                if (IS_SERIAL_DATA_SQUASHED)
                {
                    if (2 * x >= numsamples)
                        break;
                    toRet[2 * x] = (data[dataind++] << 4) + (data[dataind] >> 4);
                    if (2 * x + 1 >= numsamples)
                        break;
                    toRet[2 * x + 1] = ((((int)data[dataind++]) & 0x0F) << 8) + data[dataind++];
                }
                //if it is packed as the high 4 bits 0 inside a 16 bit number, no desquash
                else
                {
                    toRet[x] = (data[dataind++] << 8) + data[dataind++];
                }
            }
            return toRet;
        }
        private byte[] Rotation(byte[] Base, int number)
        {
            byte[] toRet = new byte[Base.Length];
            for (int x = 0; x < Base.Length; x++)
            {
                toRet[x] = Base[(number + x) % Base.Length];
            }
            return toRet;
        }
        private double[,] Rotation(double[,] Base, int number)
        {
            double[,] toRet = new double[Base.Length,1];
            for (int x = 0; x < Base.Length; x++)
            {
                toRet[x,0] = Base[(number + x) % Base.Length,0];
            }
            return toRet;
        }


    }
}
