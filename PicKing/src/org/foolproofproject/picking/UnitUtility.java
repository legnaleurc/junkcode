package org.foolproofproject.picking;

import javax.swing.JComboBox;

public class UnitUtility {
	
	private static final String[] unit = new String[4];
	
	static {
		unit[0] = "B";
		unit[1] = "KB";
		unit[2] = "MB";
		unit[3] = "GB";
	}
	
	public static long extract( long value, int eng ) {
		return (long) (value * Math.pow( 1024, eng ));
	}
	
	public static String toString( long value, int eng ) {
		return String.format( "%d %s", (long)( value / Math.pow( 1024, eng ) ), unit[eng] );
	}
	
	public static String toString( long value ) {
		double size = value;
		int pow = 0;
		while( size >= 1024 ) {
			++pow;
			size /= 1024;
		}
		return String.format( "%.3f %s", size, unit[pow] );
	}
	
	public static JComboBox createComboBox() {
		JComboBox tmp = new JComboBox();
		for( String u : unit ) {
			tmp.addItem( u );
		}
		return tmp;
	}

}
