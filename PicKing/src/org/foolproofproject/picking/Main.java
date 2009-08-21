package org.foolproofproject.picking;

import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import org.foolproofproject.picking.gui.LogDialog;
import org.foolproofproject.picking.gui.Window;

public class Main {

	public static void main(String[] args) {
		SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				try {
					UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
				} catch( Exception e ) {
					LogDialog.getErrorLog().log( e.getMessage() );
				}
				
				Window self = new Window( "PicKing" );
				self.setVisible(true);
			}
		} );
	}

}
