package edu.ncu.oolab;

public class SoldState implements AbstractState {
	GumballMachine gumballMachine_;
	Signal noQuarter_;
	Signal soldOut_;

	public SoldState(GumballMachine gumballMachine) {
		this.gumballMachine_ = gumballMachine;
		this.noQuarter_ = new Signal(this);
		this.soldOut_ = new Signal(this);
	}

	public void insertQuarter() {
		System.out.println("Please wait, we're already giving you a gumball");
	}

	public void ejectQuarter() {
		System.out.println("Sorry, you already turned the crank");
	}

	public void turnCrank() {
		System.out.println("Turning twice doesn't get you another gumball!");
	}

	public void dispense() {
		gumballMachine_.releaseBall();
		if (gumballMachine_.getCount() > 0) {
			this.noQuarter_.emit();
			// gumballMachine.setState(gumballMachine.getNoQuarterState());
		} else {
			System.out.println("Oops, out of gumballs!");
			this.soldOut_.emit();
			// gumballMachine.setState(gumballMachine.getSoldOutState());
		}
	}

	public String toString() {
		return "dispensing a gumball";
	}

	public Signal noQuarter() {
		return this.noQuarter_;
	}

	public Signal soldOut() {
		return this.soldOut_;
	}
}
