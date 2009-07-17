package org.FoolproofProject;

import java.io.File;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;

public class FileTree extends JPanel {
	
	private static final long serialVersionUID = -8724999594568776949L;
	
	public FileTree() {
		this( "." );
	}
	
	public FileTree( String path ) {
		this( new File( path ) );
	}
	
	public FileTree( File root ) {
		FileTreeModel model = new FileTreeModel( root );
		JTree view = new JTree( model );
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
	}
	
	private class FileTreeModel implements TreeModel {
		
		private File root;
		
		public FileTreeModel( File root ) {
			this.root = root;
		}

		@Override
		public void addTreeModelListener(TreeModelListener l) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public File getChild(Object parent, int index) {
			String[] children = ( ( File )parent ).list();
			if( children == null || ( index >= children.length ) ) {
				return null;
			}
			return new File( ( File )parent, children[index] );
		}

		@Override
		public int getChildCount(Object parent) {
			String[] children = ( ( File )parent ).list();
			if( children == null ) {
				return 0;
			}
			return children.length;
		}

		@Override
		public int getIndexOfChild(Object parent, Object child) {
			String[] children = ( ( File )parent ).list();
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
			return ( ( File )node ).isFile();
		}

		@Override
		public void removeTreeModelListener(TreeModelListener l) {
			// TODO Auto-generated method stub
			
		}

		@Override
		public void valueForPathChanged(TreePath path, Object newValue) {
			// TODO Auto-generated method stub
			
		}
	}

}
