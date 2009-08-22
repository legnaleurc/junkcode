package org.foolproofproject.picking;

import java.io.File;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;

import org.foolproofproject.Travaler;

public class Performer {
	
	public static class Result {
		private String title;
		private Vector< File > files;
		public Result( long size, long eng, Vector< File > files ) {
			title = ( size / ( long )Math.pow( 1024, eng ) ) + " " + unit[(int)eng];
			Collections.sort( files );
			this.files = files;
		}
		public String getTitle() {
			return title;
		}
		public Vector< File > getFiles() {
			return files;
		}
	}
	
	private static final String[] unit = new String[4];
	private final long limit, eng;
	private long size;
	private Hashtable< File, Long > items, table;
	private Vector< File > overflow;
	
	static {
		unit[0] = "B";
		unit[1] = "KB";
		unit[2] = "MB";
		unit[3] = "GB";
	}
	
	public Performer( long limit, long eng, File[] files ) {
		this.limit = limit * ( long )Math.pow( 1024, eng );
		this.eng = eng;
		size = 0L;
		items = new Hashtable< File, Long >();
		table = new Hashtable< File, Long >();
		overflow = new Vector< File >();
		
		for( File file : files ) {
			put( file, Travaler.getSize( file ) );
		}
	}
	
	public Result once() {
		Travaler.Result r = ( size < limit ) ? new Travaler.Result( size, new Vector< File >( items.keySet() ) ) : Travaler.pick( limit, items );
		return new Result( r.getSize(), eng, r.getItems() );
	}
	public void remove( Vector< File > keys ) {
		for( File key : keys ) {
			size -= items.get( key );
			items.remove( key );
		}
	}
	
	public Hashtable< File, Long > getTable() {
		return table;
	}
	public boolean noItem() {
		return items.isEmpty();
	}
	public boolean noOverflow() {
		return overflow.isEmpty();
	}
	public Vector< File > getOverflow() {
		return overflow;
	}
	
	private void put( File key, Long value ) {
		if( value < limit ) {
			size += value;
			items.put( key, value );
		} else {
			overflow.add( key );
		}
		table.put( key, value );
	}

}
