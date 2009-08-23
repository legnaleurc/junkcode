package org.foolproofproject.picking;

import java.io.File;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;

import org.foolproofproject.Pack;

public class Performer {
	
	public static class Result {
		private String title;
		private Vector< String > items;
		public Result( long size, long eng, Vector< String > files ) {
			title = ( size / ( long )Math.pow( 1024, eng ) ) + " " + unit[(int)eng];
			Collections.sort( files );
			this.items = files;
		}
		public String getTitle() {
			return title;
		}
		public Vector<String> getItems() {
			return items;
		}
	}
	
	private static final String[] unit = new String[4];
	private final long limit, eng;
	private long size;
	private Hashtable< String, Long > items, table;
	private Vector< String > overflow;
	
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
		items = new Hashtable< String, Long >();
		table = new Hashtable< String, Long >();
		overflow = new Vector< String >();
		
		for( File file : files ) {
			put( file.getName(), Pack.getFileSize( file ) );
		}
	}
	
	public Result once() {
		Pack r = ( size < limit ) ? new Pack( size, new Vector< String >( items.keySet() ) ) : Pack.pick( limit, items );
		return new Result( r.getSize(), eng, r.getItems() );
	}
	public void remove( Vector<String> keys ) {
		for( String key : keys ) {
			size -= items.get( key );
			items.remove( key );
		}
	}
	
	public Hashtable<String,Long> getTable() {
		return table;
	}
	public boolean noItem() {
		return items.isEmpty();
	}
	public boolean noOverflow() {
		return overflow.isEmpty();
	}
	public Vector<String> getOverflow() {
		return overflow;
	}
	
	private void put( String key, Long value ) {
		if( value < limit ) {
			size += value;
			items.put( key, value );
		} else {
			overflow.add( key );
		}
		table.put( key, value );
	}

}
