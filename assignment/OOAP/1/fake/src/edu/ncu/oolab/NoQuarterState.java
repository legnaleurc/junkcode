package edu.ncu.oolab;

public class NoQuarterState implements AbstractState {
	GumballMachine gumballMachine_;
	Signal inserted_;

	public NoQuarterState(GumballMachine gumballMachine) {
		this.gumballMachine_ = gumballMachine;
		this.inserted_ = new Signal(this);
	}

	public void insertQuarter() {
		System.out.println("You inserted a quarter");
		this.inserted().emit();
		// gumballMachine_.setState(gumballMachine_.getHasQuarterState());
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
}
