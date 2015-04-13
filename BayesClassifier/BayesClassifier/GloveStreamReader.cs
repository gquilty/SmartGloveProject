using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Globalization;
using Disruptor;
using Disruptor.Dsl;

namespace GloveApplication
{
    class GloveStreamReader
    {
        static int iSizeOfOneReading = 10;
        static int iNoOfRecs = 12;
        static public int iTOPVAL = iSizeOfOneReading * iNoOfRecs + 6;

        DisruptorRingBuffer buffer = new DisruptorRingBuffer();

        public void readData()
        {
            // Create a new SerialPort object with default settings.
            SerialPort _serialPort = new SerialPort();
            string srtrBuildPort = "COM8";
            _serialPort.PortName = srtrBuildPort;
            _serialPort.BaudRate = 1000000;
            _serialPort.Parity = Parity.None;
            _serialPort.DataBits = 8;
            _serialPort.StopBits = StopBits.One;
            _serialPort.Handshake = Handshake.None;

            _serialPort.ReadTimeout = 1500;
            _serialPort.WriteTimeout = 500;

            try
            {
                _serialPort.Open();
                //Console.Write("Port 6 open");
            }
            catch (Exception ex)
            {
                _serialPort.Close();
                _serialPort.Open();
            }

            //char[] arrayOfInts = new char[iTOPVAL*5]
            char[] arrayOfInts = new char[iTOPVAL];

            while (true)
            {
                char chkStart = (char)_serialPort.ReadChar();
                if (chkStart == '*')
                {
                    for (int iIndex = 0; iIndex < iTOPVAL; iIndex++)
                    { // iTopValue*5 <<< avgLayer1
                        arrayOfInts[iIndex] = (char)_serialPort.ReadChar();
                    }

                    string str = new string(arrayOfInts);
                    //Console.Write(str);

                    buffer.run(str);
                }
            }
        }
    }
}
