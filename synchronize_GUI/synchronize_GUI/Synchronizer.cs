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
        public bool doSubdirs { get; set; }			// Unterverzeichnisse mit einbeziehen
        public bool checkBeforeCopy { get; set; }	// Änderungen anzeigen, bevor sie umgesetzt werden können
        public bool flatCopy { get; set; }			// im Ziel nicht die Ordnerstruktur abbilden
		public bool extFileCheck { get; set; }		// Dateien mit demselben Namen auf Unterschiede prüfen
        
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

		/// <summary>
		/// bootstrap method for Synchronizing.
		/// </summary>
        public void synchronize()
        {
			synchronize_recursive(this.getSourcePath(), this.getDestinationPath());

			System.Windows.Forms.DialogResult dr = System.Windows.Forms.DialogResult.OK;
			if (this.checkBeforeCopy) {
				dr = ChangesWindow.showChangeWindow(ref this.changeList);
			}
			
			if (dr == System.Windows.Forms.DialogResult.OK) {
				foreach (Change changeItem in this.changeList) {
					changeItem.perform();
				}
			}
			else if (dr == System.Windows.Forms.DialogResult.Cancel) {
				// clear the change list, so that it will be refilled correctly next time.
				changeList.Clear();
			}
        }

		/// <summary>
		/// recursive method to got through all (files and) directories.
		/// </summary>
		/// <param name="srcPath">path where files are copied from.</param>
		/// <param name="destPath">path where files will be copied to.</param>
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
			foreach (System.IO.FileInfo file in sourceInfo.GetFiles().Where(s => s.Extension == ".mp3" || s.Extension == ".wma")) {

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

			TagLib.File tagSrcFile = TagLib.File.Create(completeSrcPath);
			TagLib.File tagDestFile = TagLib.File.Create(completeDestPath);
			String srcArtist = tagSrcFile.Tag.FirstAlbumArtist;
			String destArtist = tagDestFile.Tag.FirstAlbumArtist;

			if (!System.IO.File.Exists(completeDestPath.ToString())) {
				bool hidden = ((System.IO.File.GetAttributes(completeSrcPath) & System.IO.FileAttributes.Hidden) == System.IO.FileAttributes.Hidden);
				return new Change(completeSrcPath.ToString(), completeDestPath.ToString(), ChangeType.TYPE_COPY, hidden, srcArtist);
			} else if (this.extFileCheck && srcInfo.Length != destInfo.Length) {
				
				// look for files with the same name but not the same artist. (like "name (2)"; "name (3)"...)
				if (srcArtist != destArtist) {

				}

				return new Change(completeSrcPath.ToString(), completeDestPath.ToString(), ChangeType.TYPE_REPLACE, ((System.IO.File.GetAttributes(completeSrcPath) & System.IO.FileAttributes.Hidden) == System.IO.FileAttributes.Hidden), srcArtist);
			}

			return new Change(ChangeType.TYPE_INVALID);
		}

		/// <summary>
		/// method to check if a new directory has to be created
		/// </summary>
		/// <param name="destinationPath">path where the new directory will be created if it doesn't exist yet.</param>
		/// <param name="newDirectory">directory that will be created if not existing.</param>
		/// <returns>Change object that represents the directory creation.</returns>
		public Change directoryCheck(String destinationPath, String newDirectory)
		{
			String completeDestPath = String.Concat(destinationPath, @"\", newDirectory);

			if (!System.IO.Directory.Exists(completeDestPath.ToString())) {
				return new Change(completeDestPath.ToString(), ChangeType.TYPE_MKDIR, false);
			}

			return new Change(ChangeType.TYPE_INVALID);
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
