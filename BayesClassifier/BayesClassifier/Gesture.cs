using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace BayesClassifier
{
    public class Gesture
    {
        public List<List<double> > RawData;
        public string GestureType;

        public Gesture(string gestureType)
        {
            this.GestureType = gestureType;
        }


        public Gesture(List< List<double> > rawData, string gestureType)
        {
            this.RawData = rawData;
            this.GestureType = gestureType;
        }
    }
}
