package org.foolproofproject.picking.gui;

import java.io.File;
import java.io.FileFilter;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.BorderFactory;
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

import org.foolproofproject.picking.ShortFile;


public class DirectoryTree extends JPanel {
	
	private static final long serialVersionUID = -8724999594568776949L;
	private Vector< FileList > listener;
	private JTabbedPane tabWidget;
	private boolean hidden;
	
	public DirectoryTree() {
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		setBorder( BorderFactory.createTitledBorder( "Directory Tree" ) );
		listener = new Vector< FileList >();
		hidden = (Boolean) Configuration.get( "hidden" );
		
		tabWidget = new JTabbedPane();
		add( tabWidget );
		createTabs();
		tabWidget.addChangeListener( new ChangeListener() {
			@Override
			public void stateChanged(ChangeEvent e) {
				dispatch( getCurrentTree() );
			}
		} );
	}
	
	private void dispatch( JTree tree ) {
		if( tree != null ) {
			TreePath selection = tree.getSelectionPath();
			if( selection != null ) {
				File file = ( File )selection.getLastPathComponent();
				File[] items = file.listFiles( new DirectoryFilter() );
				for( FileList list : listener ) {
					list.setItems( items );
				}
			}
		}
	}
	
	private void createTabs() {
		for( File root : File.listRoots() ) {
			tabWidget.addTab( root.getPath(), createRootTab( root ) );
		}
	}
	
	private JScrollPane createRootTab( File root ) {
		JTree view = new JTree( new DirectoryTreeModel( root ) );
		view.getSelectionModel().setSelectionMode( TreeSelectionModel.SINGLE_TREE_SELECTION );
		view.addTreeSelectionListener( new TreeSelectionListener() {
			@Override
			public void valueChanged(TreeSelectionEvent e) {
				dispatch( ( JTree )e.getSource() );
			}
		} );
		return new JScrollPane( view );
	}
	
	private JTree getCurrentTree() {
		JScrollPane current = ( JScrollPane )tabWidget.getSelectedComponent();
		if( current == null ) {
			return null;
		}
		JTree tree = ( JTree )current.getViewport().getView();
		return tree;
	}
	
	public void setHidden( boolean hidden ) {
		this.hidden = hidden;
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
			
			TreePath path = new TreePath( list.toArray() );
			getCurrentTree().setSelectionPath( path );
		}
	}
	
	public void refresh() {
		// dump state
		Hashtable< File, TreePath > sel = new Hashtable< File, TreePath >();
		File curRoot = null;
		for( int i = 0; i < tabWidget.getTabCount(); ++i ) {
			JScrollPane tab = ( JScrollPane )tabWidget.getComponentAt( i );
			JTree tree = ( JTree )tab.getViewport().getView();
			File root = ( File )tree.getModel().getRoot();
			TreePath path = tree.getSelectionPath();
			if( path != null ) {
				sel.put( root, path );
			}
			
			if( tabWidget.getSelectedIndex() == i ) {
				curRoot = root;
			}
		}
		
		// clear tabs
		tabWidget.removeAll();
		createTabs();
		
		// restore state
		for( int i = 0; i < tabWidget.getTabCount(); ++i ) {
			JScrollPane tab = ( JScrollPane )tabWidget.getComponentAt( i );
			JTree tree = ( JTree )tab.getViewport().getView();
			File root = ( File )tree.getModel().getRoot();
			TreePath path = sel.get( root );
			if( path != null ) {
				tree.setSelectionPath( path );
			}
			if( root.equals( curRoot ) ) {
				tabWidget.setSelectedIndex( i );
				dispatch( tree );
			}
		}
	}
	
	private class DirectoryTreeModel implements TreeModel {
		
		private File root;
		
		public DirectoryTreeModel( File root ) {
			this.root = new ShortFile( root );
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
		
	}
	
	private class DirectoryFilter implements FileFilter {
		@Override
		public boolean accept(File file) {
			return file.isDirectory() && ( !hidden ? !file.isHidden() : true );
		}
	}

}
