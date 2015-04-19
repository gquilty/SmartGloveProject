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
using System.Windows.Forms;

namespace GloveApplication
{
    class Program
    {

        [STAThread]
        public static void Main()
        {
            new Program().run();
        }

        public void run()
        {
            MainWindow window = new MainWindow();
            Application.Run(window);
        }
    }
}
