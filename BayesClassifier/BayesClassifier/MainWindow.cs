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
            // GloveStreamReader creates all necessary objects in the system sequentially
            glove = new GloveStreamReader();
            classifierRef = glove.buffer.dch.avgLayer.sharedBuffer.sbh.classifier;
            classifierRef.gui = this;

            glove.readData(comPort);            
        }

        public void SetImage(int gestureType, string gestureName)
        {
            gestureLabel.Invoke((MethodInvoker)(() => gestureLabel.Text = gestureName));

            try
            {
                switch (gestureType)
                {
                    case 1:
                        gestureImage.Image = Flat;
                        break;
                    case 2:
                        gestureImage.Image = ThumbsUp;
                        break;
                    case 3:
                        gestureImage.Image = PeaceSign;
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            int comPort = (int)Double.Parse(comPortTextBox.Text);
            StartGlove(comPort);
        }

        private void comPortTextBox_Click(object sender, EventArgs e)
        {
            comPortTextBox.Text = "";
        }

        private void recordTextBox_Click(object sender, EventArgs e)
        {
            recordTextBox.Text = "";
        }


        private void record_Toggle(object sender, EventArgs e)
        {
            classifierRef.Recording = !classifierRef.Recording;
            classifierRef.RecordingName = recordTextBox.Text;
        }

        private void simulate_Toggle(object sender, EventArgs e)
        {
            classifierRef.Simulate = !classifierRef.Simulate;
        }
    }
}
