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

        public void SetImage(int gestureType)
        {
            try
            {
                switch (gestureType)
                {
                    case 1:
                        gestureImage.Image = Flat;
                        gestureLabel.Invoke((MethodInvoker)(() => gestureLabel.Text = "No Bend"));
                        break;
                    case 2:
                        gestureImage.Image = ThumbsUp;
                        gestureLabel.Invoke((MethodInvoker)(() => gestureLabel.Text = "Thumbs Up"));
                        break;
                    case 3:
                        gestureImage.Image = PeaceSign;
                        gestureLabel.Invoke((MethodInvoker)(() => gestureLabel.Text = "Peace Sign"));
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
    }
}
