using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace synchronize_GUI
{
	public class Change
	{
		private int _type;
		private StringBuilder _source;		// empty, if it's type '3'
		private StringBuilder _destination;
		private bool _hidden;
		private bool _doPerform;

		/**
		 * -1: ungültig
		 * 0: nicht gesetzt
		 * 1: Datei kopieren
		 * 2: Datei ersetzen
		 * 3: Ordner erstellen
		 */
        public int type
		{
			get { 
				return _type;
			}
			set {
				if (value < -1 || value > 3) {
					this._type = -1;
				}
				else {
					this._type = value;
				}
			}
		}

		public String source
		{
			get	{
				return this._source.ToString();
			}
			set {
				this._source = new StringBuilder(value);
			}
		}

		public String destination
		{
			get	{
				return this._destination.ToString();
			}
			set	{
				this._destination = new StringBuilder(value);
			}
		}

		public bool hidden
		{
			get	{
				return this._hidden;
			}
			set	{
				this._hidden = value;
			}
		}

		public bool doPerform
		{
			get {
				return this._doPerform;
			}
			set {
				this._doPerform = value;
			}
		}


		public Change() : this("", "", 0, false) { }
		public Change(int t) : this("", "", t, false) { }
		public Change(String directory, int t, bool h) : this("", directory, t, h) { }

		public Change(String src, String dest, int t, bool h)
		{
			this.source = src;
			this.destination = dest;
			this.type = t;
			this.hidden = h;
			this.doPerform = false;
		}
		
		public void perform()
		{
			if (!this.doPerform) {
				return;
			}
			
			switch(this.type) {
			case 1:
				// Datei kopieren
				FileInfo fileCopy = new FileInfo(this.source);
				fileCopy.CopyTo(this.destination, false);
				break;
			
			case 2:
				// Datei ersetzen
				FileInfo fileReplace = new FileInfo(this.source);
				fileReplace.CopyTo(this.destination, true);
				break;
				
			case 3:
				//Ordner erstellen
				System.IO.Directory.CreateDirectory(this.destination);
				break;
				
			default:
				return;
			}
		}
	}
}
