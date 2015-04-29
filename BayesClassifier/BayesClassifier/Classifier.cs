using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;

using MetroFramework.Forms;

using GloveApplication;
using System.Windows.Forms;


namespace EuclidianDistanceClassifier
{
    
    public class Classifier
    {
        public MainWindow gui;
        public XLInterface xl;
        
        public List<Gesture> KnownGestures;
        private Dictionary<string, int> GestureTypes;
        private List<int> RegisteredGestures;

        private int BinningBoundary;
        private int LaplacianSmoothingFactor;
        private int NumRegisteredGestures;

        private int NumBendSensors;

        public bool Recording;
        public string RecordingName;

        public bool Simulate;
        public bool Running;

        public Classifier()
        {
            KnownGestures = new List<Gesture>();
            RegisteredGestures = new List<int>();
            BinningBoundary = 3;
            LaplacianSmoothingFactor = 1;
            NumRegisteredGestures = 25;

            NumBendSensors = 6;

            Recording = false;
            RecordingName = "";
            Simulate = false;
            Running = false;

            xl = new XLInterface();
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
                newGesture.userGenerated = false;
                KnownGestures.Add(newGesture);
                numGestures++;
            }

            Console.WriteLine("done. " + numGestures + " gestures loaded into program.");

        }

        
        public void handleBufferEntry(string dataSample)
        {

            if (Simulate || !Running)
            {
                return;
            }


            int j = 0;
            // dataSample has the current line of text
            string[] splitData = dataSample.Split(',');
            List<double> parsedData = new List<double>();

            // This is for printing only, to show the data for the demo.
            string binnedSnapshotValues = "";

            for (int i = 0; i < splitData.Count() - NumBendSensors; i++)
            {
                int parsedValue = BinValue(splitData[i]);
                parsedData.Add(parsedValue);

                binnedSnapshotValues += " " + parsedValue;
            }

            if (Recording)
            {
                RecordGesture(dataSample, RecordingName);
            }

            string recognisedGesture = Classify(parsedData);

            // Debugging
            //Console.WriteLine(binnedSnapshotValues + "\n\t" + recognisedGesture);
            //

<<<<<<< HEAD
            int MostCommon = 0;
=======
            int MostCommonGesture = 0;
>>>>>>> f5268adffb7b7b2aa63b531d7405c8dca61e6df4
            if (RegisteredGestures.Count() >= NumRegisteredGestures)
            {
                // Averages the last 25 or so gestures (defined by NumRegisteredGestures) and gets the most common one.
                // Avoids the gesture changing rapidly from accidental hand movements
<<<<<<< HEAD
                MostCommon = GetCurrentGesture();
=======
                MostCommonGesture = GetCurrentGesture();
>>>>>>> f5268adffb7b7b2aa63b531d7405c8dca61e6df4
                var GestureTypesInverse = GestureTypes.ToDictionary(x => x.Value, x => x.Key);
                recognisedGesture = GestureTypesInverse[MostCommon];
            }

            if (MostCommon != LastGesture)
            {
                xl.SendMessage(MostCommon);
                LastGesture = MostCommon;
            }
            gui.SetImage(GestureTypes[recognisedGesture], recognisedGesture);
<<<<<<< HEAD
=======
            xl.SendMessage(MostCommonGesture);
>>>>>>> f5268adffb7b7b2aa63b531d7405c8dca61e6df4
        }


        int LastGesture = 0;
        public void SimulateFromFile()
        {
            while (Simulate) {

                string file = "GestureData\\SimulatedData.dat";
                StreamReader dataStream = new StreamReader(file);
                string dataSample;
                int j = 0;
                while ((dataSample = dataStream.ReadLine()) != null)
                {
                    // dataSample has the current line of text
                    string[] splitData = dataSample.Split(',');
                    List<double> parsedData = new List<double>();

                    // This is for printing only, to show the data for the demo.
                    string binnedSnapshotValues = "";

                    for (int i = 0; i < splitData.Count() - NumBendSensors; i++)
                    {
                        int parsedValue = BinValue(splitData[i]);
                        parsedData.Add(parsedValue);

                        binnedSnapshotValues += " " + parsedValue;
                    }

                    string recognisedGesture = Classify(parsedData);

                    // Debugging
                    //Console.WriteLine(binnedSnapshotValues + "\n\t" + recognisedGesture);
                    //

                    int MostCommon = 0;
                    if (RegisteredGestures.Count() >= NumRegisteredGestures)
                    {
                        // Averages the last 25 or so gestures (defined by NumRegisteredGestures) and gets the most common one.
                        // Avoids the gesture changing rapidly from accidental hand movements
                        MostCommon = GetCurrentGesture();
                        var GestureTypesInverse = GestureTypes.ToDictionary(x => x.Value, x => x.Key);
                        recognisedGesture = GestureTypesInverse[MostCommon];
                    }

                    gui.SetImage(GestureTypes[recognisedGesture], recognisedGesture);


                    if (MostCommon != LastGesture)
                    {
                        xl.SendMessage(MostCommon);
                        LastGesture = MostCommon;
                    }


                    Application.DoEvents();
                    System.Threading.Thread.Sleep(100);
                }

                
            }

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

                //Console.WriteLine(binnedTrainingValues);
            }

            return new Gesture(gestureRawData, gestureType);
        }


        public void RecordGesture(string gestureData, string filename)
        {
            StreamWriter file;

            // Gesture already exists in the program
            if (GestureTypes.Keys.Contains(filename))
            {
                return;
            }

            // Gesture already recorded and added to GestureTypes.dat
            if (!File.Exists("GestureData\\" + filename + ".dat"))
            {
                File.AppendAllText("GestureData\\GestureTypes.dat", Environment.NewLine + filename);
            }

            // Create the training data for the new gesture
            // Program needs to be restarted
            file = new StreamWriter(@"GestureData\\" + filename + ".dat", true);
            //Console.WriteLine("Writing to file\t" + gestureData);
            file.WriteLine(gestureData);
            file.Close();

            Console.WriteLine("Writing " + gestureData.Split(',').Count());
        }

        public int BinValue(string value)
        {
            return ((int)Double.Parse(value) / BinningBoundary) + LaplacianSmoothingFactor;
        }

        /*
         * Returns the most common gesture from the last 10 recorded gestures
         * *
         */
        public int GetCurrentGesture()
        {
            int mostCommonGesture = (from i in RegisteredGestures
                                    group i by i into grp
                                    orderby grp.Count() descending
                                    select grp.Key).First();

            return mostCommonGesture;
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

            if (RegisteredGestures.Count() >= NumRegisteredGestures)
            {
                RegisteredGestures.RemoveAt(0);
            }
            RegisteredGestures.Add( GestureTypes[recognisedGesture]);
            
            return recognisedGesture;
        }

        
    }
}