package edu.ncu.oolab;

import java.util.Random;

public class HasQuarterState implements AbstractState {
	Random randomWinner_;
	GumballMachine gumballMachine_;
	Signal ejected_;
	Signal won_;
	Signal sold_;

	public HasQuarterState(GumballMachine gumballMachine) {
		this.randomWinner_ = new Random(System.currentTimeMillis());
		this.gumballMachine_ = gumballMachine;
		this.ejected_ = new Signal(this);
		this.won_ = new Signal(this);
		this.sold_ = new Signal(this);
	}

	public void insertQuarter() {
		System.out.println("You can't insert another quarter");
	}

	public void ejectQuarter() {
		System.out.println("Quarter returned");
		this.ejected().emit();
		// gumballMachine_.setState(gumballMachine_.getNoQuarterState());
	}

	public void turnCrank() {
		System.out.println("You turned...");
		int winner = randomWinner_.nextInt(10);
		if ((winner == 0) && (gumballMachine_.getCount() > 1)) {
			this.won().emit();
			// gumballMachine_.setState(gumballMachine_.getWinnerState());
		} else {
			this.sold().emit();
			// gumballMachine_.setState(gumballMachine_.getSoldState());
		}
	}

	public void dispense() {
		System.out.println("No gumball dispensed");
	}

	public String toString() {
		return "waiting for turn of crank";
	}

	public Signal ejected() {
		return this.ejected_;
	}

	public Signal won() {
		return this.won_;
	}

	public Signal sold() {
		return this.sold_;
	}
}
