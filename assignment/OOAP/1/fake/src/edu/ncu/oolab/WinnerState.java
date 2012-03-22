package edu.ncu.oolab;

public class WinnerState implements AbstractState {
	GumballMachine gumballMachine_;
	Signal soldOut_;
	Signal noQuarter_;

	public WinnerState(GumballMachine gumballMachine) {
		this.gumballMachine_ = gumballMachine;
		this.soldOut_ = new Signal(this);
		this.noQuarter_ = new Signal(this);
	}

	public void insertQuarter() {
		System.out.println("Please wait, we're already giving you a Gumball");
	}

	public void ejectQuarter() {
		System.out.println("Please wait, we're already giving you a Gumball");
	}

	public void turnCrank() {
		System.out.println("Turning again doesn't get you another gumball!");
	}

	public void dispense() {
		System.out.println("YOU'RE A WINNER! You get two gumballs for your quarter");
		gumballMachine_.releaseBall();
		if (gumballMachine_.getCount() == 0) {
			this.soldOut_.emit();
			// gumballMachine_.setState(gumballMachine_.getSoldOutState());
		} else {
			gumballMachine_.releaseBall();
			if (gumballMachine_.getCount() > 0) {
				this.noQuarter_.emit();
				// gumballMachine_.setState(gumballMachine_.getNoQuarterState());
			} else {
				System.out.println("Oops, out of gumballs!");
				this.soldOut_.emit();
				// gumballMachine_.setState(gumballMachine_.getSoldOutState());
			}
		}
	}

	public String toString() {
		return "despensing two gumballs for your quarter, because YOU'RE A WINNER!";
	}

	public Signal soldOut() {
		return this.soldOut_;
	}

	public Signal noQuarter() {
		return this.noQuarter_;
	}
}
