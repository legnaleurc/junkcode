package org.FoolproofProject;

import java.io.File;
import java.net.URI;

public class ShortFile extends File {

	private static final long serialVersionUID = -498630998152519151L;

	public ShortFile(String pathname) {
		super(pathname);
	}

	public ShortFile(URI uri) {
		super(uri);
	}

	public ShortFile(String parent, String child) {
		super(parent, child);
	}

	public ShortFile(File parent, String child) {
		super(parent, child);
	}
	
	public ShortFile( File file ) {
		super( file.getPath() );
	}
	
	public String toString() {
		return this.getName();
	}
	
	public ShortFile[] listFiles() {
		if( !isDirectory() ) {
			return null;
		} else {
			File[] original = super.listFiles();
			if( original == null ) {
				return null;
			}
			ShortFile[] tmp = new ShortFile[original.length];
			for( int i = 0; i < tmp.length; ++i ) {
				tmp[i] = new ShortFile( original[i] );
			}
			return tmp;
		}
	}
	
	public ShortFile getParentFile() {
		File parent = super.getParentFile();
		if( parent == null ) {
			return null;
		} else {
			return new ShortFile( parent );
		}
	}

}
