package org.foolproofproject.picking;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import org.foolproofproject.picking.cli.CommandLine;
import org.foolproofproject.picking.gui.LogDialog;
import org.foolproofproject.picking.gui.MainWindow;

public class Main {

	public static void main(String[] args) {
		if( args.length == 0 ) {
			SwingUtilities.invokeLater( new Runnable() {
				public void run() {
					try {
						UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
					} catch( Exception e ) {
						LogDialog.getErrorLog().log( e.getMessage() );
					}
					
					MainWindow self = new MainWindow( "PicKing" );
					self.setVisible(true);
				}
			} );
		} else {
			CommandLine.parse( args );
		}
	}

}
