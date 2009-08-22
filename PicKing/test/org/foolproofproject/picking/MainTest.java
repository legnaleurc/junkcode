package org.foolproofproject.picking;

import org.junit.Test;

public class MainTest {

	@Test
	public void testMain() {
		String[] args = new String[1];
		args[0] = "help";
		Main.main( args );
		
		args[0] = "ramdom";
		Main.main( args );
		
		args[0] = "1024K";
		Main.main( args );
		
		args = new String[2];
		args[0] = "random";
		args[1] = "dummy";
		Main.main( args );
		
		args[0] = "1024MB";
		args[1] = "src";
		Main.main( args );
		
		args = new String[3];
		args[0] = "1024GB";
		args[1] = "dummy";
		args[1] = "dummy";
		Main.main( args );
	}

}
