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
    public class DataCommunicationHandler : IEventHandler<ValueEntry>
    {
        public MovingAverageFilter avgLayer;

        public void OnNext(ValueEntry data, long sequence, bool endOfBatch)
        {
            // Console.WriteLine("Event handled: Value = {0} (processed event {1}", data.Value, sequence);
            //MovingAverageFilter avgLayer = new MovingAverageFilter();
            avgLayer.movingAverage2(data.Value);
        }
    }
}
