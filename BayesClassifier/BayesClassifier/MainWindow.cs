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
            populateGrid();

            glove.readData(comPort);            
        }

        public void SetImage(int gestureType, string gestureName)
        {
            string messageType = classifierRef.xl.MessageTypes[gestureType];
            gestureLabel.Invoke((MethodInvoker)(() => gestureLabel.Text = gestureName + " / " + messageType));
            
            try
            {
                switch (gestureType)
                {
                    case 1:
                        gestureImage.Image = Flat;
                        break;
                    case 2:
                        gestureImage.Image = PeaceSign;
                        break;
                    case 3:
                        gestureImage.Image = ThumbsUp;
                        break;
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        bool startedOnce = false;
        private void startBtn_Click(object sender, EventArgs e)
        {
            if (!startedOnce)
            {
                startBtn.Text = "Running...";
                button1.Text = "Running...";
                this.tabControl.SelectedIndex = 1;
                int comPort = (int)Double.Parse(comPortTextBox.Text);
                StartGlove(comPort);
                classifierRef.Running = !classifierRef.Running;
                startedOnce = true;
            }
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
            if (classifierRef == null)
                return;

            classifierRef.Recording = !classifierRef.Recording;
            classifierRef.RecordingName = recordTextBox.Text;
        }

        private void simulate_Toggle(object sender, EventArgs e)
        {
            if (classifierRef == null)
                return;

            classifierRef.Simulate = !classifierRef.Simulate;
            classifierRef.SimulateFromFile();
        }

        public void populateGrid()
        {
            if (classifierRef == null)
                return;

            List<Gesture> gestures = classifierRef.KnownGestures;

            foreach (var g in gestures)
            {
                bool userGenerated = false;
                string name = g.GestureType;
                
                string[] row = new string[] { g.GestureType, userGenerated ? "Yes" : "No" };
                gestureGrid.Rows.Add(row);
            }
            
        }



    }
}
