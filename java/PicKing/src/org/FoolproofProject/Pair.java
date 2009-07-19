package org.FoolproofProject;

import java.io.File;
import java.util.Vector;

public class Pair {
	
	public Long size;
	public Vector< File > items;
	
	public Pair() {
		size = 0L;
		items = new Vector< File >();
	}
	
	public Pair( Long size, Vector< File > items ) {
		this.size = size;
		this.items = items;
	}
	
	public String toString() {
		return "("+size+":"+items+")";
	}

}
