package org.foolproofproject;

import java.util.Arrays;
import java.util.Hashtable;

import org.junit.BeforeClass;
import org.junit.Test;

public class PackTest {
	
	private static Hashtable< Object, Long > table = new Hashtable< Object, Long >();
	private static long limit = 0L;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		long totalSize = 0L;
		int tableSize = (int) (Math.random() * 128) + 16;
		for( int i = 0; i < tableSize; ++i ) {
			long single = (long) (Math.random() * 1073741824);
			table.put( Integer.toString( i ), single );
			totalSize += single;
		}
		limit = totalSize / 3;
	}

	@Test
	public void testPick() {
		long[] result = new long[10];
		for( int i = 0; i < 10; ++i ) {
			result[i] = Pack.pick( limit, table ).getSize();
		}
		Arrays.sort( result );
		for( int i = 0; i < 10; ++i ) {
			System.out.println( result[i] );
		}
	}

}
