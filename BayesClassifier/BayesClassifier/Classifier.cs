using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;

using GloveApplication;

namespace BayesClassifier
{
    public class Classifier
    {
        private DataSet TrainedData;
        private List<Gesture> Gestures;
        private List<string> GestureTypes;

        private double RecognitionThreshold;

        public Classifier()
        {
            TrainedData = new DataSet();
            Gestures = new List<Gesture>();
            GestureTypes = LoadGestureTypes();

            RecognitionThreshold = 0.001;
        }

        public void run()
        {

            // Instantiate a Gesture object for each type of known gesture
            foreach (string gesture in GestureTypes)
            {
                Gesture newGesture = LoadTrainingData(gesture);
                Gestures.Add(newGesture);
            }

            DataTable GestureTable = new DataTable();
            GestureTable.Columns.Add("GestureType", typeof(string));
            GestureTable.Columns.Add("1X", typeof(double));
            GestureTable.Columns.Add("1Y", typeof(double));
            GestureTable.Columns.Add("1Z", typeof(double));
            GestureTable.Columns.Add("2X", typeof(double));
            GestureTable.Columns.Add("2Y", typeof(double));
            GestureTable.Columns.Add("2Z", typeof(double));
            GestureTable.Columns.Add("3X", typeof(double));
            GestureTable.Columns.Add("3Y", typeof(double));
            GestureTable.Columns.Add("3Z", typeof(double));
            GestureTable.Columns.Add("4X", typeof(double));
            GestureTable.Columns.Add("4Y", typeof(double));
            GestureTable.Columns.Add("4Z", typeof(double));

            foreach (Gesture g in Gestures)
            {
                
                // oops need to seperate rows
                foreach (var dataEntry in g.RawData)
                {
                    object[] dataArray = new object[1 + dataEntry.Count()];
                    dataArray[0] = g.GestureType;
                    dataEntry.ToArray().CopyTo(dataArray, 1);
                    GestureTable.Rows.Add(dataArray);
                }
            }

            TrainClassifier(GestureTable);



            // TODO Replace this with read real time
           /* string file = "GestureData\\NoBendContinuous.txt";
            StreamReader dataStream = new StreamReader(file);
            string dataSample;
            int j = 0;
            while ((dataSample = dataStream.ReadLine()) != null)
            {

                // dataSample has the current line of text
                string[] split = dataSample.Split(new Char[] { ' ' });
                double[] splitDouble = new double[12];

                for (int i = 0; i < split.Count(); i++)
                {
                    splitDouble[i] = Double.Parse(split[i]);
                }
                string received = Classify(splitDouble);
                Console.WriteLine(++j + "\t" + received);
                System.Threading.Thread.Sleep(100);
            }*/
           
        }

        
        public void handleBufferEntry(string dataSample)
        {
            
            string[] split = dataSample.Split(new Char[] { ',' });
            
            double[] splitDouble = new double[13];

            for (int i = 0; i < split.Count(); i++)
            {
                // Skip the time variable
                if (i == 0)
                {
                    continue;
                }
                
                splitDouble[i] = Double.Parse(split[i]);
            }

            string detectedGesture = Classify(splitDouble);
            Console.WriteLine("Received:\t" + detectedGesture);
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

            int lineNumber = 0;
            while ((dataSample = dataStream.ReadLine()) != null)
            {
                // dataSample has the current line of text. Values are separated by a space and must be split
                string[] split = dataSample.Split(new Char[] { ' ' });
                
                // parse the split doubles and add to List
                gestureRawData.Add(new List<double>());
                for (int i = 0; i < split.Count(); i++)
                {
                    //Console.WriteLine(gestureType + " " + split[i]);
                    double parsedValue = Double.Parse(split[i]);
                    gestureRawData[lineNumber].Add(parsedValue);
                    
                }
                lineNumber++;
            }

            return new Gesture(gestureRawData, gestureType);
        }

        /*
        // Takes Gesture data and makes DataTable for TrainClassifier()
        public void constructTrainingData()
        {
            

        }*/

        public void TrainClassifier(DataTable GestureTable)
        {
            TrainedData.Tables.Add(GestureTable);

            //table
            DataTable GaussianDistribution = TrainedData.Tables.Add("Gaussian");
            GaussianDistribution.Columns.Add(GestureTable.Columns[0].ColumnName);

            //columns
            for (int i = 1; i < GestureTable.Columns.Count; i++)
            {
                GaussianDistribution.Columns.Add(GestureTable.Columns[i].ColumnName + "Mean");
                GaussianDistribution.Columns.Add(GestureTable.Columns[i].ColumnName + "Variance");
            }

            //calc data
            var results = (from myRow in GestureTable.AsEnumerable()
                           group myRow by myRow.Field<string>(GestureTable.Columns[0].ColumnName) into g
                           select new { Name = g.Key, Count = g.Count() }).ToList();

            for (int j = 0; j < results.Count; j++)
            {
                DataRow row = GaussianDistribution.Rows.Add();
                row[0] = results[j].Name;

                int a = 1;
                for (int i = 1; i < GestureTable.Columns.Count; i++)
                {
                    row[a] = Helper.Mean(SelectRows(GestureTable, i, string.Format("{0} = '{1}'", GestureTable.Columns[0].ColumnName, results[j].Name)));
                    row[++a] = Helper.Variance(SelectRows(GestureTable, i, string.Format("{0} = '{1}'", GestureTable.Columns[0].ColumnName, results[j].Name)));
                    a++;
                }
            }
        }



        int j = 0;
        public string Classify(double[] obj)
        {
            Dictionary<string, double> score = new Dictionary<string, double>();


            var results = (from myRow in TrainedData.Tables[0].AsEnumerable()
                            group myRow by myRow.Field<string>(TrainedData.Tables[0].Columns[0].ColumnName) into g
                            select new { Name = g.Key, Count = g.Count() }).ToList();

            for (int i = 0; i < results.Count; i++)
            {
                List<double> subScoreList = new List<double>();
                int a = 1, b = 1;
                for (int k = 1; k < TrainedData.Tables["Gaussian"].Columns.Count; k = k + 2)
                {
                    double mean = Convert.ToDouble(TrainedData.Tables["Gaussian"].Rows[i][a]);
                    double variance = Convert.ToDouble(TrainedData.Tables["Gaussian"].Rows[i][++a]);
                    double result = Helper.NormalDist(obj[b - 1], mean, Helper.SquareRoot(variance));
                    subScoreList.Add(result);
                    a++; b++;
                }

                double finalScore = 0;
                for (int z = 0; z < subScoreList.Count; z++)
                {
                    if (finalScore == 0)
                    {
                        finalScore = subScoreList[z];
                        continue;
                    }

                    finalScore = finalScore * subScoreList[z];
                }

                score.Add(results[i].Name, finalScore * 0.5);

                // writes the gesture names and their probability score to screen
                Console.WriteLine(j++ + ": " + results[i].Name + " \t" + score[results[i].Name]);
            }

            double maxOne = score.Max(c => c.Value);

            if (maxOne < RecognitionThreshold)
                return "NO_GESTURE";


            var name = (from c in score
                        where c.Value == maxOne
                        select c.Key).First();

            return name;
        }

        // Helper function just takes a row and returns all values matching the filter as a list
        public IEnumerable<double> SelectRows(DataTable table, int column, string filter)
        {
            List<double> _doubleList = new List<double>();

            // Return values matching the filter "GestureType = 'SOME_GESTURE'"
            DataRow[] rows = table.Select(filter);
            for (int i = 0; i < rows.Length; i++)
            {
                _doubleList.Add((double)rows[i][column]);
            }

            return _doubleList;
        }

        public void ClearTrainedData()
        {
            TrainedData = new DataSet();
        }
    }
}
