package edu.ncu.oolab;

public class GumballMachine {

	AbstractState state_;
	int count = 0;

	public GumballMachine(int numberGumballs) {
		this.count = numberGumballs;
	}

	public void setInitialState(AbstractState state) {
		this.setState(state);
	}

	public void addTransition(Signal signal, AbstractState state) {
		final AbstractState STATE = state;
		signal.connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.setState(STATE);
			}
		});
	}

	public void insertQuarter() {
		state_.insertQuarter();
	}

	public void ejectQuarter() {
		state_.ejectQuarter();
	}

	public void turnCrank() {
		state_.turnCrank();
		state_.dispense();
	}

	private void setState(AbstractState state) {
		this.state_ = state;
	}

	void releaseBall() {
		System.out.println("A gumball comes rolling out the slot...");
		if (count != 0) {
			count = count - 1;
		}
	}

	public int getCount() {
		return count;
	}

	// void refill(int count) {
	// this.count = count;
	// state_ = noQuarterState_;
	// }

	public String toString() {
		StringBuffer result = new StringBuffer();
		result.append("\nMighty Gumball, Inc.");
		result.append("\nJava-enabled Standing Gumball Model #2004");
		result.append("\nInventory: " + count + " gumball");
		if (count != 1) {
			result.append("s");
		}
		result.append("\n");
		result.append("Machine is " + state_ + "\n");
		return result.toString();
	}
}
