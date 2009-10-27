package org.foolproofproject.picking.gui;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.Hashtable;


public class Configuration implements Serializable {
	
	private static final long serialVersionUID = -6422237746611091558L;
	private static final File file = new File( System.getProperty( "user.home" ) + File.separator + ".packing" );
	private static Configuration self;
	private Hashtable< String, Object > data;
	
	static {
		try {
			ObjectInputStream fin = new ObjectInputStream( new FileInputStream( file ) );
			self = (Configuration) fin.readObject();
			if( self.data == null ) {
				throw new Exception( "Configuration format is mismatched." );
			}
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
	
	public static void set( String key, Object value ) {
		synchronized (self) {
			self.data.put(key, value);
		}
	}
	
	public static Object get( String key ) {
		return self.data.get( key );
	}
	
	private Configuration() {
		data = new Hashtable< String, Object >();
		data.put( "limit", 4483L );
		data.put( "unit", 2 );
		data.put( "k3b_export_lower_bound", 4000L );
		data.put( "k3b_export_bound_unit", 2 );
		data.put( "debug", false );
		data.put( "hidden", false );
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
