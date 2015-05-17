using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Windows.Media;
using System.Windows;

namespace synchronize_GUI
{
	public class Synchronizer
	{
        private StringBuilder destinationPath { get; set; }
        private StringBuilder sourcePath { get; set; }
        public bool doSubdirs { get; set; }
        public bool checkBeforeCopy { get; set; }
        public bool flatCopy { get; set; }
		public bool extFileCheck { get; set; }
        
        public int type { get; set; } // what, how (see listBox)
        public ArrayList changeList;

        /* ########### Methoden ########### */

		public Synchronizer()
		{
            this.destinationPath = new StringBuilder();
            this.sourcePath = new StringBuilder();
			this.doSubdirs = true;
			this.checkBeforeCopy = true;
			this.flatCopy = false;          // only useful for mp3 -> SD
            changeList = new ArrayList();
		}
 
        public String getDestinationPath() 
        {
            return this.destinationPath.ToString();
        }

		public void setDestinationPath(String newString)
        {
			this.destinationPath = new StringBuilder(newString);
        }

		public void appendDestinationPath(String s)
		{
			this.destinationPath.Append(s);
		}

        public String getSourcePath()
        {
            return this.sourcePath.ToString();
        }

		public void setSourcePath(String newString)
        {
			this.sourcePath = new StringBuilder(newString);
        }

		public void appendSourcePath(String s)
		{
			this.sourcePath.Append(s);
		}

        public void synchronize()
        {
			synchronize_recursive(this.getSourcePath(), this.getDestinationPath());

			System.Windows.Forms.DialogResult dr = System.Windows.Forms.DialogResult.OK;
			if (this.checkBeforeCopy) {
				dr = ChangesWindow.showChangeWindow(ref this.changeList);
			}
			
			if (dr == System.Windows.Forms.DialogResult.OK) {
				foreach (Change item in this.changeList) {
					item.perform();
				}
			}
        }

		public void synchronize_recursive(String srcPath, String destPath)
		{
			/*
			 * get directory information
			 */
			System.IO.DirectoryInfo sourceInfo = new System.IO.DirectoryInfo(srcPath);
			System.IO.DirectoryInfo destinationInfo = new System.IO.DirectoryInfo(destPath);

			/*
			 * check files in directory
			 */
			//foreach (String file in System.IO.Directory.EnumerateFiles(srcPath, "*.*", System.IO.SearchOption.TopDirectoryOnly).Where(s => s.EndsWith(".mp3") || s.EndsWith(".wma"))) {
			foreach (System.IO.FileInfo file in sourceInfo.GetFiles().Where(s => s.Extension == ".mp3" || s.Extension == ".wma")) {

				//Change c = fileCheck(srcPath, destPath, file);
				Change c = fileCheck(srcPath, destPath, file.ToString());
				/*
				 * file doesn't exist in destination directory, so copy it
				 */
				if (c.type > 0) {
					this.changeList.Add(c);
				}
				else if (this.extFileCheck) {
					//System.IO.Directory.GetFiles();
				}
			}

			/*
			 * check which directories exist and which still have to be created
			 */
			if (!this.flatCopy) {
				foreach (System.IO.DirectoryInfo directory in sourceInfo.GetDirectories()) {
					Change c = directoryCheck(destPath, directory.ToString());
					if (c.type > 0) {
						this.changeList.Add(c);
					}
				}
			}

			/*
			 * if there are no subdirectories, return to higher level
			 */
			if (sourceInfo.GetDirectories().Length == 0) {
				return;
			}

			/*
			 * otherwise, go deeper
			 */
			foreach (System.IO.DirectoryInfo dir in sourceInfo.GetDirectories()) {
				if (this.type == 4 && dir.ToString().Equals("Filmmusik")) {
					continue;
				}
				if (!this.flatCopy) {
					synchronize_recursive(String.Concat(srcPath, @"\", dir.ToString()), String.Concat(destPath, @"\", dir.ToString()));
				}
				else {
					synchronize_recursive(String.Concat(srcPath, @"\", dir.ToString()), destPath);
				}
			}
		}

		public Change fileCheck(String sourcePath, String destinationPath, String sourceFile)
		{
			String completeDestPath = String.Concat(destinationPath, @"\", sourceFile);
			String completeSrcPath = String.Concat(sourcePath, @"\", sourceFile);

			System.IO.FileInfo srcInfo = new System.IO.FileInfo(completeSrcPath/*.ToString()*/);
			System.IO.FileInfo destInfo = new System.IO.FileInfo(completeDestPath/*.ToString()*/);

			if (!System.IO.File.Exists(completeDestPath.ToString())) {				
				return new Change(completeSrcPath.ToString(), completeDestPath.ToString(), 1, ((System.IO.File.GetAttributes(completeSrcPath) & System.IO.FileAttributes.Hidden) == System.IO.FileAttributes.Hidden));
			} else if (this.extFileCheck && srcInfo.Length != destInfo.Length) {
				return new Change(completeSrcPath.ToString(), completeDestPath.ToString(), 2, ((System.IO.File.GetAttributes(completeSrcPath) & System.IO.FileAttributes.Hidden) == System.IO.FileAttributes.Hidden));
			}

			return new Change(-1);
		}

		public Change directoryCheck(String destinationPath, String newDirectory)
		{
			String completeDestPath = String.Concat(destinationPath, @"\", newDirectory);

			if (!System.IO.Directory.Exists(completeDestPath.ToString())) {
				return new Change(completeDestPath.ToString(), 3, false);
			}

			return new Change(-1);
		}

		/*
		public void list_Changes()
		{
			//ChangesWindow cw = new ChangesWindow();

			int index = 1;
			foreach (Change changeItem in this.changeList) {
				System.Windows.Controls.CheckBox chb = new System.Windows.Controls.CheckBox();
				chb.Content = generateCheckboxContent(changeItem);
				chb.IsChecked = true;
				chb.Background = new SolidColorBrush(generateCheckboxBackground(changeItem.type));
				chb.Margin = new Thickness(6, (index * 0) + 6, 0, 0);
				chb.Height = 16;
				chb.HorizontalAlignment = HorizontalAlignment.Left;
				chb.VerticalAlignment = VerticalAlignment.Top;
				chb.Name = String.Concat("checkBox", index);

				//cw.stackPanelCheckbox.Children.Add(chb);

				index++;
			}
		}
*/
		private void performChanges()
		{

		}
	}
}
