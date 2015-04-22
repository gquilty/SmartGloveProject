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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.tabPage2 = new MetroFramework.Controls.MetroTabPage();
            this.metroGrid1 = new MetroFramework.Controls.MetroGrid();
            this.tabPage1 = new MetroFramework.Controls.MetroTabPage();
            this.recordTextBox = new MetroFramework.Controls.MetroTextBox();
            this.recordLabel = new MetroFramework.Controls.MetroLabel();
            this.recordToggle = new MetroFramework.Controls.MetroToggle();
            this.gestureLabel = new MetroFramework.Controls.MetroLabel();
            this.gestureImage = new System.Windows.Forms.PictureBox();
            this.comPortTextBox = new MetroFramework.Controls.MetroTextBox();
            this.startBtn = new MetroFramework.Controls.MetroButton();
            this.tabControl = new MetroFramework.Controls.MetroTabControl();
            this.simulateToggle = new MetroFramework.Controls.MetroToggle();
            this.simulateLabel = new MetroFramework.Controls.MetroLabel();
            this.tabPage2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.metroGrid1)).BeginInit();
            this.tabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureImage)).BeginInit();
            this.tabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.metroGrid1);
            this.tabPage2.HorizontalScrollbarBarColor = true;
            this.tabPage2.HorizontalScrollbarHighlightOnWheel = false;
            this.tabPage2.HorizontalScrollbarSize = 10;
            this.tabPage2.Location = new System.Drawing.Point(4, 38);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(584, 322);
            this.tabPage2.TabIndex = 2;
            this.tabPage2.Text = "Data";
            this.tabPage2.VerticalScrollbarBarColor = true;
            this.tabPage2.VerticalScrollbarHighlightOnWheel = false;
            this.tabPage2.VerticalScrollbarSize = 10;
            // 
            // metroGrid1
            // 
            this.metroGrid1.AllowUserToAddRows = false;
            this.metroGrid1.AllowUserToDeleteRows = false;
            this.metroGrid1.AllowUserToResizeRows = false;
            this.metroGrid1.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.metroGrid1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.metroGrid1.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
            this.metroGrid1.ColumnHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(174)))), ((int)(((byte)(219)))));
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(174)))), ((int)(((byte)(219)))));
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(17)))), ((int)(((byte)(17)))), ((int)(((byte)(17)))));
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.metroGrid1.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.metroGrid1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(136)))), ((int)(((byte)(136)))), ((int)(((byte)(136)))));
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(174)))), ((int)(((byte)(219)))));
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(17)))), ((int)(((byte)(17)))), ((int)(((byte)(17)))));
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.metroGrid1.DefaultCellStyle = dataGridViewCellStyle2;
            this.metroGrid1.EnableHeadersVisualStyles = false;
            this.metroGrid1.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            this.metroGrid1.GridColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.metroGrid1.Location = new System.Drawing.Point(4, 18);
            this.metroGrid1.Name = "metroGrid1";
            this.metroGrid1.ReadOnly = true;
            this.metroGrid1.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(174)))), ((int)(((byte)(219)))));
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Segoe UI", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Pixel);
            dataGridViewCellStyle3.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(174)))), ((int)(((byte)(219)))));
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(17)))), ((int)(((byte)(17)))), ((int)(((byte)(17)))));
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.metroGrid1.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.metroGrid1.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.metroGrid1.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.metroGrid1.Size = new System.Drawing.Size(580, 308);
            this.metroGrid1.TabIndex = 2;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.simulateLabel);
            this.tabPage1.Controls.Add(this.simulateToggle);
            this.tabPage1.Controls.Add(this.recordTextBox);
            this.tabPage1.Controls.Add(this.recordLabel);
            this.tabPage1.Controls.Add(this.recordToggle);
            this.tabPage1.Controls.Add(this.gestureLabel);
            this.tabPage1.Controls.Add(this.gestureImage);
            this.tabPage1.Controls.Add(this.comPortTextBox);
            this.tabPage1.Controls.Add(this.startBtn);
            this.tabPage1.HorizontalScrollbarBarColor = true;
            this.tabPage1.HorizontalScrollbarHighlightOnWheel = false;
            this.tabPage1.HorizontalScrollbarSize = 10;
            this.tabPage1.Location = new System.Drawing.Point(4, 38);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(584, 322);
            this.tabPage1.TabIndex = 1;
            this.tabPage1.Text = "Running";
            this.tabPage1.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.tabPage1.VerticalScrollbarBarColor = true;
            this.tabPage1.VerticalScrollbarHighlightOnWheel = false;
            this.tabPage1.VerticalScrollbarSize = 10;
            // 
            // recordTextBox
            // 
            this.recordTextBox.Lines = new string[] {
        "Enter New Gesture Name"};
            this.recordTextBox.Location = new System.Drawing.Point(23, 181);
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
            this.recordLabel.Location = new System.Drawing.Point(23, 141);
            this.recordLabel.Name = "recordLabel";
            this.recordLabel.Size = new System.Drawing.Size(57, 19);
            this.recordLabel.TabIndex = 7;
            this.recordLabel.Text = "Record?";
            this.recordLabel.Theme = MetroFramework.MetroThemeStyle.Dark;
            // 
            // recordToggle
            // 
            this.recordToggle.AutoSize = true;
            this.recordToggle.Location = new System.Drawing.Point(110, 143);
            this.recordToggle.Name = "recordToggle";
            this.recordToggle.Size = new System.Drawing.Size(80, 17);
            this.recordToggle.TabIndex = 6;
            this.recordToggle.Text = "Off";
            this.recordToggle.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.recordToggle.UseSelectable = true;
            this.recordToggle.CheckStateChanged += new System.EventHandler(this.record_Toggle);
            // 
            // gestureLabel
            // 
            this.gestureLabel.AutoSize = true;
            this.gestureLabel.Location = new System.Drawing.Point(346, 273);
            this.gestureLabel.Name = "gestureLabel";
            this.gestureLabel.Size = new System.Drawing.Size(0, 0);
            this.gestureLabel.TabIndex = 4;
            // 
            // gestureImage
            // 
            this.gestureImage.Location = new System.Drawing.Point(240, 36);
            this.gestureImage.Name = "gestureImage";
            this.gestureImage.Size = new System.Drawing.Size(310, 209);
            this.gestureImage.TabIndex = 5;
            this.gestureImage.TabStop = false;
            // 
            // comPortTextBox
            // 
            this.comPortTextBox.Lines = new string[] {
        "Enter COM port here"};
            this.comPortTextBox.Location = new System.Drawing.Point(24, 36);
            this.comPortTextBox.MaxLength = 32767;
            this.comPortTextBox.Name = "comPortTextBox";
            this.comPortTextBox.PasswordChar = '\0';
            this.comPortTextBox.ScrollBars = System.Windows.Forms.ScrollBars.None;
            this.comPortTextBox.SelectedText = "";
            this.comPortTextBox.Size = new System.Drawing.Size(166, 23);
            this.comPortTextBox.TabIndex = 1;
            this.comPortTextBox.Text = "Enter COM port here";
            this.comPortTextBox.UseSelectable = true;
            this.comPortTextBox.Click += new System.EventHandler(this.comPortTextBox_Click);
            // 
            // startBtn
            // 
            this.startBtn.Location = new System.Drawing.Point(23, 74);
            this.startBtn.Name = "startBtn";
            this.startBtn.Size = new System.Drawing.Size(167, 37);
            this.startBtn.TabIndex = 0;
            this.startBtn.Text = "Start";
            this.startBtn.Theme = MetroFramework.MetroThemeStyle.Light;
            this.startBtn.UseSelectable = true;
            this.startBtn.Click += new System.EventHandler(this.startBtn_Click);
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tabPage1);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(20, 60);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(592, 364);
            this.tabControl.TabIndex = 0;
            this.tabControl.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.tabControl.UseSelectable = true;
            // 
            // simulateToggle
            // 
            this.simulateToggle.AutoSize = true;
            this.simulateToggle.Location = new System.Drawing.Point(110, 258);
            this.simulateToggle.Name = "simulateToggle";
            this.simulateToggle.Size = new System.Drawing.Size(80, 17);
            this.simulateToggle.TabIndex = 9;
            this.simulateToggle.Text = "Off";
            this.simulateToggle.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.simulateToggle.UseSelectable = true;
            this.simulateToggle.CheckStateChanged += new System.EventHandler(this.simulate_Toggle);
            // 
            // simulateLabel
            // 
            this.simulateLabel.AutoSize = true;
            this.simulateLabel.Location = new System.Drawing.Point(23, 256);
            this.simulateLabel.Name = "simulateLabel";
            this.simulateLabel.Size = new System.Drawing.Size(65, 19);
            this.simulateLabel.TabIndex = 10;
            this.simulateLabel.Text = "Simulate?";
            this.simulateLabel.Theme = MetroFramework.MetroThemeStyle.Dark;
            
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
            this.ClientSize = new System.Drawing.Size(632, 444);
            this.Controls.Add(this.tabControl);
            this.Name = "MainWindow";
            this.Text = "Gesture Recogniser";
            this.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.tabPage2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.metroGrid1)).EndInit();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gestureImage)).EndInit();
            this.tabControl.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion


        private Image ThumbsUp;
        private Image PeaceSign;
        private Image Flat;
        private MetroFramework.Controls.MetroTabPage tabPage2;
        private MetroFramework.Controls.MetroGrid metroGrid1;
        private MetroFramework.Controls.MetroTabPage tabPage1;
        private MetroFramework.Controls.MetroLabel gestureLabel;
        private PictureBox gestureImage;
        private MetroFramework.Controls.MetroTextBox comPortTextBox;
        private MetroFramework.Controls.MetroButton startBtn;
        private MetroFramework.Controls.MetroTabControl tabControl;
        private MetroFramework.Controls.MetroLabel recordLabel;
        private MetroFramework.Controls.MetroToggle recordToggle;
        private MetroFramework.Controls.MetroTextBox recordTextBox;
        private MetroFramework.Controls.MetroLabel simulateLabel;
        private MetroFramework.Controls.MetroToggle simulateToggle;
    }
}