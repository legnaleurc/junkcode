package org.FoolproofProject;

import java.io.File;
import java.io.FileFilter;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTree;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.TreeModelListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.TreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

public class DirectoryTree extends JPanel {
	
	private static final long serialVersionUID = -8724999594568776949L;
	private Vector< FileList > listener;
	private JTabbedPane tabWidget;
	
	public DirectoryTree() {
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		listener = new Vector< FileList >();
		
		tabWidget = new JTabbedPane();
		add( tabWidget );
		for( File root : File.listRoots() ) {
			tabWidget.addTab( root.getPath(), createRootTab( root ) );
		}
		tabWidget.addChangeListener( new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				JScrollPane tab = ( JScrollPane )tabWidget.getSelectedComponent();
				JTree tree = ( JTree )tab.getViewport().getView();
				dispatch( tree );
			}
		} );
	}
	
	private void dispatch( JTree tree ) {
		TreePath[] selection = tree.getSelectionPaths();
		if( selection != null && selection.length == 1 ) {
			for( FileList list : listener ) {
				list.setItems( ( File )selection[0].getLastPathComponent() );
			}
		}
	}
	
	private JScrollPane createRootTab( File root ) {
		JTree view = new JTree( new DirectoryTreeModel( root ) );
		view.setRootVisible( false );
		view.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
		view.addTreeSelectionListener( new TreeSelectionListener() {
			@Override
			public void valueChanged(TreeSelectionEvent e) {
				dispatch( ( JTree )e.getSource() );
			}
		} );
		return new JScrollPane( view );
	}
	
	public void addFileListListener( FileList list ) {
		listener.add( list );
	}
	
	public void open( File file ) {
		if( file.isDirectory() ) {
			File root = file;
			Vector< File > list = new Vector< File >();
			while( root.getParentFile() != null ) {
				list.insertElementAt( root, 0 );
				root = root.getParentFile();
			}
			list.insertElementAt( root, 0 );
			File[] roots = File.listRoots();
			for( int i = 0; i < roots.length; ++i ) {
				if( roots[i].equals( root ) ) {
					tabWidget.setSelectedIndex( i );
					break;
				}
			}
			
			JScrollPane current = ( JScrollPane )tabWidget.getSelectedComponent();
			JTree tree = ( JTree )current.getViewport().getView();
			TreePath path = new TreePath( list.toArray() );
			tree.setSelectionPath( path );
		}
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
