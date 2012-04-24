package edu.ncu.oolab;

public class HasFakeQuarterState implements AbstractState {
	GumballMachine gumballMachine_;
	Signal ejected_;

	public HasFakeQuarterState(GumballMachine gumballMachine) {
		this.gumballMachine_ = gumballMachine;
		this.ejected_ = new Signal(this);
	}

	public void insertQuarter() {
		System.out.println("You can't insert another quarter");
	}

	public void ejectQuarter() {
		System.out.println("Quarter returned");
		this.ejected().emit();
	}

	public void turnCrank() {
		System.out.println("You can not turn");
		this.ejected().emit();
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
}
