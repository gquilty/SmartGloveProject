using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace EuclidianDistanceClassifier
{
    public static class Helper
    {
        public static double EuclidianDistance(List<double> x, List<double> y)
        {
            if (x.Count() != y.Count())
            {

                throw new Exception("Helper.EuclidianDistance(): \tInputs are not of the same length.");
            }

            double distance = 0;
            int numEntries = x.Count();

            List<double> differencesSquared = new List<double>();

            var toProcess = numEntries;

            var resetEvent = new ManualResetEvent(false);

            for (var i = 0; i < numEntries; i++)
            {
                int k = i;
                ThreadPool.QueueUserWorkItem(
                    new WaitCallback(delegate(object state)
                    {
                        lock (differencesSquared)
                        {
                            differencesSquared.Add(calc(x[k], y[k]));
                        }
                        if (Interlocked.Decrement(ref toProcess) == 0) resetEvent.Set();
                    }), null);
            }

            resetEvent.WaitOne();

            for (int j = 0; j < differencesSquared.Count(); j++)
            {
                distance += differencesSquared[j];
            }


            return distance;
            //return SquareRoot(distance);

            //return x.Zip(y, (a, b) => (a - b) * (a - b)).Sum();


        }

        public static double calc(double a, double b)
        {
            double difference = a - b;
            double differenceSquared = Squared(difference);
            return differenceSquared;
        }

        public static double Squared(double source)
        {
            return Math.Pow(source, 2);
        }


        public static double SquareRoot(double source)
        {
            return Math.Sqrt(source);
        }

        public static double differenceSquared { get; set; }
    }
}