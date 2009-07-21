package org.FoolproofProject;

import javax.swing.JTextField;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;

public class NeturalField extends JTextField {

	private static final long serialVersionUID = -4923755274608244338L;
	
	public NeturalField( int columns ) {
		super( "4483", columns );
	}
	
	public Long toLong() {
		return Long.parseLong( super.getText() );
	}
	
	protected Document createDefaultModel() {
		return new NeturalDocument();
	}
	
	private class NeturalDocument extends PlainDocument {
		
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
		
	}

}
