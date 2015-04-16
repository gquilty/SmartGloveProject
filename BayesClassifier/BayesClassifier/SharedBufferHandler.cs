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

using BayesClassifier;

namespace GloveApplication
{
    public class SharedBufferHandler : IEventHandler<SharedBufferValue>
    {
        public Classifier classifier;

        public void OnNext(SharedBufferValue data, long sequence, bool endOfBatch)
        {
           // classifier.handleBufferEntry(data.Value);
        }
    }
}
