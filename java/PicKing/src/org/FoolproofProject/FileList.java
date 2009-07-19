package org.FoolproofProject;

import java.io.File;
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
	
	public void setItems( File path ) {
		view.setListData( path.listFiles() );
	}

}
