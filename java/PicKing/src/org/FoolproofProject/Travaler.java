package org.FoolproofProject;

import java.io.File;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

public class Travaler {
	
	public static Pair pick( Long limit, Hashtable< File, Long > items ) {
		Vector< Pair > table = new Vector< Pair >();
		table.add( new Pair() );
		
		for( Entry< File, Long > e : items.entrySet() ) {
			Vector< Pair > tmp = new Vector< Pair >();
			for( Pair p : table ) {
				Long newSize = p.size + e.getValue();
				if( newSize <= limit ) {
					Vector< File > newDirs = new Vector< File >( p.items );
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
	
	public static Long getSize( File path ) {
		if( path.isDirectory() ) {
			Long sum = path.length();
			for( File file : path.listFiles() ) {
				sum += getSize( file );
			}
			return sum;
		} else {
			return path.length();
		}
	}

}
