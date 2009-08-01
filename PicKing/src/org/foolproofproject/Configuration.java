package org.foolproofproject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Scanner;

import javax.swing.JOptionPane;

public class Configuration {
	
	private static final File file = new File( ".packing" );
	private static Configuration self;
	private long limit;
	private int unit;
	
	static {
		self = new Configuration();
		try {
			Scanner fin = new Scanner( new FileInputStream( file ) );
			self.limit = fin.nextLong();
			self.unit = fin.nextInt();
			fin.close();
		} catch (FileNotFoundException e) {
			try {
				save();
			} catch (Exception e1) {
				LogDialog.getErrorLog().log( e1.getMessage() );
			}
		}
		Runtime.getRuntime().addShutdownHook( new Thread() {
			public void run() {
				try {
					save();
				} catch (Exception e) {
					JOptionPane.showMessageDialog( null, e.getMessage() );
				}
			}
		} );
	}
	
	public static void setLimit( long limit ) {
		synchronized (self) {
			self.limit = limit;
		}
	}
	
	public static void setUnit( int unit ) {
		synchronized (self) {
			self.unit = unit;
		}
	}
	
	public static long getLimit() {
		return self.limit;
	}
	
	public static int getUnit() {
		return self.unit;
	}
	
	private Configuration() {
		limit = 4483L;
		unit = 2;
	}
	
	private static void save() throws InterruptedException, IOException {
		if( isWindows() && file.exists() && file.isHidden() ) {
			Runtime.getRuntime().exec( String.format( "ATTRIB -H %s" , file.getAbsolutePath()) ).waitFor();
		}
		PrintStream fout = null;
		try {
			fout = new PrintStream( new FileOutputStream( file ) );
			fout.printf( "%d %d\n", getLimit(), getUnit() );
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
