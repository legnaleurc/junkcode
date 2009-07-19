package org.FoolproofProject;

import java.io.File;
import java.io.FileFilter;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

public class DirectoryTree extends JPanel {
	
	private static final long serialVersionUID = -8724999594568776949L;
	private Vector< FileList > listener;
	
	public DirectoryTree() {
		this( "." );
	}
	
	public DirectoryTree( String path ) {
		this( new File( path ) );
	}
	
	public DirectoryTree( File root ) {
		DirectoryTreeModel model = new DirectoryTreeModel( root );
		JTree view = new JTree( model );
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
		
		view.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
		view.addTreeSelectionListener( new TreeSelectionListener() {
			@Override
			public void valueChanged(TreeSelectionEvent e) {
				TreePath[] selection = ( ( JTree )e.getSource() ).getSelectionPaths();
				if( selection.length == 1 ) {
					for( FileList list : listener ) {
						list.setItems( ( ShortFile )selection[0].getLastPathComponent() );
					}
				}
			}
		} );
		
		listener = new Vector< FileList >();
	}
	
	public void addFileListListener( FileList list ) {
		listener.add( list );
	}
	
	private class DirectoryTreeModel implements TreeModel {
		
		private ShortFile root;
		
		public DirectoryTreeModel( File root ) {
			this.root = new ShortFile( root );
		}

		@Override
		public void addTreeModelListener(TreeModelListener l) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public ShortFile getChild(Object parent, int index) {
			File[] children = ( ( File )parent ).listFiles( new DirectoryFilter() );
			if( children == null || ( index >= children.length ) ) {
				return null;
			}
			return new ShortFile( ( File )parent, children[index].getName() );
		}

		@Override
		public int getChildCount(Object parent) {
			File[] children = ( ( File )parent ).listFiles( new DirectoryFilter() );
			if( children == null ) {
				return 0;
			}
			return children.length;
		}

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			File[] children = ( ( File )parent ).listFiles( new DirectoryFilter() );
			if( children == null ) {
				return -1;
			}
			String childName = ( ( File )child ).getName();
			for( int i = 0; i < children.length; ++i ) {
				if( childName.equals( children[i] ) ) {
					return i;
				}
			}
			return -1;
		}

		@Override
		public ShortFile getRoot() {
			return root;
		}

		@Override
		public boolean isLeaf(Object node) {
			return ( ( File )node ).listFiles( new DirectoryFilter() ) == null;
		}

		@Override
		public void removeTreeModelListener(TreeModelListener l) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) {
			// TODO Auto-generated method stub
			
		}
		
		private class DirectoryFilter implements FileFilter {
			@Override
			public boolean accept(File file) {
				return file.isDirectory();
			}
		}
	}

}
