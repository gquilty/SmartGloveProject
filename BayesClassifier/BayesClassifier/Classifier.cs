using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;

using GloveApplication;


namespace EuclidianDistanceClassifier
{
    
    public class Classifier
    {
        private DataSet TrainedData;

        public DataSet trainedData
        {
            get { return TrainedData; }
            set { TrainedData = value; }
        }
        private List<Gesture> KnownGestures;
        private List<string> GestureTypes;

        private double DistanceThreshold;

        public Classifier()
        {
            TrainedData = new DataSet();
            KnownGestures = new List<Gesture>();

            Console.Write("\nLoading gesture types...");
            GestureTypes = LoadGestureTypes();
            Console.WriteLine("done.\n");

            DistanceThreshold = 80;

        }

        public void run()
        {
            // Instantiate a Gesture object for each type of known gesture
            Console.Write("\nPopulating Gestures...");
            int numGestures = 0;
            foreach (string gesture in GestureTypes)
            {
                Gesture newGesture = LoadTrainingData(gesture);
                newGesture.calculateAverageData();
                KnownGestures.Add(newGesture);
                numGestures++;
            }

            Console.WriteLine("done. " + numGestures + " gestures loaded into program.");

        }

        
        public void handleBufferEntry(string dataSample)
        {
            int j = 0;
            // dataSample has the current line of text
            string[] split = dataSample.Split(new Char[] { ',' });
            List<double> splitDouble = new List<double>();

            string binnedData = "";
            for (int i = 0; i < split.Count(); i++)
            {
                //double parsedDouble = Double.Parse(split[i]);
                int parsedValue = ((int)Double.Parse(split[i]) / 3) + 1;
                splitDouble.Add(parsedValue);

                binnedData += parsedValue + " ";
            }

            string received = Classify(splitDouble);
            Console.WriteLine(binnedData + "\n\t" + received);
            System.Threading.Thread.Sleep(100);
           
        }
         


        /*
         * Read the names of known gestures from file.
         * These are used to then read the gestures's data from other files
         */
        public List<string> LoadGestureTypes()
        {
            List<string> gestures = new List<string>();

            string file = "GestureData\\GestureTypes.txt";
            StreamReader dataStream = new StreamReader(file);
            string gesture;
            while ((gesture = dataStream.ReadLine()) != null)
            {
                gestures.Add(gesture);
            }

            return gestures;
        }


        /* 
         * Creates the gestures for a given GestureType and returns the Gesture object
         */
        public Gesture LoadTrainingData(string gestureType)
        {
            List<List<double>> gestureRawData = new List<List<double>>();
            string dataSample;
            string gestureDataFile = "GestureData\\" + gestureType + ".txt";
            StreamReader dataStream = new StreamReader(gestureDataFile);


            string binnedData = "\n";
            int lineNumber = 0;
            while ((dataSample = dataStream.ReadLine()) != null)
            {
                // dataSample has the current line of text. Values are separated by a space and must be split
                string[] split = dataSample.Split(new Char[] { ',' });

                // parse the split doubles and add to List
                gestureRawData.Add(new List<double>());
                for (int i = 0; i < split.Count(); i++)
                {
                    //double parsedValue = Double.Parse(split[i]);
                    int parsedValue = ((int)Double.Parse(split[i]) / 3) + 1;
                    gestureRawData[lineNumber].Add(parsedValue);

                    binnedData += parsedValue + " ";
                }
                lineNumber++;
            }

            Console.WriteLine(binnedData);
            return new Gesture(gestureRawData, gestureType);
        }


        public string Classify(List<double> gloveSnapshot)
        {
            string recognisedGesture = "";

            Dictionary<string, double> gestureDistances = new Dictionary<string, double>();

            // Calculate Euclidian Distances for each gesture. Compares current snapshot with each gesture's averaged data
            for (int i = 0; i < KnownGestures.Count(); i++)
            {
                double distance = Helper.EuclidianDistance(gloveSnapshot, KnownGestures[i].AveragedData);
                
                string gestureName = KnownGestures[i].GestureType;
                gestureDistances.Add(gestureName, distance);
            }

            // Find out which has the smallest distance
            double lowestDistance = Double.MaxValue;
            foreach (var key in gestureDistances.Keys)
            {
                double dist = gestureDistances[key];
                if (dist < lowestDistance)
                {
                    lowestDistance = dist;
                }
            }

            // Is it smaller than the recognition threshold?
            // DIstanceThreshold is squared to compensate for the lack of Square Root in the Euclidian Distance function (saves processing)
            if (lowestDistance > DistanceThreshold*DistanceThreshold)
            {
                return "NO_GESTURE";
            }

            Console.WriteLine("Dist:\t" + lowestDistance);
            recognisedGesture = (from c in gestureDistances
                                 where c.Value == lowestDistance
                                 select c.Key).First();

            
            return recognisedGesture;
        }

        
    }
}