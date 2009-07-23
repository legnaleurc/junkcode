package org.FoolproofProject;

import java.io.File;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Map.Entry;

public class Travaler {

	public static class Result {
		
		public Long size;
		public Vector< File > items;
		
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

	}
	
	private static class GeneticAlgorithm {
		
		private class Cell implements Comparable< Cell > {
			public Hashtable< File, Boolean > table;
			public Long size;
			public Cell( Hashtable< File, Boolean > table, Long size ) {
				this.table = table;
				this.size = size;
			}
			public Cell( Cell cell ) {
				this.table = new Hashtable< File, Boolean >( cell.table );
				this.size = new Long( cell.size );
			}
			public Long update( Hashtable< File, Long > items ) {
				size = 0L;
				for( Entry< File, Boolean > e : table.entrySet() ) {
					if( e.getValue() ) {
						size += items.get( e.getKey() );
					}
				}
				return size;
			}
			@Override
			public int compareTo(Cell rhs) {
				if( rhs.size < this.size ) {
					return -1;
				}
				if( rhs.size > this.size ) {
					return 1;
				}
				return 0;
			}
		}
		
		private Long limit;
		private Hashtable< File, Long > items;
		private Cell[] population;
		
		public GeneticAlgorithm( Long limit, Hashtable< File, Long > items ) {
			this.limit = limit;
			this.items = items;
			
			population = new Cell[items.size() * 2];
			for( int i = 0; i < population.length; ++i ) {
				population[i] = generatePopulation();
			}
			Arrays.sort( population );
		}
		
		public Result perform() {
			while( !canStop() ) {
				crossOver();
				mutation();
				Arrays.sort( population );
			}
			
			Cell survivor = population[0];
			Result result = new Result( survivor.size, new Vector< File >() );
			for( Entry< File, Boolean > e : survivor.table.entrySet() ) {
				if( e.getValue() ) {
					result.items.add( e.getKey() );
				}
			}
			return result;
		}
		
		private Cell generatePopulation() {
			Hashtable< File, Boolean > cell = new Hashtable< File, Boolean >();
			Long sum = null;
			do {
				cell.clear();
				sum = 0L;
				for( Entry< File, Long > e : items.entrySet() ) {
					if( Math.random() * 2 < 1.0 ) {
						cell.put( e.getKey(), false );
					} else {
						cell.put( e.getKey(), true );
						sum += e.getValue();
						if( sum > limit ) {
							break;
						}
					}
				}
			} while( sum > limit );
			return new Cell( cell, sum );
		}
		
		private int selectParent( int i ) {
			int total = items.size() - i;
			if( i + 1 == total || Math.random() * total < 2.0 ) {
				return i;
			} else {
				return selectParent( i + 1 );
			}
		}
		
		private Boolean canStop() {
			Long head = population[0].size;
			Long tail = population[items.size()-1].size;
			return head.equals( tail );
		}
		
		private void crossOver() {
			final int halfSize = population.length / 2;
			for( int i = 0; i < halfSize; ++i ) {
				Cell new1 = new Cell( population[i] );
				Cell new2 = new Cell( population[selectParent( 0 )] );
				for( Entry< File, Boolean > e : new1.table.entrySet() ) {
					if( Math.random() * 2 < 1.0 ) {
						Boolean tmp = new2.table.get( e.getKey() );
						new2.table.put( e.getKey(), e.getValue() );
						e.setValue( tmp );
					}
				}
				new1 = ( new1.update( items ) > limit ) ? generatePopulation() : new1;
				new2 = ( new2.update( items ) > limit ) ? generatePopulation() : new2;
				population[halfSize + i] = ( new1.size > new2.size ) ? new1 : new2;
			}
		}
		
		private void mutation() {
			final int halfSize = population.length / 2;
			for( int i = halfSize; i < population.length; ++i ) {
				Cell cell = population[i];
				for( Entry< File, Boolean > e : cell.table.entrySet() ) {
					if( Math.random() * cell.table.size() < 1.0 ) {
						if( e.getValue() ) {
							cell.table.put( e.getKey(), false );
							cell.size -= items.get( e.getKey() );
						} else {
							cell.table.put( e.getKey(), true );
							cell.size += items.get( e.getKey() );
						}
					}
				}
				if( cell.size > limit ) {
					population[i] = generatePopulation();
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
				Long newSize = p.size + e.getValue();
				if( newSize <= limit ) {
					Vector< File > newDirs = new Vector< File >( p.items );
					newDirs.add( e.getKey() );
					tmp.add( new Result( newSize, newDirs ) );
				}
			}
			table.addAll(table.size(), tmp);
		}
		
		Result max = new Result();
		for( Result p : table ) {
			if( p.size >= max.size ) {
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
