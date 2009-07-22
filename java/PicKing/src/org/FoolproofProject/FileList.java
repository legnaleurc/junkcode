package org.FoolproofProject;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

public class FileList extends JPanel {

	private static final long serialVersionUID = -5296371739711677521L;
	private JList view;
	private DirectoryTree parentTree;
	
	public FileList( DirectoryTree tree ) {
		parentTree = tree;
		parentTree.addFileListListener( this );
		view = new JList();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
		
		view.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				if( e.getClickCount() == 2 ) {
					File file = ( File )view.getModel().getElementAt( view.locationToIndex( e.getPoint() ) );
					parentTree.open( file );
				}
			}
		} );
	}
	
	public void setItems( ShortFile path ) {
		ShortFile[] files = path.listFiles();
		if( files != null ) {
			view.setListData( files );
		}
	}
	
	public Vector< File > getSelectedFiles() {
		Object[] selection = view.getSelectedValues();
		if( selection == null ) {
			return null;
		}

		Vector< File > tmp = new Vector< File >();
		for( Object o : selection ) {
			tmp.add( ( File )o );
		}
		return tmp;
	}

}
