package org.FoolproofProject;

import java.util.Vector;

public class Pair {
	
	public Long size;
	public Vector< String > items;
	
	public Pair() {
		size = 0L;
		items = new Vector< String >();
	}
	
	public Pair( Long size, Vector< String > items ) {
		this.size = size;
		this.items = items;
	}
	
	public String toString() {
		return "("+size+":"+items+")";
	}

}
