package org.FoolproofProject;

import java.io.File;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

public class FileList extends JPanel {

	private static final long serialVersionUID = -5296371739711677521L;
	private JList view;
	
	public FileList( DirectoryTree tree ) {
		tree.addFileListListener( this );
		view = new JList();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
	}
	
	public void setItems( ShortFile path ) {
		view.setListData( path.listFiles() );
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
