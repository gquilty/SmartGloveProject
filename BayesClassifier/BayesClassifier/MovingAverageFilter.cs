using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Data;
using System.IO.Ports;
using System.Threading;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Globalization;
using Disruptor;
using Disruptor.Dsl;

using BayesClassifier;

namespace GloveApplication
{

    /// <summary>
    /// The MovingAverageFilter class is used to filter the raw output to a more readible form
    /// by removing noise and calibrating to a set standard
    /// </summary>
    public class MovingAverageFilter
    {
        List<List<string>> lastValues = new List<List<string>>();
        List<double> averagedData = new List<double>();
        //SharedBuffer sharedBuffer = new SharedBuffer();
        Classifier classifier = new Classifier();

        /// <summary>
        /// The movingAverage() method takes in data from the glove and performs a simple moving average on this data 
        /// it then calibrates the data using normilization between -1 to 1 
        /// </summary>
        /// <param name="data">A packet of data recieved from the ring buffer that contains a snapshot of the glove</param>
        public void movingAverage2(string data)
        {
            string[] currentData = data.Split(',');
             
            List<string> input  = new List<string>(currentData);
            //Debugging
            //Console.Write("Current Data Count: " + currentData.Count() + "\n\n");
            //Debugging
           // Console.Write("Current Data: " + data + "\n\n");
            if (input.Count() == 13)
            {
                /*//Debugging
                for (int i = 0; i < input.Count(); i++)
                {
                    Console.Write(input[i] + "\n");
                }
                 * */

                if (lastValues.Count() <= 4)
                {
                    lastValues.Add(input);
                }
                else
                {
                    lastValues.RemoveAt(0);
                    lastValues.Add(input);
                    try
                    {
                        averagedData.Add(double.Parse(lastValues[4][0]));
                    }
                    catch (Exception ex)
                    {
                        Console.Write("Time Parse Error \n");
                        return;
                    }
                    for (int j = 1; j < 13; j++)
                    {
                        double value1;
                        double value2;
                        double value3;
                        double value4;
                        double value5;
                        try
                        {

                            value1 = double.Parse(lastValues[0][j]);

                            value2 = double.Parse(lastValues[1][j]);

                            value3 = double.Parse(lastValues[2][j]);

                            value4 = double.Parse(lastValues[3][j]);

                            value5 = double.Parse(lastValues[4][j]);



                            double averagedValues = (value1 + value2 + value3 + value4 + value5) / 5;
                            double normalisedValue = (100 * (averagedValues + 1.5) / 4.5);
                            double truncatedValue = (Math.Truncate(normalisedValue * 1000.0) / 1000.0);
                            averagedData.Add(truncatedValue);
                            //averagedData.Add(averagedValues);

                        }
                        catch (Exception ex)
                        {
                            //Debugging
                            Console.Write("Value Parse error \n");
                            /*
                            Console.Write("ERROR: Value1-" + j + ": " + lastValues[0][j] + "\n");
                            Console.Write("ERROR: Value2 " + j + ": " + lastValues[1][j] + "\n");
                            Console.Write("ERROR: Value3 " + j + ": " + lastValues[2][j] + "\n");
                            Console.Write("ERROR: Value4 " + j + ": " + lastValues[3][j] + "\n");
                            Console.Write("ERROR: Value5 " + j + ": " + lastValues[4][j] + "\n\n");
                             * */
                             
                            averagedData.Clear();
                            return;
                        }

                    }
                

                    string averagedSnapshot = "";

                    for (int i = 1; i < averagedData.Count(); i++)
                    {
                        if (i == averagedData.Count() - 1)
                        {
                            averagedSnapshot += ((averagedData[i].ToString()));
                        }
                        else
                        {
                            averagedSnapshot += ((averagedData[i].ToString()) + ",");
                        }
                    }
                    //Debugging
                    //Console.Write("This is the averaged data: " + averagedSnapshot + "\n\n");
                   // using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\PleaseWorkBend.txt", true))
                   // {
                   //     file.WriteLine(averagedSnapshot);
                   // }

                
                    // DataSnapshot averagedSnapshot = new DataSnapshot(averagedData);

                    //string averagedSnapshot = "";

                    //for (int i = 1; i < 13; i++)
                    //{
                    //    try
                    //    {
                    //        double rawValue = Double.Parse(input[i]);
                    //        averagedData.Add(rawValue);
                    //    }catch(Exception ex)
                    //    {
                    //        //ParseError
                    //        averagedData.Clear();
                    //        return;
                    //    }
                        
                    //}

                    //for (int i = 0; i < averagedData.Count(); i++)
                    //{
                    //    if (i == averagedData.Count() - 1)
                    //    {
                    //        averagedSnapshot += ((averagedData[i].ToString()));
                    //    }
                    //    else
                    //    {
                    //        averagedSnapshot += ((averagedData[i].ToString()) + ",");
                    //    }
                    //}
                    
                    ///*using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\TrainingData\RawDataTestBend.txt", true))
                    //{
                    //    file.WriteLine(averagedSnapshot);
                    //}*/
                    

                    //sharedBuffer.run(averagedSnapshot);

                    classifier.Classify(averagedData.ToArray());


                    averagedData.Clear();

                }
            }
        }
    }
}
