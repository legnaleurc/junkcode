package edu.ncu.oolab;

public class GumballMachineTestDrive {

	public static void main(String[] args) {
		GumballMachine gumballMachine = new GumballMachine(10);

		SoldOutState soldOutState = new SoldOutState(gumballMachine);
		NoQuarterState noQuarterState = new NoQuarterState(gumballMachine);
		HasQuarterState hasQuarterState = new HasQuarterState(gumballMachine);
		SoldState soldState = new SoldState(gumballMachine);
		WinnerState winnerState = new WinnerState(gumballMachine);

		if (gumballMachine.getCount() > 0) {
			gumballMachine.setInitialState(noQuarterState);
		} else {
			gumballMachine.setInitialState(soldOutState);
		}

		gumballMachine.addTransition(noQuarterState.inserted(), hasQuarterState);

		gumballMachine.addTransition(hasQuarterState.ejected(), noQuarterState);
		gumballMachine.addTransition(hasQuarterState.won(), winnerState);
		gumballMachine.addTransition(hasQuarterState.sold(), soldState);

		gumballMachine.addTransition(soldState.noQuarter(), noQuarterState);
		gumballMachine.addTransition(soldState.soldOut(), soldOutState);

		gumballMachine.addTransition(winnerState.noQuarter(), noQuarterState);
		gumballMachine.addTransition(winnerState.soldOut(), soldOutState);

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
