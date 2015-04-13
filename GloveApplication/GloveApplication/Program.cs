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
    class Program
    {
        
        public static void Main()
        {
            new Program().run();
        }

        public void run()
        {
            GloveStreamReader input = new GloveStreamReader();

            input.readData();
        }
    }
}
/*
    class AveragingLayer
    {
        public void movingAverage(string str)
        {
            string[] allData = str.Split('*');

            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\RawData3.txt", true))
            {
                file.WriteLine(allData.Length);
                for (int i = 0; i < allData.Length; i++)
                    file.WriteLine("RawInput{0} = {1}", i, allData[i]);
            }

            string inputFirst = allData[0];
            string[] datas1 = inputFirst.Split(',');
            float[] fDatas1 = new float[datas1.Length];
            for (int i = 1; i < datas1.Length; i++)
            {
                try
                {
                    float data = float.Parse(datas1[i]);
                    fDatas1[i] = data;
                }
                catch (FormatException ex)
                {
                    fDatas1[i] = 0.0f;
                }
            }

            string inputSecond = allData[1];
            string[] datas2 = inputSecond.Split(',');
            float[] fDatas2 = new float[datas2.Length];
            for (int j = 1; j < datas2.Length; j++)
            {
                try
                {
                    float data = float.Parse(datas2[j]);
                    fDatas2[j] = data;
                }
                catch (FormatException ex)
                {
                    fDatas2[j] = 0.0f;
                }
            }

            string inputThird = allData[2];
            string[] datas3 = inputThird.Split(',');
            float[] fDatas3 = new float[datas3.Length];
            for (int k = 1; k < datas3.Length; k++)
            {
                try
                {
                    float data = float.Parse(datas3[k]);
                    fDatas3[k] = data;
                }
                catch (FormatException ex)
                {
                    fDatas3[k] = 0.0f;
                }
            }

            string inputFourth = allData[3];
            string[] datas4 = inputFourth.Split(',');
            float[] fDatas4 = new float[datas4.Length];
            for (int l = 1; l < datas4.Length; l++)
            {
                try
                {
                    float data = float.Parse(datas4[l]);
                    fDatas4[l] = data;
                }
                catch (FormatException ex)
                {
                    fDatas4[l] = 0.0f;
                }
            }

            string inputFifth = allData[4];
            //Console.WriteLine(inputFifth);
            string[] datas5 = inputFifth.Split(',');

            float currentTime = float.Parse(datas5[0]);
            float[] fDatas5 = new float[datas5.Length];
            for (int m = 1; m < datas5.Length; m++)
            {
                try
                {
                    //Console.WriteLine(datas5[m]);
                    float data = float.Parse(datas5[m]);
                    fDatas5[m] = data;
                }
                catch (FormatException ex)
                {
                    //Console.WriteLine("Format ERROR! {0}", datas5[m]);
                    fDatas5[m] = 0.0f;
                }

            }

            float[] averageData = new float[datas5.Length];
            averageData[0] = currentTime;
            for (int n = 1; n < datas5.Length; n++)
            {
                try
                {
                    float data = (fDatas1[n] + fDatas2[n] + fDatas3[n] + fDatas4[n] + fDatas5[n]) / 5;
                    float normalisedData = (2 * (data + 1.5f) / 4.5f) - 1.0f;
                    float truncatedData = (float)(Math.Truncate((double)normalisedData * 1000.0) / 1000.0);
                    averageData[n] = truncatedData;
                }
                catch (IndexOutOfRangeException ex)
                {
                    //Neeeds better solution
                    averageData[n] = 0.000f;
                }
            }
            string avgData = "";
            for (int i = 0; i < averageData.Length; i++)
            {
                avgData += ((averageData[i].ToString()) + ",");
            }
            // Console.WriteLine(avgData);
            using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"C:\Users\gavin\Desktop\development area\FYP\DataTest3.txt", true))
            {
                file.WriteLine(avgData);
            }
        }
    }

    class AveragingLayer2
    {
        public void movingAverage2(string data)
        {
            string[] allData = data.Split('*');
            string[] currentData = allData[0].Split(',');
            for (int i = 0; i < currentData.Length; i++)
            {
                try
                {
                    float signal = float.Parse(currentData[i]);
                    //! number of last measure values to be used for the calculation
                    int smoothFactor = 4;  

                    // local variables
                    bool first = true;
                    double[] lastValues = new double[100];
                    double sum;
                    int j;
                    int index;

                    // initialize array with last values on start
                    if (first) {
                      first = false;

                      sum = smoothFactor * signal;
                      for (j = 0; j < smoothFactor; j++) {
                        lastValues[j] = signal;
                      } 
                      index = 0;
                      return signal;
                    }

                    sum -= lastValues[index];
                    sum += signal;
                    lastValues[index] = signal;
                    if (++index >= smoothFactor) {
                      index = 0;
                    }
                    return sum / smoothFactor;

                }
                catch (FormatException ex)
                {
                }
            }

        }
    }

*/

