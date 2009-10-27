package org.foolproofproject.picking.cli;

import java.io.File;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.foolproofproject.picking.Performer;
import org.foolproofproject.picking.ShortFile;
import org.foolproofproject.picking.UnitUtility;

public class CommandLine {
	
	private static final Pattern pattern = Pattern.compile( "(\\d+)((K|M|G)?B?)" );
	
	public static void parse( String[] args ) {
		Matcher m = pattern.matcher( args[0].toUpperCase() );
		if( !m.matches() ) {
			System.out.println( "Usage." );
		} else {
			long limit = Long.parseLong( m.group( 1 ) );
			String unit = m.group( 2 );
			int eng = 0;
			if( unit.startsWith( "G" ) ) {
				eng = 3;
			} else if( unit.startsWith( "M" ) ) {
				eng = 2;
			} else if( unit.startsWith( "K" ) ) {
				eng = 1;
			} else {
				;
			}
			
			File path = new File( ( args.length < 2 ) ? "." : args[1] );
			if( !path.isDirectory() ) {
				path = new File( "." );
			}
			
			perform( path.listFiles(), UnitUtility.extract( limit, eng ), eng );
		}
	}
	
	private static void perform( File[] files, long limit, int eng ) {
		Performer p = new Performer( limit, files );
		
		while( !p.noItem() ) {
			Performer.Result pair = p.once();
			System.out.println( UnitUtility.toString( pair.getSize(), eng ) + ":" );
			for( ShortFile item : pair.getItems() ) {
				System.out.println( "\t" + item );
			}
			p.remove( pair.getItems() );
		}
		
		if( !p.noOverflow() ) {
			System.out.println( "Overflow:" );
			for( ShortFile item : p.getOverflow() ) {
				System.out.println( "\t" + item );
			}
		}
	}

}
