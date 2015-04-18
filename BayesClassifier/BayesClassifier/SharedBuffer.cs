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

using EuclidianDistanceClassifier;

namespace GloveApplication
{
    class SharedBuffer
    {
        private static readonly int _ringSize = 1 << 11;  // Must be multiple of 2
        private Disruptor.Dsl.Disruptor<SharedBufferValue> disruptor;
        private RingBuffer<SharedBufferValue> ringBuffer;

        public SharedBuffer()
        {
            disruptor = new Disruptor.Dsl.Disruptor<SharedBufferValue>(() => new SharedBufferValue(), _ringSize, TaskScheduler.Default);

            SharedBufferHandler sbh = new SharedBufferHandler();
            sbh.classifier = new Classifier();
            sbh.classifier.run();
            disruptor.HandleEventsWith(sbh);
            ringBuffer = disruptor.Start();
        }

        public void run(string dataSnapshot)
        {

            while (true)
            {
                long sequenceNo = ringBuffer.Next();

                SharedBufferValue entry = ringBuffer[sequenceNo];

                entry.Value = dataSnapshot;

                ringBuffer.Publish(sequenceNo);

                //Debugging
                //Console.WriteLine("Published entry {0}, value {1}", sequenceNo, entry.Value);
                break;

            }

        }
    }
}
