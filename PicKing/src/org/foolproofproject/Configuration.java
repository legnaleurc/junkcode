package org.foolproofproject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

public class Configuration implements Serializable {
	
	private static final long serialVersionUID = 1450881336532749573L;
	private static final File file = new File( System.getProperty( "user.home" ) + File.separator + ".packing" );
	private static Configuration self;
	private long limit;
	private int unit;
	private boolean debug;
	
	static {
		try {
			ObjectInputStream fin = new ObjectInputStream( new FileInputStream( file ) );
			self = (Configuration) fin.readObject();
			fin.close();
		} catch (FileNotFoundException e) {
			self = new Configuration();
			try {
				sync();
			} catch (Exception e1) {
				LogDialog.getErrorLog().log( e1.getMessage() );
			}
		} catch (Exception e) {
			LogDialog.getErrorLog().log( e.getMessage() );
			LogDialog.getErrorLog().log( "Rolling back to default value." );
			self = new Configuration();
		}
	}
	
	public static void setLimit( long limit ) {
		synchronized (self) {
			self.limit = limit;
		}
	}
	public static long getLimit() {
		return self.limit;
	}
	
	public static void setUnit( int unit ) {
		synchronized (self) {
			self.unit = unit;
		}
	}
	
	public static int getUnit() {
		return self.unit;
	}
	
	public static void setDebug( boolean debug ) {
		synchronized (self) {
			self.debug = debug;
		}
	}
	
	public static boolean getDebug() {
		return self.debug;
	}
	
	private Configuration() {
		limit = 4483L;
		unit = 2;
		debug = false;
	}
	
	public static void sync() throws InterruptedException, IOException {
		if( isWindows() && file.exists() && file.isHidden() ) {
			Runtime.getRuntime().exec( String.format( "ATTRIB -H %s" , file.getAbsolutePath()) ).waitFor();
		}
		try {
			ObjectOutputStream fout = new ObjectOutputStream( new FileOutputStream( file ) );
			fout.writeObject( self );
			fout.close();
		} catch (FileNotFoundException e) {
			LogDialog.getErrorLog().log( e.getMessage() );
		}
		if( isWindows() && !file.isHidden() ) {
			Runtime.getRuntime().exec( String.format( "ATTRIB +H %s" , file.getAbsolutePath()) ).waitFor();
		}
	}
	
	private static boolean isWindows() {
		return System.getProperty( "os.name" ).startsWith( "Windows" );
	}

}
