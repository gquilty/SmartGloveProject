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

namespace GloveApplication
{
    public class MovingAverageFilter
    {
        List<List<string>> lastValues = new List<List<string>>();
        List<double> averagedData = new List<double>();
        SharedBuffer sharedBuffer = new SharedBuffer();

        public void movingAverage2(string data)
        {
            string[] currentData = data.Split(',');
             
            List<string> input  = new List<string>(currentData);
            Console.Write("Current Data Count: " + currentData.Count() + "\n\n");
            Console.Write("Current Data: " + data + "\n\n");
            if (currentData.Count() == 13)
            {
                /*
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
                            double normalisedValue = (2 * (averagedValues + 1.5) / 4.5) - 1.0;
                            double truncatedValue = (Math.Truncate(normalisedValue * 1000.0) / 1000.0);
                            averagedData.Add(truncatedValue);
                        }
                        catch (Exception ex)
                        {
                            Console.Write("Value Parse error \n");
                            /*
                            Console.Write("ERROR: Value1-" + j + ": " + lastValues[0][j] + "\n");
                            Console.Write("ERROR: Value2 " + j + ": " + lastValues[1][j] + "\n");
                            Console.Write("ERROR: Value3 " + j + ": " + lastValues[2][j] + "\n");
                            Console.Write("ERROR: Value4 " + j + ": " + lastValues[3][j] + "\n");
                            Console.Write("ERROR: Value5 " + j + ": " + lastValues[4][j] + "\n\n");
                             */
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
                    Console.Write("This is the averaged data: " + averagedSnapshot + "\n\n");
                   // using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\ReadGloveFix.txt", true))
                    //{
                   //     file.WriteLine(averagedSnapshot);
                   // }


                    // DataSnapshot averagedSnapshot = new DataSnapshot(averagedData);

                    sharedBuffer.run(averagedSnapshot);
                    averagedData.Clear();

                }
            }
        }
    }
}
