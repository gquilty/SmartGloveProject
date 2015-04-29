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
        public string GestureType;
        public List<double> AveragedData;
        public List<List<double>> RawData;

        public bool userGenerated;

        public Gesture()
        {
            RawData = new List<List<double>>();
            this.GestureType = "default";
            AveragedData = new List<double>();
            userGenerated = false;
        }

        public Gesture(string gestureType)
        {
            this.GestureType = gestureType;
            AveragedData = new List<double>();
            userGenerated = false;
        }


        public Gesture(List< List<double> > rawData, string gestureType)
        {
            this.RawData = rawData;
            this.GestureType = gestureType;
            AveragedData = new List<double>();
            userGenerated = false;
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


        public void WriteXML()
        {
            System.Xml.Serialization.XmlSerializer writer =
                new System.Xml.Serialization.XmlSerializer(typeof(Gesture));

            System.IO.StreamWriter file = new System.IO.StreamWriter(@"GestureData\\" + this.GestureType + ".xml");
            writer.Serialize(file, this);
            file.Close();
        }

    }// class Gesture


}// namespace EuclidianDistanceClassifier
