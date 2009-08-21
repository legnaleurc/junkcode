package org.foolproofproject;

import java.io.File;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

import org.foolproofproject.picking.gui.Configuration;
import org.foolproofproject.picking.gui.LogDialog;

public class Travaler {

	public static class Result {
		
		private Long size;
		private Vector< File > items;
		
		public Result() {
			size = 0L;
			items = new Vector< File >();
		}
		public Result( Long size, Vector< File > items ) {
			this.size = size;
			this.items = items;
		}
		public String toString() {
			return "("+size+":"+items+")";
		}
		public Long getSize() {
			return size;
		}
		public Vector< File > getItems() {
			return items;
		}

	}
	
	private static class GeneticAlgorithm {
		
		private class Cell implements Comparable< Cell > {
			
			private Hashtable< File, Boolean > table;
			private Long size;
			
			public Cell( Hashtable< File, Boolean > table, Long size ) {
				this.table = table;
				this.size = size;
			}
			public Cell( Cell cell ) {
				this.table = new Hashtable< File, Boolean >( cell.table );
				this.size = new Long( cell.size );
			}
			public boolean canToggle( File key, Long value, Long limit ) {
				return ( table.get( key ) || size + value < limit );
			}
			public void toggle( File key, Long value ) {
				boolean tmp = table.get( key );
				table.put( key, !tmp );
				if( tmp ) {
					size -= value;
				} else {
					size += value;
				}
			}
			public Hashtable<File, Boolean> getTable() {
				return table;
			}
			public Long getSize() {
				return size;
			}
			public String toString() {
				return String.format( "(%d,%s)", size, table.keySet() );
			}
			@Override
			public int compareTo(Cell rhs) {
				return rhs.size.compareTo( size );
			}
		}
		
		private Long limit;
		private Hashtable< File, Long > items;
		private Vector< Cell > population;
		
		public GeneticAlgorithm( Long limit, Hashtable< File, Long > items ) {
			this.limit = limit;
			this.items = items;
			
			population = new Vector< Cell >();
			for( int i = 0; i < items.size(); ++i ) {
				population.add( generatePopulation() );
			}
			Collections.sort( population );
		}
		
		public Result perform() {
			while( !canStop() ) {
				crossOver();
				mutation();
				Collections.sort( population );
				population.subList( items.size(), population.size() ).clear();
			}
			
			Cell survivor = population.get( 0 );
			Result result = new Result( survivor.getSize(), new Vector< File >() );
			for( Entry< File, Boolean > e : survivor.getTable().entrySet() ) {
				if( e.getValue() ) {
					result.getItems().add( e.getKey() );
				}
			}
			return result;
		}
		
		private Cell generatePopulation() {
			Hashtable< File, Boolean > cell = new Hashtable< File, Boolean >();
			Long sum = 0L;
			for( Entry< File, Long > e : items.entrySet() ) {
				if( e.getValue() + sum >= limit || Math.random() * 2 < 1.0 ) {
					cell.put( e.getKey(), false );
				} else {
					cell.put( e.getKey(), true );
					sum += e.getValue();
				}
			}
			return new Cell( cell, sum );
		}
		
		/**
		 * Select parent index.
		 * 
		 * N := total size
		 * 
		 * Plain:
		 * int i = 0;
		 * for( ; Math.random() * ( N - i ) >= 2.0; ++i );
		 * return i;
		 * 
		 * PDF:
		 * P( X = i ) = 2(N-1-i)/(N(N-1))
		 * 
		 * CDF:
		 * P( X <= i ) = i(2N-1-i)/(N(N-1)) = C
		 * b = 2N-1
		 * d = b^2 - 4CN(N-1)
		 * i = (b-d^0.5)/2
		 */
		private int selectParent() {
			int n = items.size();
			int b = 2 * n - 1;
			double c = Math.random();
			double d = ( b * b - 1 ) * ( 1 - c ) + 1;
			double k = ( b - Math.sqrt( d ) ) / 2;
			return ( int )Math.floor( k );
		}
		
		private Boolean canStop() {
			Cell head = population.firstElement();
			Cell tail = population.lastElement();
			if( (Boolean) Configuration.get( "debug" ) ) {
				LogDialog.getDebugLog().log( "head: " + head );
				LogDialog.getDebugLog().log( "tail: " + tail );
			}
			return head.getSize().equals( tail.getSize() );
		}
		
		private void crossOver() {
			final int length = population.size();
			for( int i = 0; i < length; ++i ) {
				Cell new1 = new Cell( population.get( i ) );
				Cell new2 = new Cell( population.get( selectParent() ) );
				for( Entry< File, Long > e : items.entrySet() ) {
					if( new1.getTable().get( e.getKey() ) == new2.getTable().get( e.getKey() ) ) {
						break;
					}
					if( !new1.canToggle( e.getKey(), e.getValue(), limit ) || !new2.canToggle( e.getKey(), e.getValue(), limit ) ) {
						break;
					}
					if( Math.random() < 0.5 ) {
						new1.toggle( e.getKey(), e.getValue() );
						new2.toggle( e.getKey(), e.getValue() );
					}
				}
				population.add( new1 );
				population.add( new2 );
			}
		}
		
		private void mutation() {
			final int length = population.size();
			for( int i = 0; i < length; ++i ) {
				Cell cell = population.get( i );
				for( Entry< File, Long > e : items.entrySet() ) {
					if( cell.canToggle( e.getKey(), e.getValue(), limit) && Math.random() * items.size() < 1.0 ) {
						cell.toggle( e.getKey(), e.getValue() );
					}
				}
			}
		}
		
	}
	
	public static Result pick( Long limit, Hashtable< File, Long > items ) {
		if( items.size() < 16 ) {
			return pickSmall( limit, items );
		} else {
			return pickLarge( limit, items );
		}
	}
	
	public static Result pickLarge( Long limit, Hashtable< File, Long > items ) {
		GeneticAlgorithm ga = new GeneticAlgorithm( limit, items );
		return ga.perform();
	}
	
	public static Result pickSmall( Long limit, Hashtable< File, Long > items ) {
		Vector< Result > table = new Vector< Result >();
		table.add( new Result() );
		
		for( Entry< File, Long > e : items.entrySet() ) {
			Vector< Result > tmp = new Vector< Result >();
			for( Result p : table ) {
				Long newSize = p.getSize() + e.getValue();
				if( newSize <= limit ) {
					Vector< File > newDirs = new Vector< File >( p.getItems() );
					newDirs.add( e.getKey() );
					tmp.add( new Result( newSize, newDirs ) );
				}
			}
			table.addAll(table.size(), tmp);
		}
		
		Result max = new Result();
		for( Result p : table ) {
			if( p.getSize() >= max.getSize() ) {
				max = p;
			}
		}
		return max;
	}
	
	public static Long getSize( File path ) {
		if( path.isDirectory() ) {
			Long sum = path.length();
			File[] files = path.listFiles();
			if( files != null ) {
				for( File file : files ) {
					sum += getSize( file );
				}
			}
			return sum;
		} else {
			return path.length();
		}
	}

}
