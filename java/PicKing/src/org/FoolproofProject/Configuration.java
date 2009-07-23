package org.FoolproofProject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Scanner;

public class Configuration {
	
	public static class Record {
		public Long limit;
		public int unit;
		public Record() {
			limit = 4483L;
			unit = 2;
		}
		public Record( Long limit, int unit ) {
			this.limit = limit;
			this.unit = unit;
		}
	}
	
	private static final File file = new File( ".packing" );
	
	public static void save( Record r ) {
		if( file.exists() && file.isHidden() ) {
			try {
				Runtime.getRuntime().exec( String.format( "ATTRIB -H %s" , file.getAbsolutePath()) ).waitFor();
			} catch (IOException e) {
				ErrorLog.getInstance().log( e.getMessage() );
				return;
			} catch (InterruptedException e) {
				ErrorLog.getInstance().log( e.getMessage() );
			}
		}
		PrintStream fout = null;
		try {
			fout = new PrintStream( new FileOutputStream( file ) );
		} catch (FileNotFoundException e) {
			ErrorLog.getInstance().log( e.getMessage() );
			return;
		}
		fout.printf( "%d %d\n", r.limit, r.unit );
		fout.close();
		if( !file.isHidden() ) {
			try {
				Runtime.getRuntime().exec( String.format( "ATTRIB +H %s" , file.getAbsolutePath()) ).waitFor();
			} catch (IOException e) {
				ErrorLog.getInstance().log( e.getMessage() );
				return;
			} catch (InterruptedException e) {
				ErrorLog.getInstance().log( e.getMessage() );
				return;
			}
		}
	}
	
	public static Record load() {
		Record r = new Record();
		try {
			Scanner fin = new Scanner( new FileInputStream( file ) );
			r.limit = fin.nextLong();
			r.unit = fin.nextInt();
			fin.close();
		} catch (FileNotFoundException e) {
			save( r );
		}
		return r;
	}

}
