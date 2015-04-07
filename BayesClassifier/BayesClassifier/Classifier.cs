using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;

namespace ProbabilityFunctions
{
    public class Classifier
    {
        private DataSet dataSet = new DataSet();
        
        public DataSet DataSet
        {
            get { return dataSet; }
            set { dataSet = value; }
        }


        static int Main(string[] args) {

            DataTable table = new DataTable(); 
            table.Columns.Add("GestureType");
            table.Columns.Add("1X", typeof(double));
            table.Columns.Add("1Y", typeof(double));
            table.Columns.Add("1Z", typeof(double));
            table.Columns.Add("2X", typeof(double));
            table.Columns.Add("2Y", typeof(double));
            table.Columns.Add("2Z", typeof(double));
            table.Columns.Add("3X", typeof(double));
            table.Columns.Add("3Y", typeof(double));
            table.Columns.Add("3Z", typeof(double));
            table.Columns.Add("4X", typeof(double));
            table.Columns.Add("4Y", typeof(double));
            table.Columns.Add("4Z", typeof(double));  

            //training data. 
            String file = "NoBend.txt";
            StreamReader dataStream = new StreamReader(file);
            string datasample;
            while ((datasample = dataStream.ReadLine()) != null)
            {

                // datasample has the current line of text - write it to the console.
                string[] split = datasample.Split(new Char [] {' '});

                // Add this line to training data
                table.Rows.Add("NO_BEND", split[0], split[1], split[2], split[3], split[4], split[5], 
                                            split[6], split[7], split[8], split[9], split[10], split[11]);
                //table.Rows.Add("No Bend", split);
            }

            file = "SlightBend.txt";
            dataStream = new StreamReader(file);
            while ((datasample = dataStream.ReadLine()) != null)
            {

                // datasample has the current line of text - write it to the console.
                string[] split = datasample.Split(new Char[] { ' ' });

                // Add this line to training data
                table.Rows.Add("SLIGHT_BEND", split[0], split[1], split[2], split[3], split[4], split[5],
                                                split[6], split[7], split[8], split[9], split[10], split[11]);
            }

            file = "FullBend.txt";
            dataStream = new StreamReader(file);
            while ((datasample = dataStream.ReadLine()) != null)
            {

                // datasample has the current line of text - write it to the console.
                string[] split = datasample.Split(new Char[] { ' ' });

                // Add this line to training data
                //table.Rows.Add("Full Bend", split[0], split[1], split[2], split[3], split[4], split[5],
                //                                split[6], split[7], split[8], split[9], split[10], split[11]);
            }


            Classifier classifier = new Classifier(); 
            classifier.TrainClassifier(table);

            classifier.TestClassifier();

            return 0;
        }


        public void TestClassifier()
        {

            int success = 0;
            /*
             * Known cases -  these are known gestures used to test if the training data taken above is sufficient
             * Print expected classification, and actual classification
             */
            Console.WriteLine("\n|----------------------------------------------------------|\n");

            // Should be classed as No Bend            
            Console.WriteLine("Input values: \t 0.064062,0.1617188,0.8421878,0.0789064,0.183594,0.9078128,0.2164066,0.0234374,0.7632812,0.902344,1.666113,1.220742");
            Console.WriteLine("\n\nExpected Gesture: \t\tNO_BEND");
            string received = Classify(new double[] { 0.064062, 0.1617188, 0.8421878, 0.0789064, 0.183594, 0.9078128, 0.2164066, 0.0234374, 0.7632812, 0.902344, 1.666113, 1.220742 });
            if (received.Equals("NO_BEND"))
                success++;
            Console.WriteLine("Received Gesture: \t\t" + received);
            Console.WriteLine((received.Equals("NO_BEND")) ? "\t\t\t\tSUCCESS" : "\t\t\t\tFAILURE");


            Console.WriteLine("\n|----------------------------------------------------------|\n");


            Console.WriteLine("Input values: \t 0.064, 0.163718, 0.8121878, 0.0799064, 0.183594, 0.9078128, 0.2364066, 0.0234374, 0.7632812, 0.902344, 1.666113, 1.220742");
            Console.WriteLine("\nExpected Gesture: \t\tNO_BEND");
            received = Classify(new double[] { 0.064, 0.163718, 0.8121878, 0.0799064, 0.183594, 0.9078128, 0.2364066, 0.0234374, 0.7632812, 0.902344, 1.666113, 1.220742 });
            if (received.Equals("NO_BEND"))
                success++;
            Console.WriteLine("Received Gesture: \t\t" + received);
            Console.WriteLine((received.Equals("NO_BEND")) ? "\t\t\t\tSUCCESS" : "\t\t\t\tFAILURE");


            Console.WriteLine("\n|----------------------------------------------------------|\n");



            // Should be classed as Slight Bend
            Console.WriteLine("Input values: \t -0.071872, -0.401558, 0.7632814, -0.214056, 0.049219, 0.9476564, -0.20234, -0.574214, 0.4023438, 0.9493946, 1.695117, 1.344492");
            Console.WriteLine("\n\nExpected Gesture: \t\tSLIGHT_BEND");
            received = Classify(new double[] { -0.071872, -0.401558, 0.7632814, -0.214056, 0.049219, 0.9476564, -0.20234, -0.574214, 0.4023438, 0.9493946, 1.695117, 1.344492 });
            if (received.Equals("SLIGHT_BEND"))
                success++;
            Console.WriteLine("Received Gesture: \t\t" + received);
            Console.WriteLine((received.Equals("SLIGHT_BEND")) ? "\t\t\t\tSUCCESS" : "\t\t\t\tFAILURE");


            Console.WriteLine("\n|----------------------------------------------------------|\n");


            Console.WriteLine("Input values: \t -0.096872, -0.48984, 0.784375, -0.227338, -0.189058, 0.9046876, -0.067184, -0.354682, 0.821094, 0.928125, 1.69834, 1.295508");
            Console.WriteLine("\n\nExpected Gesture: \t\tSLIGHT_BEND");
            received = Classify(new double[] { -0.096872, -0.48984, 0.784375, -0.227338, -0.189058, 0.9046876, -0.067184, -0.354682, 0.821094, 0.928125, 1.69834, 1.295508 });
            if (received.Equals("SLIGHT_BEND"))
                success++;
            Console.WriteLine("Received Gesture: \t\t" + received);
            Console.WriteLine((received.Equals("SLIGHT_BEND")) ? "\t\t\t\tSUCCESS" : "\t\t\t\tFAILURE");


            Console.WriteLine("\n|----------------------------------------------------------|\n");


            // Unknown - closer to Full Bend than anything else
            Console.WriteLine("Input values: \t 0.0320334, -0.721088, -0.867182, -0.098434, -0.7039, 0.7093752, -0.25312, 0.446875, -0.992184, 1.468887, 1.672559, 1.224609");
            Console.WriteLine("\n\nExpected Gesture: \t\tUNKNOWN_GESTURE");
            received = Classify(new double[] { 0.0320334, -0.721088, -0.867182, -0.098434, -0.7039, 0.7093752, -0.25312, 0.446875, -0.992184, 1.468887, 1.672559, 1.224609 });
            if (!received.Equals("SLIGHT_BEND") && !received.Equals("NO_BEND"))
                success++;
            Console.WriteLine("Received Gesture: \t\t" + received);
            Console.WriteLine((received.Equals("SLIGHT_BEND")) ? "\t\t\t\tSUCCESS" : "\t\t\t\tFAILURE");

            Console.WriteLine("\n|----------------------------------------------------------|\n");
            Console.WriteLine("Testing finished:\t" + success + "\\5 successful classifications.");

            Console.Read();
        }

        public void TrainClassifier(DataTable table)
        {
            dataSet.Tables.Add(table);

            //table
            DataTable GaussianDistribution = dataSet.Tables.Add("Gaussian");
            GaussianDistribution.Columns.Add(table.Columns[0].ColumnName);

            //columns
            for (int i = 1; i < table.Columns.Count; i++)
            {
                GaussianDistribution.Columns.Add(table.Columns[i].ColumnName + "Mean");
                GaussianDistribution.Columns.Add(table.Columns[i].ColumnName + "Variance");
            }

            //calc data
            var results = (from myRow in table.AsEnumerable()
                           group myRow by myRow.Field<string>(table.Columns[0].ColumnName) into g
                           select new { Name = g.Key, Count = g.Count() }).ToList();

            for (int j = 0; j < results.Count; j++)
            {
                DataRow row = GaussianDistribution.Rows.Add();
                row[0] = results[j].Name;

                int a = 1;
                for (int i = 1; i < table.Columns.Count; i++)
                {
                    row[a] = Helper.Mean(SelectRows(table, i, string.Format("{0} = '{1}'", table.Columns[0].ColumnName, results[j].Name)));
                    row[++a] = Helper.Variance(SelectRows(table, i, string.Format("{0} = '{1}'", table.Columns[0].ColumnName, results[j].Name)));
                    a++;
                }
            }
        }

        public string Classify(double[] obj)
        {
            Dictionary<string, double> score = new Dictionary<string, double>();

            var results = (from myRow in dataSet.Tables[0].AsEnumerable()
                            group myRow by myRow.Field<string>(dataSet.Tables[0].Columns[0].ColumnName) into g
                            select new { Name = g.Key, Count = g.Count() }).ToList();

            for (int i = 0; i < results.Count; i++)
            {
                List<double> subScoreList = new List<double>();
                int a = 1, b = 1;
                for (int k = 1; k < dataSet.Tables["Gaussian"].Columns.Count; k = k + 2)
                {
                    double mean = Convert.ToDouble(dataSet.Tables["Gaussian"].Rows[i][a]);
                    double variance = Convert.ToDouble(dataSet.Tables["Gaussian"].Rows[i][++a]);
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
            }

            double maxOne = score.Max(c => c.Value);
            var name = (from c in score
                        where c.Value == maxOne
                        select c.Key).First();

            return name;
        }

        public IEnumerable<double> SelectRows(DataTable table, int column, string filter)
        {
            List<double> _doubleList = new List<double>();
            DataRow[] rows = table.Select(filter);
            for (int i = 0; i < rows.Length; i++)
            {
                _doubleList.Add((double)rows[i][column]);
            }

            return _doubleList;
        }

        public void Clear()
        {
            dataSet = new DataSet();
        }
    }
}
