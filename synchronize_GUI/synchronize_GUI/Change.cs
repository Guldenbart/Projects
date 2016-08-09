using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace synchronize_GUI
{
	enum ChangeType { TYPE_INVALID = -1, TYPE_UNDET = 0, TYPE_COPY = 1, TYPE_REPLACE = 2, TYPE_MKDIR = 3 };

	public class Change
	{
		private ChangeType _type;
		private StringBuilder _source;		// empty, if it's type '3'
		private StringBuilder _destination;
		private bool _hidden;
		private bool _doPerform;
		private StringBuilder _artist;

		/**
		 * -1: ungültig
		 * 0: nicht gesetzt
		 * 1: Datei kopieren
		 * 2: Datei ersetzen
		 * 3: Ordner erstellen
		 */
        public ChangeType type
		{
			get { 
				return _type;
			}
			set {
				if ((int)value < -1 || (int)value > 3) {
					this._type = ChangeType.TYPE_INVALID;
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

		public String artist
		{
			get
			{
				return this._artist.ToString();
			}
			set
			{
				this._artist = new StringBuilder(value);
			}
		}


		public Change() : this("", "", ChangeType.TYPE_UNDET, false, "N.A.") { }
		public Change(ChangeType t) : this("", "", t, false, "N.A.") { }
		public Change(String directory, ChangeType t, bool h) : this("", directory, t, h, "N.A.") { }

		public Change(String src, String dest, ChangeType type, bool hidden, String art)
		{
			this.source = src;
			this.destination = dest;
			this.type = type;
			this.hidden = hidden;
			this.doPerform = false;
			this.artist = art;
		}
		
		public void perform()
		{
			if (!this.doPerform) {
				return;
			}
			
			switch(this.type) {
			case ChangeType.TYPE_COPY:
				// Datei kopieren
				FileInfo fileCopy = new FileInfo(this.source);
				fileCopy.CopyTo(this.destination, false);
				break;
			
			case ChangeType.TYPE_REPLACE:
				// Datei ersetzen
				FileInfo fileReplace = new FileInfo(this.source);
				fileReplace.CopyTo(this.destination, true);
				break;
				
			case ChangeType.TYPE_MKDIR:
				//Ordner erstellen
				System.IO.Directory.CreateDirectory(this.destination);
				break;
				
			default:
				return;
			}
		}
	}
}
