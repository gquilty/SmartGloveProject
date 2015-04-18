using System;
using System.Collections.Generic;
using System.Linq;

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

            for (int i = 0; i < numEntries; i++)
            {
                double difference = x[i] - y[i];
                double differenceSquared = Squared(difference);

                distance += differenceSquared;

            }

            return distance;
            //return SquareRoot(distance);

            //return x.Zip(y, (a, b) => (a - b) * (a - b)).Sum();


        }


        public static double Squared(double source)
        {
            return Math.Pow(source, 2);
        }


        public static double SquareRoot(double source)
        {
            return Math.Sqrt(source);
        }
    }
}
