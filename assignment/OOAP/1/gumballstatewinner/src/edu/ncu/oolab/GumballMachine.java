package edu.ncu.oolab;

public class GumballMachine {

	AbstractState soldOutState;
	AbstractState noQuarterState;
	AbstractState hasQuarterState;
	AbstractState soldState;
	AbstractState winnerState;

	AbstractState state = soldOutState;
	int count = 0;

	public GumballMachine(int numberGumballs) {
		soldOutState = new SoldOutState(this);
		noQuarterState = new NoQuarterState(this);
		hasQuarterState = new HasQuarterState(this);
		soldState = new SoldState(this);
		winnerState = new WinnerState(this);

		this.count = numberGumballs;
		if (numberGumballs > 0) {
			state = noQuarterState;
		}
	}

	public void insertQuarter() {
		state.insertQuarter();
	}

	public void ejectQuarter() {
		state.ejectQuarter();
	}

	public void turnCrank() {
		state.turnCrank();
		state.dispense();
	}

	void setState(AbstractState state) {
		this.state = state;
	}

	void releaseBall() {
		System.out.println("A gumball comes rolling out the slot...");
		if (count != 0) {
			count = count - 1;
		}
	}

	int getCount() {
		return count;
	}

	void refill(int count) {
		this.count = count;
		state = noQuarterState;
	}

	public AbstractState getState() {
		return state;
	}

	public AbstractState getSoldOutState() {
		return soldOutState;
	}

	public AbstractState getNoQuarterState() {
		return noQuarterState;
	}

	public AbstractState getHasQuarterState() {
		return hasQuarterState;
	}

	public AbstractState getSoldState() {
		return soldState;
	}

	public AbstractState getWinnerState() {
		return winnerState;
	}

	public String toString() {
		StringBuffer result = new StringBuffer();
		result.append("\nMighty Gumball, Inc.");
		result.append("\nJava-enabled Standing Gumball Model #2004");
		result.append("\nInventory: " + count + " gumball");
		if (count != 1) {
			result.append("s");
		}
		result.append("\n");
		result.append("Machine is " + state + "\n");
		return result.toString();
	}
}
