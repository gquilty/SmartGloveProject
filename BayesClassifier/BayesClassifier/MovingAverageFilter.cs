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

using EuclidianDistanceClassifier;

namespace GloveApplication
{

    /// <summary>
    /// The MovingAverageFilter class is used to filter the raw output to a more readible form
    /// by removing noise and calibrating to a set standard
    /// </summary>
    public class MovingAverageFilter
    {
        private List<List<string>> lastValues = new List<List<string>>();
        private List<double> averagedData = new List<double>();
        public SharedBuffer sharedBuffer = new SharedBuffer();

        /// <summary>
        /// The movingAverage() method takes in data from the glove and performs a simple moving average on this data 
        /// it then calibrates the data using normilization between -1 to 1 
        /// </summary>
        /// <param name="data">A packet of data recieved from the ring buffer that contains a snapshot of the glove</param>
        public void movingAverage2(string data)
        {
            string[] currentData = data.Split(',');
             
            List<string> input  = new List<string>(currentData);

            if (input.Count() == 55)
            {

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
                    for (int j = 1; j < 55; j++)
                    {
                        double value1;
                        double value2;
                        double value3;
                        double value4;
                        double value5;
                        try
                        {

                            value1 = double.Parse(lastValues[0][j]);
                            if (value1 < -1.5)
                            {
                                value1 = 0;
                            }
                            value2 = double.Parse(lastValues[1][j]);
                            if (value2 < -1.5)
                            {
                                value2 = -1.5;
                            }
                            value3 = double.Parse(lastValues[2][j]);
                            if (value3 < -1.5)
                            {
                                value3 = -1.5;
                            }
                            value4 = double.Parse(lastValues[3][j]);
                            if (value4 < -1.5)
                            {
                                value4 = -1.5;
                            }
                            value5 = double.Parse(lastValues[4][j]);
                            if (value5 < -1.5)
                            {
                                value5 = -1.5;
                            }



                            double averagedValues = (value1 + value2 + value3 + value4 + value5) / 5;
                            double normalisedValue = (100 * (averagedValues + 1.5) / 4.5);
                            double truncatedValue = (Math.Truncate(normalisedValue * 1000.0) / 1000.0);
                            averagedData.Add(truncatedValue);
                            //averagedData.Add(averagedValues);

                        }
                        catch (Exception ex)
                        {
                            Console.Write("Value Parse error \n");
                             
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
                    
                    /*using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\Ger\Desktop\NO_BEND.dat", true))
                    {
                        Console.WriteLine("Writing to file\t" + averagedSnapshot);
                        file.WriteLine(averagedSnapshot);
                    }*/
                    
                    sharedBuffer.run(averagedSnapshot);
                    averagedData.Clear();

                }
            }
        }
    }
}
