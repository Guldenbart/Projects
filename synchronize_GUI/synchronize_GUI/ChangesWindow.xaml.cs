using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Collections;
using System.Windows.Forms;

namespace synchronize_GUI
{
	/// <summary>
	/// Interaktionslogik für ChangesWindow.xaml
	/// </summary>
	public partial class ChangesWindow : Window
	{
		// Erforderliche Designervariable
		//private System.ComponentModel.Container components = null;

		private static System.Windows.Forms.DialogResult dr;
		private static ArrayList changeArray;

		protected ChangesWindow()
		{
			InitializeComponent();
			dr = System.Windows.Forms.DialogResult.None;
		}


		public static System.Windows.Forms.DialogResult showChangeWindow(ref ArrayList changeList)
		{
			ChangesWindow cw = new ChangesWindow();

			changeArray = changeList;

			int index = 0;
			foreach (Change changeItem in changeList) {
				System.Windows.Controls.CheckBox chb = new System.Windows.Controls.CheckBox();
				chb.Content = generateCheckboxContent(changeItem);
				chb.IsChecked = true;
				chb.Background = new SolidColorBrush(generateCheckboxBackground(changeItem.type));
				chb.Margin = new Thickness(6, (index * 0) + 6, 0, 0);
				chb.Height = 16;
				chb.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
				chb.VerticalAlignment = VerticalAlignment.Top;
				chb.Name = String.Concat("checkBox", index);

				cw.stackPanelCheckbox.Children.Add(chb);

				index++;
			}

			//cw.Activate();
			cw.ShowDialog();

			return dr;
		}


		private static IEnumerable<T> FindVisualChildren<T>(DependencyObject depObj) where T : DependencyObject
		{
			if (depObj != null) {
				for (int i = 0; i < VisualTreeHelper.GetChildrenCount(depObj); i++) {
					DependencyObject child = VisualTreeHelper.GetChild(depObj, i);
					if (child != null && child is T) {
						yield return (T)child;
					}

					foreach (T childOfChild in FindVisualChildren<T>(child)) {
						yield return childOfChild;
					}
				}
			}
		}


		private void buttonOK_Click(object sender, RoutedEventArgs e)
		{
			dr = System.Windows.Forms.DialogResult.OK;

			foreach (System.Windows.Controls.CheckBox cb in FindVisualChildren<System.Windows.Controls.CheckBox>(this)) {
				String cbString = cb.Name;
				int i = Convert.ToInt32(cbString.Substring(8, (cbString.Length - 7 - 1)));

				if (cb.IsChecked.Value) {
					((Change)changeArray[i]).doPerform = true;
				}
			}

			this.Close();
		}


		private void buttonCancel_Click(object sender, RoutedEventArgs e)
		{
			dr = System.Windows.Forms.DialogResult.Cancel;
			
			this.Close();
		}


		private static String generateCheckboxContent(Change c)
		{
			StringBuilder retVal = new StringBuilder("<");

			switch (c.type) {
				case 1:
					retVal.Append("Datei kopieren>    ");
					break;
				case 2:
					retVal.Append("Datei ersetzen>    ");
					break;
				case 3:
					retVal.Append("Ordner erstellen> ");
					break;
				default:
					retVal.Append("ungültig!");
					break;
			}
			retVal.Append(c.destination);

			return retVal.ToString();
		}


		private static Color generateCheckboxBackground(int type)
		{
			switch (type) {
				case 1:
					return Colors.LimeGreen;
				case 2:
					return Colors.Crimson;
				case 3:
					return Colors.Gold;
				default:
					return Colors.White;
			}
		}

		private void buttonSelectAll_Click(object sender, RoutedEventArgs e)
		{
			foreach (System.Windows.Controls.CheckBox cb in FindVisualChildren<System.Windows.Controls.CheckBox>(this)) {
				cb.IsChecked = true;
			}
		}

		private void buttonDeselectAll_Click(object sender, RoutedEventArgs e)
		{
			foreach (System.Windows.Controls.CheckBox cb in FindVisualChildren<System.Windows.Controls.CheckBox>(this)) {
				cb.IsChecked = false;
			}
		}

	}
}
