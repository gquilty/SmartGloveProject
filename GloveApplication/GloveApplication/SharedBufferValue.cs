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
    public sealed class SharedBufferValue
    {
        public DataSnapshot Value { get; set; }

        public SharedBufferValue()
        {
            //Console.WriteLine("New ValueEntry created");
        }
    }
}
