package org.FoolproofProject;

import java.awt.Container;

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
				window.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
				window.setSize( 640, 480 );
				window.setVisible(true);
				
				Container pane = window.getContentPane();
				pane.setLayout( new BoxLayout( pane, BoxLayout.X_AXIS ) );
				
				DirectoryTree tree = new DirectoryTree();
				pane.add( tree );
				
				FileList list = new FileList( tree );
				pane.add( list );
			}
		} );
	}

}
