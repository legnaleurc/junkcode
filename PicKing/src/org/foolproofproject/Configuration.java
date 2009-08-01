package org.foolproofproject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Scanner;

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
			save();
		}
		Runtime.getRuntime().addShutdownHook( new Thread() {
			public void run() {
				save();
			}
		} );
	}
	
	public static Configuration get() {
		synchronized (self) {
			return self;
		}
	}
	
	public static void set( Configuration c ) {
		synchronized (self) {
			self = c;
		}
	}
	
	private static void save() {
		if( file.exists() && file.isHidden() ) {
			try {
				Runtime.getRuntime().exec( String.format( "ATTRIB -H %s" , file.getAbsolutePath()) ).waitFor();
			} catch (IOException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
				return;
			} catch (InterruptedException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
			}
		}
		PrintStream fout = null;
		try {
			fout = new PrintStream( new FileOutputStream( file ) );
		} catch (FileNotFoundException e) {
			LogDialog.getErrorLog().log( e.getMessage() );
			return;
		}
		fout.printf( "%d %d\n", self.getLimit(), self.getUnit() );
		fout.close();
		if( !file.isHidden() ) {
			try {
				Runtime.getRuntime().exec( String.format( "ATTRIB +H %s" , file.getAbsolutePath()) ).waitFor();
			} catch (IOException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
				return;
			} catch (InterruptedException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
				return;
			}
		}
	}
	
	public Configuration() {
		limit = 4483L;
		unit = 2;
	}
	
	public Configuration( long limit, int unit ) {
		this.limit = limit;
		this.unit = unit;
	}
	
	public long getLimit() {
		return limit;
	}
	
	public int getUnit() {
		return unit;
	}

}
