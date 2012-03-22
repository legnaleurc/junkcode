package edu.ncu.oolab;

public class GumballMachine {

	SoldOutState soldOutState_;
	NoQuarterState noQuarterState_;
	HasQuarterState hasQuarterState_;
	SoldState soldState_;
	WinnerState winnerState_;

	AbstractState state = soldOutState_;
	int count = 0;

	public GumballMachine(int numberGumballs) {
		this.soldOutState_ = new SoldOutState(this);
		this.noQuarterState_ = new NoQuarterState(this);
		this.hasQuarterState_ = new HasQuarterState(this);
		this.soldState_ = new SoldState(this);
		this.winnerState_ = new WinnerState(this);

		this.noQuarterState_.inserted().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.hasQuarterState_;
			}
		});

		this.hasQuarterState_.ejected().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.noQuarterState_;
			}
		});
		this.hasQuarterState_.won().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.winnerState_;
			}
		});
		this.hasQuarterState_.sold().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.soldState_;
			}
		});

		this.soldState_.noQuarter().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.noQuarterState_;
			}
		});
		this.soldState_.soldOut().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.soldOutState_;
			}
		});

		this.winnerState_.noQuarter().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.noQuarterState_;
			}
		});
		this.winnerState_.soldOut().connect(new Signal.Slot() {
			@Override
			public void call(Object sender, Object... args) {
				GumballMachine.this.state = GumballMachine.this.soldOutState_;
			}
		});

		this.count = numberGumballs;
		if (numberGumballs > 0) {
			state = noQuarterState_;
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
		state = noQuarterState_;
	}

	public AbstractState getState() {
		return state;
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
