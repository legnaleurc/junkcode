package org.foolproofproject.picking.gui;

import javax.swing.JTextField;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;


public class NeturalField extends JTextField {

	private static final long serialVersionUID = -4923755274608244338L;
	
	public NeturalField() {
		super( "4483" );
	}
	
	public NeturalField( long number ) {
		super( String.valueOf( number ) );
	}
	
	public void setLong( long number ) {
		setText( String.valueOf( number ) );
	}
	
	public long toLong() {
		try {
			return Long.parseLong( super.getText() );
		} catch (NumberFormatException e) {
			LogDialog.getErrorLog().log( e.getMessage() );
			return 0L;
		}
	}
	
	protected Document createDefaultModel() {
		return new PlainDocument() {
			private static final long serialVersionUID = -3237520404172699543L;
			public void insertString( int offs, String str, AttributeSet a ) throws BadLocationException {
				if( str == null ) {
					return;
				}
				for( char c : str.toCharArray() ) {
					if( !Character.isDigit( c ) ) {
						return;
					}
				}
				super.insertString(offs, str, a);
			}
		};
	}

}
