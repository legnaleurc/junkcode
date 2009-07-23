package org.FoolproofProject;

import java.awt.Container;

import javax.swing.BoxLayout;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JTextArea;

public class ErrorLog extends JDialog {
	
	private static final long serialVersionUID = -8893137565616271012L;
	private static ErrorLog instance = new ErrorLog();
	private JTextArea textArea;
	
	public static ErrorLog getInstance() {
		return instance;
	}
	
	private ErrorLog() {
		super( ( Main )null );
		setSize( 320, 240 );
		setTitle( "Error Log" );
		
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
