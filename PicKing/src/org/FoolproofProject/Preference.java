package org.FoolproofProject;

import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JPanel;

import org.FoolproofProject.Configuration.Record;

public class Preference extends JDialog {

	private static final long serialVersionUID = -3910072899938489788L;
	private JComboBox unit;
	private NeturalField limit;
	private Main parent;
	
	public Preference( Main window ) {
		super( window );
		
		parent = window;
		setTitle( "Preferences" );
		
		Container pane = getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		JPanel limits = new JPanel();
		pane.add( limits );
		limits.setLayout( new GridLayout( 1, 2 ) );
		limits.setBorder( BorderFactory.createTitledBorder( "Limit" ) );
		
		limit = new NeturalField();
		limits.add( limit );
		
		unit = new JComboBox();
		limits.add( unit );
		unit.addItem( "B" );
		unit.addItem( "KB" );
		unit.addItem( "MB" );
		unit.addItem( "GB" );
		
		JPanel buttons = new JPanel();
		pane.add( buttons );
		buttons.setLayout( new GridLayout( 1, 3 ) );
		
		JButton ok = new JButton( "OK" );
		buttons.add( ok );
		ok.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				Configuration.save( new Record( limit.toLong(), unit.getSelectedIndex() ) );
				parent.read();
				setVisible( false );
			}
		} );
		JButton apply = new JButton( "Apply" );
		buttons.add( apply );
		apply.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				Configuration.save( new Record( limit.toLong(), unit.getSelectedIndex() ) );
				parent.read();
			}
		} );
		JButton cancel = new JButton( "Cancel" );
		buttons.add( cancel );
		cancel.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				setVisible( false );
			}
		} );
		
		pack();
	}
	
	public void exec( Long size, int index ) {
		limit.setText( size.toString() );
		unit.setSelectedIndex( index );
		setVisible( true );
	}

}
