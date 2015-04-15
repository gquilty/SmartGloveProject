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
    class Program
    {
        
        public static void Main()
        {
            new Program().run();
        }

        public void run()
        {
            GloveStreamReader input = new GloveStreamReader();

            input.readData();
        }
    }
}
