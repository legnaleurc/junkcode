package org.foolproofproject.picking.cli;

import java.io.File;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.foolproofproject.picking.Performer;

public class CommandLine {
	
	private static final Pattern pattern = Pattern.compile( "(\\d+)((K|M|G)?B?)" );
	
	public static void parse( String[] args ) {
		Matcher m = pattern.matcher( args[0].toUpperCase() );
		if( !m.matches() ) {
			System.out.println( "Usage." );
		} else {
			long limit = Long.parseLong( m.group( 1 ) );
			String unit = m.group( 2 );
			long eng = 0L;
			if( unit.startsWith( "G" ) ) {
				eng = 3L;
			} else if( unit.startsWith( "M" ) ) {
				eng = 2L;
			} else if( unit.startsWith( "K" ) ) {
				eng = 1L;
			} else {
				;
			}
			
			File path = new File( ( args.length < 2 ) ? "." : args[1] );
			
			perform( path.listFiles(), limit, eng );
		}
	}
	
	private static void perform( File[] files, long limit, long eng ) {
		Performer p = new Performer( limit, eng, files );
		
		while( !p.noItem() ) {
			Performer.Result pair = p.once();
			System.out.println( pair.getTitle() + ":" );
			for( File item : pair.getFiles() ) {
				System.out.println( "\t" + item.getPath() );
			}
			p.remove( pair.getFiles() );
		}
		
		if( !p.noOverflow() ) {
			System.out.println( "Overflow:" );
			for( File item : p.getOverflow() ) {
				System.out.println( "\t" + item.getPath() );
			}
		}
	}

}
