using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using MetroFramework.Forms;
using MetroFramework.Controls;
using MetroFramework.Fonts;

using GloveApplication;

namespace EuclidianDistanceClassifier
{
    public partial class MainWindow : MetroForm
    {
        public GloveStreamReader glove;
        public Classifier classifierRef;

        public MainWindow()
        {
            InitializeComponent();
        }


        public void StartGlove(int comPort)
        {
            glove = new GloveStreamReader();
            classifierRef = glove.buffer.dch.avgLayer.classifier;
            classifierRef.gui = this;

            glove.readData(comPort);            
        }

        public void setImage(int gestureType)
        {

            switch (gestureType)
            {



            }

        }

        private void button1_Click(object sender, EventArgs e)
        {
            int comPort = (int)Double.Parse(metroTextBox1.Text);
            StartGlove(comPort);
        }

        private void metroTextBox1_Click(object sender, EventArgs e)
        {
            metroTextBox1.Text = "";
        }
    }
}
