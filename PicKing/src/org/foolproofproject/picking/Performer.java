package org.foolproofproject.picking;

import java.io.File;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

import org.foolproofproject.Pack;

/**
 * Algorithm performer.
 * @author legnaleurc
 *
 */
public class Performer {
	
	/**
	 * Store picking result.
	 * @author legnaleurc
	 *
	 */
	public static class Result {
		private long size;
		private Vector< ShortFile > items;
		public Result( long size, Vector< ShortFile > files ) {
			this.size = size;
			Collections.sort( files );
			this.items = files;
		}
		public long getSize() {
			return size;
		}
		public Vector< ShortFile > getItems() {
			return items;
		}
	}
	
	private final long limit;
	private long size;
	private Hashtable< ShortFile, Long > items, table;
	private Vector< ShortFile > overflow;
	
	/**
	 * Constructor.
	 * @param limit Combination maximum size
	 * @param files
	 */
	public Performer( long limit, File[] files ) {
		this.limit = limit;
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
		return new Result( r.getSize(), tmp );
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
