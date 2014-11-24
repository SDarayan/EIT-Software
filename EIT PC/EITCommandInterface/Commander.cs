using System;
using System.Collections.Generic;
using System.Text;
using EIT;
using System.Drawing;

namespace EIT
{
    public class Commander
    {
        //STILL NEED TO MAKE CONFIG FOR BOTH STIM TYPES 
        private const int STIM_TYPE = 1;
        private const byte COM_TAKE_IMAGE = 0x64;
        private const byte COM_PING = 0;
        private const byte COM_PING_JUNK = 0x01;
        private const byte COM_TAKE_SIM = 0x73;
        private const byte COM_STREAM_NEXT = 0x4E;
        private const byte COM_OPEN_STREAM1 = 0x31;
        private const byte COM_OPEN_STREAM2 = 0x32;
        public static byte[] LastResult;
        public static Bitmap CreateImage(int ComPort)
        {
            StartStream(ComPort);
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_STREAM_NEXT);
            LastResult = data;
            return Parser.CreateImage(data);
            
        }
        public static void StartStream(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse((STIM_TYPE==1?COM_OPEN_STREAM1:COM_OPEN_STREAM2));
        }
        public static Bitmap GetStreamImage(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_STREAM_NEXT);
            LastResult = data;
            return Parser.CreateImage(data);
        }
        public static Bitmap CreateSimulatedImage(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_TAKE_SIM);
            LastResult = data;
            return Parser.CreateImage(data);

        }
        public static byte[] CreateDataSet(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_TAKE_IMAGE);
            LastResult = data;
            return data;
        }
        public static byte[] Ping(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_PING);
            LastResult = data;
            return data;
        }
        public static byte[] PingJunk(int ComPort)
        {
            SerialConnector serial = new SerialConnector();
            serial.Port = "COM" + ComPort;
            byte[] data = serial.SendDataWaitResponse(COM_PING_JUNK);
            LastResult = data;
            return data;
        }
    }
   
}
