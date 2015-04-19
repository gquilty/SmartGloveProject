using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;

using MetroFramework.Forms;

using GloveApplication;


namespace EuclidianDistanceClassifier
{
    
    public class Classifier
    {
        public MetroForm gui;
        
        public List<Gesture> KnownGestures;
        private Dictionary<string, int> GestureTypes;

        private double DistanceThreshold;

        public Classifier()
        {
            KnownGestures = new List<Gesture>();

            // Debug
            Console.Write("\nLoading gesture types...");
            GestureTypes = LoadGestureTypes();
            Console.WriteLine("done.\n");
            //

            DistanceThreshold = 100;

        }

        public void run()
        {
            // Instantiate a Gesture object for each type of known gesture
            Console.Write("\nPopulating Gestures...");
            int numGestures = 0;
            foreach (string gesture in GestureTypes.Keys)
            {
                Gesture newGesture = LoadTrainingData(gesture);
                if (newGesture == null)
                {
                    // In the case of NO_GESTURE being passed to LoadTrainingData
                    continue;
                }

                newGesture.calculateAverageData();
                newGesture.WriteXML();
                KnownGestures.Add(newGesture);
                numGestures++;
            }

            Console.WriteLine("done. " + numGestures + " gestures loaded into program.");

        }

        
        public void handleBufferEntry(string dataSample)
        {
            int j = 0;
            // dataSample has the current line of text
            string[] splitData = dataSample.Split(',');
            List<double> parsedData = new List<double>();

            // Debug
            string DebugBinnedData = "";
            //
            for (int i = 0; i < splitData.Count() - 6; i++)
            {
                //double parsedDouble = Double.Parse(split[i]);
                int parsedValue = ((int)Double.Parse(splitData[i]) / 3) + 1;
                parsedData.Add(parsedValue);

                // Debug
                DebugBinnedData += parsedValue + " ";
                //
            }

            string received = Classify(parsedData);
            // Debugging
            Console.WriteLine(DebugBinnedData + "\n\t" + received);
            //System.Threading.Thread.Sleep(100);
            //
        }


        /*
         * Read the names of known gestures from file.
         * These are used to then read the gestures's data from other files
         */
        public Dictionary<string, int> LoadGestureTypes()
        {
            Dictionary<string, int> gestures = new Dictionary<string, int>();

            gestures.Add("NO_GESTURE", 0);

            string file = "GestureData\\GestureTypes.dat";
            StreamReader dataStream = new StreamReader(file);
            string gesture;
            int value = 1;
            while ((gesture = dataStream.ReadLine()) != null)
            {
                gestures.Add(gesture, value);
            }

            return gestures;
        }


        /* 
         * Creates the gestures for a given GestureType and returns the Gesture object
         */
        public Gesture LoadTrainingData(string gestureType)
        {

            if (gestureType.Equals("NO_GESTURE"))
            {
                return null;
            }


            List<List<double>> gestureRawData = new List<List<double>>();
            string dataSample;
            string gestureDataFile = "GestureData\\" + gestureType + ".dat";
            StreamReader dataStream = new StreamReader(gestureDataFile);

            // Debug
            string binnedData = "\n";
            //
            int lineNumber = 0;
            while ((dataSample = dataStream.ReadLine()) != null)
            {
                // dataSample has the current line of text. Values are separated by a space and must be split
                string[] split = dataSample.Split(',');

                // parse the split doubles and add to List
                gestureRawData.Add(new List<double>());
                for (int i = 0; i < split.Count() - 6; i++)
                {
                    //double parsedValue = Double.Parse(split[i]);
                    int parsedValue = ((int)Double.Parse(split[i]) / 3) + 1;
                    gestureRawData[lineNumber].Add(parsedValue);

                    // Debug
                    binnedData += parsedValue + " ";
                    //
                }
                lineNumber++;
            }

            // Debug
            Console.WriteLine(binnedData);
            //

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

            // Debug
            //Console.WriteLine("Dist:\t" + lowestDistance);
            //

            recognisedGesture = (from c in gestureDistances
                                 where c.Value == lowestDistance
                                 select c.Key).First();

            
            return recognisedGesture;
        }

        
    }
}