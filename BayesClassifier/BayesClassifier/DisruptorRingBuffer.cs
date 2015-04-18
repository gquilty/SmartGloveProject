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

    class DisruptorRingBuffer
    {
        private static readonly int _ringSize = 1 << 11;  // Must be multiple of 2
        private Disruptor.Dsl.Disruptor<ValueEntry>  disruptor;
        private RingBuffer<ValueEntry> ringBuffer;

        public DisruptorRingBuffer()
        {
            disruptor = new Disruptor.Dsl.Disruptor<ValueEntry>(() => new ValueEntry(), _ringSize, TaskScheduler.Default);

            DataCommunicationHandler dch = new DataCommunicationHandler();
            dch.avgLayer = new MovingAverageFilter();
            disruptor.HandleEventsWith(dch);
            ringBuffer = disruptor.Start();
        }

        public void run(string averagedArrayInput)
        {

            while (true)
            {
                long sequenceNo = ringBuffer.Next();

                ValueEntry entry = ringBuffer[sequenceNo];

                entry.Value = averagedArrayInput;

                ringBuffer.Publish(sequenceNo);

                //Console.WriteLine("Published entry {0}, value {1}", sequenceNo, entry.Value);
                break;

            }

        }
    }
}
