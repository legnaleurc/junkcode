package org.FoolproofProject;

import javax.swing.BoxLayout;
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
				window.setLayout( new BoxLayout( window.getContentPane(), BoxLayout.Y_AXIS ) );
				window.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
				window.setSize( 640, 480 );
				window.getContentPane().add( new DirectoryTree() );
				window.setVisible(true);
			}
		} );
	}

}
