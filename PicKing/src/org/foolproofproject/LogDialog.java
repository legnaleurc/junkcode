package org.foolproofproject;

import java.awt.Container;

import javax.swing.BoxLayout;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JTextArea;

public class LogDialog extends JDialog {
	
	private static final long serialVersionUID = -8893137565616271012L;
	private static LogDialog error = new LogDialog( "Error Log" );
	private static LogDialog debug = new LogDialog( "Debug Log" );
	private JTextArea textArea;
	
	public static LogDialog getErrorLog() {
		return error;
	}
	
	public static LogDialog getDebugLog() {
		return debug;
	}
	
	private LogDialog( String title ) {
		super( ( Main )null );
		setSize( 320, 240 );
		setTitle( title );
		
		Container pane = getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		JPanel main = new JPanel();
		pane.add( main );
		main.setLayout( new BoxLayout( main, BoxLayout.Y_AXIS ) );
		
		textArea = new JTextArea();
		main.add( textArea );
		textArea.setEditable( false );
	}
	
	public void log( String msg ) {
		synchronized( textArea ) {
			textArea.append( msg + "\n" );
		}
	}

}
