package org.foolproofproject.picking.gui;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;


public class FileList extends JPanel {

	private static final long serialVersionUID = -5296371739711677521L;
	private JList view;
	private DirectoryTree parentTree;
	private JLabel items;
	
	public FileList( DirectoryTree tree ) {
		parentTree = tree;
		parentTree.addFileListListener( this );
		view = new JList();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		setBorder( BorderFactory.createTitledBorder( "File List" ) );
		add( scroll );
		
		view.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				if( e.getClickCount() == 2 ) {
					int index = view.locationToIndex( e.getPoint() );
					if( index >= 0 ) {
						File file = ( File )view.getModel().getElementAt( index );
						parentTree.open( file );
					}
				}
			}
		} );
		
		items = new JLabel( "0" );
		view.getSelectionModel().addListSelectionListener( new ListSelectionListener() {
			@Override
			public void valueChanged(ListSelectionEvent e) {
				items.setText( String.valueOf( view.getSelectedIndices().length ) );
			}
		} );
		add( items );
	}
	
	public void setItems( File[] files ) {
		if( files != null ) {
			view.setListData( files );
		} else {
			view.removeAll();
		}
	}
	
	public File[] getSelectedFiles() {
		Object[] selection = view.getSelectedValues();
		if( selection == null ) {
			return null;
		}

		File[] tmp = new File[selection.length];
		for( int i = 0; i < tmp.length; ++i ) {
			tmp[i] = (File)selection[i];
		}
		return tmp;
	}

}
