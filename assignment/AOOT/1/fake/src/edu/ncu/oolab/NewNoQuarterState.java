package edu.ncu.oolab;

import java.util.Random;

public class NewNoQuarterState implements AbstractState {
	GumballMachine gumballMachine_;
	Random random_;
	Signal inserted_;
	Signal fakeInserted_;

	public NewNoQuarterState(GumballMachine gumballMachine) {
		this.gumballMachine_ = gumballMachine;
		this.random_ = new Random(System.currentTimeMillis());
		this.inserted_ = new Signal(this);
		this.fakeInserted_ = new Signal(this);
	}

	public void insertQuarter() {
		Integer r = this.random_.nextInt(3);
		if (r == 0) {
			System.out.println("You inserted a fake quarter");
			this.fakeInserted().emit();
		} else {
			System.out.println("You inserted a quarter");
			this.inserted().emit();
		}
	}

	public void ejectQuarter() {
		System.out.println("You haven't inserted a quarter");
	}

	public void turnCrank() {
		System.out.println("You turned, but there's no quarter");
	}

	public void dispense() {
		System.out.println("You need to pay first");
	}

	public String toString() {
		return "waiting for quarter";
	}

	public Signal inserted() {
		return this.inserted_;
	}

	public Signal fakeInserted() {
		return this.fakeInserted_;
	}
}
