package edu.ncu.oolab;

import java.util.ArrayList;

public class Signal {

	public static interface Slot {
		void call(Object sender, Object... args);
	}

	private Object sender_;
	private ArrayList<Slot> slots_;

	public Signal(Object sender) {
		this.sender_ = sender;
		this.slots_ = new ArrayList<Slot>();
	}

	public synchronized Boolean connect(Slot slot) {
		if (this.slots_.indexOf(slot) >= 0) {
			return false;
		}
		this.slots_.add(slot);
		return null;
	}

	public synchronized void disconnect() {
		this.slots_.clear();
	}

	public synchronized Boolean disconnect(Slot slot) {
		return this.slots_.remove(slot);
	}

	public synchronized void emit(Object... args) {
		for (Slot slot : this.slots_) {
			slot.call(this.sender_, args);
		}
	}

}
