package org.foolproofproject.picking;

import java.io.File;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

import org.foolproofproject.Pack;

public class Performer {
	
	public static class Result {
		private String title;
		private Vector< ShortFile > items;
		public Result( long size, long eng, Vector< ShortFile > files ) {
			title = ( size / ( long )Math.pow( 1024, eng ) ) + " " + unit[(int)eng];
			Collections.sort( files );
			this.items = files;
		}
		public String getTitle() {
			return title;
		}
		public Vector< ShortFile > getItems() {
			return items;
		}
	}
	
	private static final String[] unit = new String[4];
	private final long limit, eng;
	private long size;
	private Hashtable< ShortFile, Long > items, table;
	private Vector< ShortFile > overflow;
	
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
		items = new Hashtable< ShortFile, Long >();
		table = new Hashtable< ShortFile, Long >();
		overflow = new Vector< ShortFile >();
		
		for( File f : files ) {
			ShortFile file = new ShortFile( f );
			put( file, file.getTotalSize() );
		}
	}
	
	public Result once() {
		Pack r = null;
		if( size < limit ) {
			r = new Pack( size, new Vector< Object >( items.keySet() ) );
		} else {
			Hashtable< Object, Long > tmp = new Hashtable< Object, Long >();
			for( Entry< ShortFile, Long > e : items.entrySet() ) {
				tmp.put( e.getKey(), e.getValue() );
			}
			r = Pack.pick( limit, tmp );
		}
		Vector< ShortFile > tmp = new Vector< ShortFile >();
		for( Object o : r.getItems() ) {
			tmp.add( ( ShortFile )o );
		}
		return new Result( r.getSize(), eng, tmp );
	}
	public void remove( Vector< ShortFile > keys ) {
		for( ShortFile key : keys ) {
			size -= items.get( key );
			items.remove( key );
		}
	}
	
	public Hashtable< ShortFile, Long > getTable() {
		return table;
	}
	public boolean noItem() {
		return items.isEmpty();
	}
	public boolean noOverflow() {
		return overflow.isEmpty();
	}
	public Vector< ShortFile > getOverflow() {
		return overflow;
	}
	
	private void put( ShortFile key, Long value ) {
		if( value < limit ) {
			size += value;
			items.put( key, value );
		} else {
			overflow.add( key );
		}
		table.put( key, value );
	}

}
