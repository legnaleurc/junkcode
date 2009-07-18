package org.FoolproofProject;

import java.io.File;
import java.io.FileFilter;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class DirectoryTree extends JPanel {
	
	private static final long serialVersionUID = -8724999594568776949L;
	
	public DirectoryTree() {
		this( File.listRoots()[0] );
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
		
		view.addTreeSelectionListener( new TreeSelectionListener() {
			@Override
			public void valueChanged(TreeSelectionEvent e) {
				TreePath[] paths = ( ( JTree )e.getSource() ).getSelectionPaths();
				System.err.println( "<?" );
				for( TreePath path : paths ) {
					System.err.println( path.getLastPathComponent() );
				}
				System.err.println( "?>" );
			}
		} );
	}
	
	private class DirectoryTreeModel implements TreeModel {
		
		private File root;
		
		public DirectoryTreeModel( File root ) {
			this.root = root;
		}

		@Override
		public void addTreeModelListener(TreeModelListener l) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public File getChild(Object parent, int index) {
			File[] children = ( ( File )parent ).listFiles( new DirectoryFilter() );
			if( children == null || ( index >= children.length ) ) {
				return null;
			}
			return new File( ( File )parent, children[index].getName() );
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
		public File getRoot() {
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
