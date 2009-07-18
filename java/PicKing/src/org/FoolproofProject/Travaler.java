package org.FoolproofProject;

import java.io.File;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

public class Travaler {
	
	public static Pair pick( Long limit, String path ) {
		return pick( limit, new File( path ) );
	}
	
	public static Pair pick( Long limit, File path ) {
		return pick( limit, walk( path ) );
	}
	
	public static Pair pick( Long limit, Hashtable< String, Long > items ) {
		Vector< Pair > table = new Vector< Pair >();
		table.add( new Pair() );
		
		for( Entry< String, Long > e : items.entrySet() ) {
			Vector< Pair > tmp = new Vector< Pair >();
			for( Pair p : table ) {
				Long newSize = p.size + e.getValue();
				if( newSize <= limit ) {
					Vector< String > newDirs = new Vector< String >( p.items );
					newDirs.add( e.getKey() );
					tmp.add( new Pair( newSize, newDirs ) );
				}
			}
			table.addAll(table.size(), tmp);
		}
		
		Pair max = new Pair();
		for( Pair p : table ) {
			if( p.size >= max.size ) {
				max = p;
			}
		}
		return max;
	}
	
	public static Hashtable< String, Long > walk( String path ) {
		return walk( new File( path ) );
	}
	
	public static Hashtable< String, Long > walk( File path ) {
		Hashtable< String, Long > table = new Hashtable< String, Long >();
		if( path.isDirectory() ) {
			for( File file : path.listFiles() ) {
				table.put(file.getAbsolutePath(), deepWalk( file ) );
			}
		}
		return table;
	}
	
	private static Long deepWalk( File path ) {
		if( path.isDirectory() ) {
			Long sum = path.length();
			for( File file : path.listFiles() ) {
				sum += deepWalk( file );
			}
			return sum;
		} else {
			return path.length();
		}
	}
	
	public static class Pair {
		public Long size;
		public Vector< String > items;
		public Pair() {
			size = 0L;
			items = new Vector< String >();
		}
		public Pair( Long size, Vector< String > items ) {
			this.size = size;
			this.items = items;
		}
		public String toString() {
			return "("+size+":"+items+")";
		}
	}

}
