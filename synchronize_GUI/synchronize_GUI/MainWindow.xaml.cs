using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace synchronize_GUI
{
    /// <summary>
    /// Interaktionslogik für MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
		private Synchronizer synch;

		/// <summary>
		/// constructor of class MainWindow
		/// </summary>
        public MainWindow()
        {
            InitializeComponent();
			synch = new Synchronizer();

			debugSettings();

			/*
			System.Windows.Controls.CheckBox chb = new System.Windows.Controls.CheckBox();
			chb.Content = "Click me";
			chb.IsChecked = true;
			chb.Foreground = new SolidColorBrush(Colors.Orange);
			chb.IsChecked = true;
			chb.Margin = new Thickness(6,60,0,0);			 

			gridCheckBox.Children.Add(chb);
			 */ 
        }

		/// <summary>
		/// enables some settings only for debugging
		/// </summary>
		private void debugSettings()
		{
			this.listBox.SelectedIndex = 0;
			this.checkBoxCheckBeforeCopy.IsChecked = true;
			this.checkBoxExtended.IsChecked = true;
			this.checkBoxsubDir.IsChecked = true;
			textBoxSrc.Text = @"D:\stuff\testSrc";
			textBoxDst.Text = @"D:\stuff\testDest";
		}

		/// <summary>
		/// deletes the slash at the end of a path string if one exists.
		/// </summary>
		/// <param name="path">path to edited</param>
		/// <returns>path string with no slash at the end</returns>
		private String cutEndingSlash(String path)
		{
			if (path.EndsWith(@"\") || path.EndsWith(@"/")) {
				return path.Substring(0, path.Length - 1);
			}
			else {
				return path;
			}
		}

		/// <summary>
		/// click event for the button "buttonSrc"
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
        private void buttonSrc_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            System.Windows.Forms.DialogResult result = dialog.ShowDialog();

            if (result.ToString().Equals("OK")) {
                textBoxSrc.Text = dialog.SelectedPath;
            }
        }

		/// <summary>
		/// click event for the button "buttonDst"
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
        private void buttonDst_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new System.Windows.Forms.FolderBrowserDialog();
            System.Windows.Forms.DialogResult result = dialog.ShowDialog();

            if (result.ToString().Equals("OK")) {
                textBoxDst.Text = dialog.SelectedPath;
            }

        }

		/// <summary>
		/// click event for the button "buttonStart"
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
        private void buttonStart_Click(object sender, RoutedEventArgs e)
        {
            if (listBox.SelectedItem == null) {
				this.labelError.Content = "no profile selected!";
                return;
            } else if (textBoxSrc.Text == "") {
				this.labelError.Content = "no Source!";
				return;
			} else if (textBoxDst.Text == "") {
				this.labelError.Content = "no Destination!";
				return;
			}
            else {
				this.labelError.Content = "";
				synch.type = listBox.SelectedIndex + 1;
				synch.setDestinationPath(cutEndingSlash(textBoxDst.Text));
				synch.setSourcePath(cutEndingSlash(textBoxSrc.Text));
				synch.doSubdirs = checkBoxsubDir.IsChecked.Value;
				synch.checkBeforeCopy = checkBoxCheckBeforeCopy.IsChecked.Value;
				synch.extFileCheck = checkBoxExtended.IsChecked.Value;
				synch.flatCopy = checkBoxFlat.IsChecked.Value;

				synch.synchronize();
            }
            
        }
    }
}
