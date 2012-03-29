package edu.ncu.oolab;

public class GumballMachineTestDrive {

	public static void main(String[] args) {
		GumballMachine gumballMachine = new GumballMachine(10);

		SoldOutState soldOutState = new SoldOutState(gumballMachine);
		HasQuarterState hasQuarterState = new HasQuarterState(gumballMachine);
		SoldState soldState = new SoldState(gumballMachine);
		WinnerState winnerState = new WinnerState(gumballMachine);
		NewNoQuarterState newNoQuarterState = new NewNoQuarterState(gumballMachine);
		HasFakeQuarterState hasFakeQuarterState = new HasFakeQuarterState(gumballMachine);

		if (gumballMachine.getCount() > 0) {
			gumballMachine.setInitialState(newNoQuarterState);
		} else {
			gumballMachine.setInitialState(soldOutState);
		}

		gumballMachine.addTransition(newNoQuarterState.inserted(), hasQuarterState);
		gumballMachine.addTransition(newNoQuarterState.fakeInserted(), hasFakeQuarterState);

		gumballMachine.addTransition(hasQuarterState.ejected(), newNoQuarterState);
		gumballMachine.addTransition(hasQuarterState.won(), winnerState);
		gumballMachine.addTransition(hasQuarterState.sold(), soldState);

		gumballMachine.addTransition(soldState.noQuarter(), newNoQuarterState);
		gumballMachine.addTransition(soldState.soldOut(), soldOutState);

		gumballMachine.addTransition(winnerState.noQuarter(), newNoQuarterState);
		gumballMachine.addTransition(winnerState.soldOut(), soldOutState);

		gumballMachine.addTransition(hasFakeQuarterState.ejected(), newNoQuarterState);

		System.out.println(gumballMachine);

		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();
		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();

		System.out.println(gumballMachine);

		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();
		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();

		System.out.println(gumballMachine);

		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();
		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();

		System.out.println(gumballMachine);

		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();
		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();

		System.out.println(gumballMachine);

		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();
		gumballMachine.insertQuarter();
		gumballMachine.turnCrank();

		System.out.println(gumballMachine);
	}
}
