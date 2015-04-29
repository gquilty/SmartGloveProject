using System.Collections.Generic;
using System.Data;
using System.Windows.Forms;
using System.Drawing;

namespace EuclidianDistanceClassifier
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.tabPage2 = new MetroFramework.Controls.MetroTabPage();
            this.gestureLabel = new System.Windows.Forms.Label();
            this.startBtn = new System.Windows.Forms.Button();
            this.simulateCheckbox = new System.Windows.Forms.CheckBox();
            this.simulateLabel = new MetroFramework.Controls.MetroLabel();
            this.gestureImage = new System.Windows.Forms.PictureBox();
            this.recordCheckbox = new System.Windows.Forms.CheckBox();
            this.recordTextBox = new MetroFramework.Controls.MetroTextBox();
            this.recordLabel = new MetroFramework.Controls.MetroLabel();
            this.tabPage1 = new MetroFramework.Controls.MetroTabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.button1 = new System.Windows.Forms.Button();
            this.comPortTextBox = new MetroFramework.Controls.MetroTextBox();
            this.tabPage3 = new MetroFramework.Controls.MetroTabPage();
            this.gestureGrid = new System.Windows.Forms.DataGridView();
            this.tabControl = new MetroFramework.Controls.MetroTabControl();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.GestureType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.UserGenerated = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tabPage2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureImage)).BeginInit();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureGrid)).BeginInit();
            this.tabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.gestureLabel);
            this.tabPage2.Controls.Add(this.startBtn);
            this.tabPage2.Controls.Add(this.simulateCheckbox);
            this.tabPage2.Controls.Add(this.simulateLabel);
            this.tabPage2.Controls.Add(this.gestureImage);
            this.tabPage2.HorizontalScrollbarBarColor = true;
            this.tabPage2.HorizontalScrollbarHighlightOnWheel = false;
            this.tabPage2.HorizontalScrollbarSize = 10;
            this.tabPage2.Location = new System.Drawing.Point(4, 38);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(482, 263);
            this.tabPage2.TabIndex = 2;
            this.tabPage2.Text = "Live";
            this.tabPage2.VerticalScrollbarBarColor = true;
            this.tabPage2.VerticalScrollbarHighlightOnWheel = false;
            this.tabPage2.VerticalScrollbarSize = 10;
            // 
            // gestureLabel
            // 
            this.gestureLabel.AutoSize = true;
            this.gestureLabel.Location = new System.Drawing.Point(137, 230);
            this.gestureLabel.Name = "gestureLabel";
            this.gestureLabel.Size = new System.Drawing.Size(0, 13);
            this.gestureLabel.TabIndex = 14;
            // 
            // startBtn
            // 
            this.startBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.startBtn.Location = new System.Drawing.Point(341, 37);
            this.startBtn.Name = "startBtn";
            this.startBtn.Size = new System.Drawing.Size(135, 37);
            this.startBtn.TabIndex = 13;
            this.startBtn.Text = "Start";
            this.startBtn.UseVisualStyleBackColor = true;
            this.startBtn.Click += new System.EventHandler(this.startBtn_Click);
            // 
            // simulateCheckbox
            // 
            this.simulateCheckbox.AutoSize = true;
            this.simulateCheckbox.Location = new System.Drawing.Point(363, 101);
            this.simulateCheckbox.Name = "simulateCheckbox";
            this.simulateCheckbox.Size = new System.Drawing.Size(15, 14);
            this.simulateCheckbox.TabIndex = 12;
            this.simulateCheckbox.UseVisualStyleBackColor = true;
            this.simulateCheckbox.CheckStateChanged += new System.EventHandler(this.simulate_Toggle);
            // 
            // simulateLabel
            // 
            this.simulateLabel.AutoSize = true;
            this.simulateLabel.Location = new System.Drawing.Point(384, 97);
            this.simulateLabel.Name = "simulateLabel";
            this.simulateLabel.Size = new System.Drawing.Size(65, 19);
            this.simulateLabel.TabIndex = 10;
            this.simulateLabel.Text = "Simulate?";
            // 
            // gestureImage
            // 
            this.gestureImage.Location = new System.Drawing.Point(23, 12);
            this.gestureImage.Name = "gestureImage";
            this.gestureImage.Size = new System.Drawing.Size(300, 200);
            this.gestureImage.TabIndex = 5;
            this.gestureImage.TabStop = false;

            ThumbsUp = Image.FromFile("Images\\ThumbsUp.png");
            PeaceSign = Image.FromFile("Images\\PeaceSign.png");
            Flat = Image.FromFile("Images\\Flat.png");
            // 
            // recordCheckbox
            // 
            this.recordCheckbox.AutoSize = true;
            this.recordCheckbox.Location = new System.Drawing.Point(99, 235);
            this.recordCheckbox.Name = "recordCheckbox";
            this.recordCheckbox.Size = new System.Drawing.Size(15, 14);
            this.recordCheckbox.TabIndex = 11;
            this.recordCheckbox.UseVisualStyleBackColor = true;
            this.recordCheckbox.CheckStateChanged += new System.EventHandler(this.record_Toggle);
            // 
            // recordTextBox
            // 
            this.recordTextBox.Lines = new string[] {
        "Enter New Gesture Name"};
            this.recordTextBox.Location = new System.Drawing.Point(135, 230);
            this.recordTextBox.MaxLength = 32767;
            this.recordTextBox.Name = "recordTextBox";
            this.recordTextBox.PasswordChar = '\0';
            this.recordTextBox.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.recordTextBox.SelectedText = "";
            this.recordTextBox.Size = new System.Drawing.Size(167, 23);
            this.recordTextBox.TabIndex = 8;
            this.recordTextBox.Text = "Enter New Gesture Name";
            this.recordTextBox.UseSelectable = true;
            this.recordTextBox.Click += new System.EventHandler(this.recordTextBox_Click);
            // 
            // recordLabel
            // 
            this.recordLabel.AutoSize = true;
            this.recordLabel.Location = new System.Drawing.Point(23, 230);
            this.recordLabel.Name = "recordLabel";
            this.recordLabel.Size = new System.Drawing.Size(57, 19);
            this.recordLabel.TabIndex = 7;
            this.recordLabel.Text = "Record?";
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.button1);
            this.tabPage1.Controls.Add(this.comPortTextBox);
            this.tabPage1.HorizontalScrollbarBarColor = true;
            this.tabPage1.HorizontalScrollbarHighlightOnWheel = false;
            this.tabPage1.HorizontalScrollbarSize = 10;
            this.tabPage1.Location = new System.Drawing.Point(4, 38);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(482, 263);
            this.tabPage1.TabIndex = 1;
            this.tabPage1.Text = "Setup";
            this.tabPage1.VerticalScrollbarBarColor = true;
            this.tabPage1.VerticalScrollbarHighlightOnWheel = false;
            this.tabPage1.VerticalScrollbarSize = 10;
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.InitialImage = null;
            this.pictureBox1.Location = new System.Drawing.Point(30, 150);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(395, 89);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 15;
            this.pictureBox1.TabStop = false;
            // 
            // button1
            // 
            this.button1.FlatAppearance.BorderColor = System.Drawing.Color.Red;
            this.button1.FlatStyle = System.Windows.Forms.FlatStyle.System;
            this.button1.Location = new System.Drawing.Point(258, 36);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(167, 37);
            this.button1.TabIndex = 14;
            this.button1.Text = "Start";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.startBtn_Click);
            // 
            // comPortTextBox
            // 
            this.comPortTextBox.Lines = new string[] {
        "Enter COM port here"};
            this.comPortTextBox.Location = new System.Drawing.Point(30, 44);
            this.comPortTextBox.MaxLength = 32767;
            this.comPortTextBox.Name = "comPortTextBox";
            this.comPortTextBox.PasswordChar = '\0';
            this.comPortTextBox.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.comPortTextBox.SelectedText = "";
            this.comPortTextBox.Size = new System.Drawing.Size(166, 23);
            this.comPortTextBox.TabIndex = 1;
            this.comPortTextBox.Text = "Enter COM port here";
            this.comPortTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.comPortTextBox.UseSelectable = true;
            this.comPortTextBox.Click += new System.EventHandler(this.comPortTextBox_Click);
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.gestureGrid);
            this.tabPage3.Controls.Add(this.recordTextBox);
            this.tabPage3.Controls.Add(this.recordLabel);
            this.tabPage3.Controls.Add(this.recordCheckbox);
            this.tabPage3.HorizontalScrollbarBarColor = true;
            this.tabPage3.HorizontalScrollbarHighlightOnWheel = false;
            this.tabPage3.HorizontalScrollbarSize = 10;
            this.tabPage3.Location = new System.Drawing.Point(4, 38);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(482, 263);
            this.tabPage3.TabIndex = 3;
            this.tabPage3.Text = "Training";
            this.tabPage3.VerticalScrollbarBarColor = true;
            this.tabPage3.VerticalScrollbarHighlightOnWheel = false;
            this.tabPage3.VerticalScrollbarSize = 10;
            // 
            // gestureGrid
            // 
            this.gestureGrid.AllowUserToAddRows = false;
            this.gestureGrid.AllowUserToDeleteRows = false;
            this.gestureGrid.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.GestureType,
            this.UserGenerated});
            this.gestureGrid.Location = new System.Drawing.Point(4, 4);
            this.gestureGrid.Name = "gestureGrid";
            this.gestureGrid.ReadOnly = true;
            this.gestureGrid.Size = new System.Drawing.Size(475, 168);
            this.gestureGrid.TabIndex = 12;
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tabPage1);
            this.tabControl.Controls.Add(this.tabPage2);
            this.tabControl.Controls.Add(this.tabPage3);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(20, 60);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(490, 305);
            this.tabControl.Style = MetroFramework.MetroColorStyle.Red;
            this.tabControl.TabIndex = 0;
            this.tabControl.UseSelectable = true;
            // 
            // GestureType
            // 
            this.GestureType.HeaderText = "Gesture Type";
            this.GestureType.Name = "GestureType";
            this.GestureType.ReadOnly = true;
            this.GestureType.ToolTipText = "Names of known gestures";
            this.GestureType.Width = 250;
            // 
            // UserGenerated
            // 
            this.UserGenerated.HeaderText = "User Generated?";
            this.UserGenerated.Name = "UserGenerated";
            this.UserGenerated.ReadOnly = true;
            this.UserGenerated.Width = 180;
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.ClientSize = new System.Drawing.Size(530, 385);
            this.Controls.Add(this.tabControl);
            this.Name = "MainWindow";
            this.Resizable = false;
            this.Style = MetroFramework.MetroColorStyle.Red;
            this.Text = "Gesture Recogniser";
            this.Theme = MetroFramework.MetroThemeStyle.Default;
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureImage)).EndInit();
            this.tabPage1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureGrid)).EndInit();
            this.tabControl.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion


        private Image ThumbsUp;
        private Image PeaceSign;
        private Image Flat;

        private MetroFramework.Controls.MetroTabPage tabPage1;
        private MetroFramework.Controls.MetroTabPage tabPage2;
        private MetroFramework.Controls.MetroTabPage tabPage3;
        private PictureBox gestureImage;
        private MetroFramework.Controls.MetroTextBox comPortTextBox;
        private MetroFramework.Controls.MetroTabControl tabControl;
        private MetroFramework.Controls.MetroLabel recordLabel;
        private MetroFramework.Controls.MetroTextBox recordTextBox;
        private MetroFramework.Controls.MetroLabel simulateLabel;
        private CheckBox simulateCheckbox;
        private CheckBox recordCheckbox;
        private Button startBtn;
        private PictureBox pictureBox1;
        private Button button1;
        private Label gestureLabel;
        private DataGridView gestureGrid;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private DataGridViewTextBoxColumn GestureType;
        private DataGridViewTextBoxColumn UserGenerated;
    }
}