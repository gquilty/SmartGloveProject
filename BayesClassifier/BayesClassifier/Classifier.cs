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
        public MainWindow gui;
        
        public List<Gesture> KnownGestures;
        private Dictionary<string, int> GestureTypes;

        private int BinningBoundary;
        private int LaplacianSmoothingFactor;

        public Classifier()
        {
            KnownGestures = new List<Gesture>();
            BinningBoundary = 3;
            LaplacianSmoothingFactor = 1;
        }

        public void Setup()
        {

            // Debug
            Console.Write("\nLoading gesture types...");
            GestureTypes = LoadGestureTypes();
            Console.WriteLine("done.\n");
            //

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

            // This is for printing only, to show the data for the demo.
            string binnedSnapshotValues = "";

            for (int i = 0; i < splitData.Count() - 6; i++)
            {
                int parsedValue = BinValue(splitData[i]);
                parsedData.Add(parsedValue);

                binnedSnapshotValues += " " + parsedValue;
            }

            string received = Classify(parsedData);
            // Debugging
            Console.WriteLine(binnedSnapshotValues + "\n\t" + received);
            //
        }


        /*
         * Read the names of known gestures from file.
         * These are used to then read the gestures's data from other files
         */
        public Dictionary<string, int> LoadGestureTypes()
        {
            Dictionary<string, int> gestures = new Dictionary<string, int>();

            string file = "GestureData\\GestureTypes.dat";
            StreamReader dataStream = new StreamReader(file);
            string gesture;

            int value = 0;
            gestures.Add("NO_GESTURE", value);
            value++;
            while ((gesture = dataStream.ReadLine()) != null)
            {
                gestures.Add(gesture, value);
                value++;
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
                        
            int lineNumber = 0;
            while ((dataSample = dataStream.ReadLine()) != null)
            {
                // dataSample has the current line of text. Values are separated by a space and must be split
                string[] splitData = dataSample.Split(',');

                // parse the split doubles and add to List
                gestureRawData.Add(new List<double>());

                // This is for printing only, to show the data for the demo.
                string binnedTrainingValues = "";
                for (int i = 0; i < splitData.Count() - 6; i++)
                {
                    int parsedValue = BinValue(splitData[i]);
                    binnedTrainingValues += " " + parsedValue;
                    gestureRawData[lineNumber].Add(parsedValue);

                }
                lineNumber++;

                Console.WriteLine(binnedTrainingValues);
            }

            return new Gesture(gestureRawData, gestureType);
        }

        public int BinValue(string value)
        {
            return ((int)Double.Parse(value) / BinningBoundary) + LaplacianSmoothingFactor;
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

            // Get string value for the gesture with the lowest distance
            recognisedGesture = (from c in gestureDistances
                                 where c.Value == lowestDistance
                                 select c.Key).First();

            
            gui.SetImage( GestureTypes[recognisedGesture] );
            
            return recognisedGesture;
        }

        
    }
}