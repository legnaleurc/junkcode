package org.sandbox;

import java.util.ArrayList;

public class Signal {

	public static interface Slot {
		public void call( Object sender, Object... args );
	}

	public Signal( Object sender ) {
		this.sender_ = sender;
		this.slots_ = new ArrayList< Slot >();
	}

	public Boolean connect( Slot slot ) {
		if( this.slots_.indexOf( slot ) >= 0 ) {
			return false;
		}
		this.slots_.add( slot );
		return true;
	}

	public void disconnect() {
		this.slots_.clear();
	}

	public Boolean disconnect( Slot slot ) {
		return this.slots_.remove( slot );
	}

	public void emit( Object... args ) {
		for( Slot slot : this.slots_ ) {
			slot.call( sender_, args );
		}
	}

	private Object sender_;
	private ArrayList< Slot > slots_;

}
