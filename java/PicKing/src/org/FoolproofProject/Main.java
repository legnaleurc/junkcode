package org.FoolproofProject;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class Main {

	public static void main(String[] args) {
		SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				try {
					UIManager.setLookAndFeel(UIManager.getLookAndFeel());
				} catch( Exception e ) {
					System.err.println( e.getMessage() );
				}
				
				JFrame window = new JFrame();
				window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
				window.add( new FileTree() );
				window.pack();
				window.setVisible(true);
			}
		} );
	}

}
