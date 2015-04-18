using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace EuclidianDistanceClassifier
{
    public class Gesture
    {
        public List<List<double> > RawData;
        public List<double> AveragedData;
        public string GestureType;

        public Gesture(string gestureType)
        {
            this.GestureType = gestureType;
            AveragedData = new List<double>();
        }


        public Gesture(List< List<double> > rawData, string gestureType)
        {
            this.RawData = rawData;
            this.GestureType = gestureType;
            AveragedData = new List<double>();
        }


        /*
         * Calculates the average values from the RawData recorded for this gesture.
         * This set of average values is what the real-time data will be compared against.
         */
        public void calculateAverageData()
        {
            int numColumns = RawData[0].Count();
            for (int i = 0; i < numColumns; i++)
            {
                double currentValueAverage = 0;
                int numRows = RawData[i].Count();

                for (int j = 0; j < numRows; j++)
                {
                    double currentValue = RawData[i][j];
                    currentValueAverage += currentValue;
                }

                currentValueAverage = currentValueAverage / numRows;
                AveragedData.Add(currentValueAverage);
            }

        }// calculateAverageData()

    }// class Gesture


}// namespace EuclidianDistanceClassifier
