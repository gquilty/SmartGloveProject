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
using System.Windows.Forms;

namespace GloveApplication
{
    /// <summary>
    /// The GloveStreamReader class is used to continuously pull data  from the glove and organise
    /// the data into concise packets for the averaging system and gesture recognition
    /// </summary>
    public class GloveStreamReader
    {
        //Create a new Ring Buffer suplied by the Disruptor.Net library
        public DisruptorRingBuffer buffer = new DisruptorRingBuffer();

        /// <summary>
        /// readData() is the first method called in the namespace which begins reading from the serial I/O port
        /// continuously
        /// </summary>
        public void readData(int comPort)
        {
            // Create a new SerialPort object with default settings.
            /*SerialPort _serialPort = new SerialPort();
            string buildPort = "COM" + comPort;
            _serialPort.PortName = buildPort;
            _serialPort.BaudRate = 1000000;
            _serialPort.Parity = Parity.None;
            _serialPort.DataBits = 8;
            _serialPort.StopBits = StopBits.One;
            _serialPort.Handshake = Handshake.None;

            _serialPort.ReadTimeout = 15000;
            _serialPort.WriteTimeout = 500;

            try
            {
                _serialPort.Open();
            }
            catch (Exception ex)
            {
                _serialPort.Close();
                _serialPort.Open();
            }
<<<<<<< HEAD
            
=======
            */
>>>>>>> f5268adffb7b7b2aa63b531d7405c8dca61e6df4

            // Read line by line and pass to buffer
            while (true)
            {
                try
                {
                    //string value = _serialPort.ReadLine();
                    //buffer.run(value);
                    buffer.run("-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,");
                    //value = "";
                    Application.DoEvents();
                }
                catch (Exception ReadTimeoutExceptiom)
                {
                    Console.WriteLine("Timeout occured on Serial COM port.\t Trying again");
                    //_serialPort.Close();
                    //_serialPort.Open();
                }
                
            }
        }
    }
}
