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
            string[] allData = data.ToString().Split('*');         
            string[] currentData = allData[0].Split(',');
            List<string> input  = new List<string>(currentData);

            if(lastValues.Count() <= 4)
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
                    return;
                }
                for (int j = 1; j < lastValues[0].Count(); j++)
                {
                    double value1 = 0;
                    double value2 = 0;
                    double value3 = 0;
                    double value4 = 0;
                    double value5 = 0;
                    try
                    {
                        value1 = double.Parse(lastValues[0][j]);
                        value2 = double.Parse(lastValues[1][j]);
                        value3 = double.Parse(lastValues[2][j]);
                        value4 = double.Parse(lastValues[3][j]);

                        value5 = double.Parse(lastValues[4][j]);
                   

                        double averagedValues = (value1 + value2 + value3 + value4 + value5) / 5;
                        double normalisedValue = (2 * (averagedValues + 1.5) / 4.5) - 1.0;
                        double truncatedValue = (Math.Truncate(averagedValues * 1000.0) / 1000.0);
                        averagedData.Add(truncatedValue);
                    }
                    catch (Exception ex)
                    {
                        averagedData.Clear();
                        return;
                    }
                    
                }

                string avgData = "";
                if (averagedData.Count() < 13)
                {
                    //Uncomplete Data is useless in gesture system so its discarded
                    //This may leave gaps in the system but with large throughput it tends to be neglagible
                }
                else
                {
                    
                    for (int i = 1; i < averagedData.Count(); i++)
                    {
                        avgData += ((averagedData[i].ToString()) + ",");
                    }
                    using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\GerardTestCaseBent.txt", true))
                    {
                        file.WriteLine(avgData);
                    }
                    

                   // DataSnapshot averagedSnapshot = new DataSnapshot(averagedData);
                    //sharedBuffer.run(averagedSnapshot);
                }
                averagedData.Clear();
                
             }
        }
    }
}
