package org.FoolproofProject;

import java.awt.Component;
import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;

public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L;
	private JTree view;
	private Hashtable< File, Long > table;
	
	public ResultTree() {
		view = new JTree();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
		
		ToolTipManager.sharedInstance().registerComponent( view );
		view.setCellRenderer( new DefaultTreeCellRenderer() {
			private static final long serialVersionUID = 8169622028702532699L;
			public Component getTreeCellRendererComponent( JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus ) {
				super.getTreeCellRendererComponent( tree, value, sel, expanded, leaf, row, hasFocus);
				if( table != null && leaf ) {
					DefaultMutableTreeNode node = ( DefaultMutableTreeNode )value;
					Object item = node.getUserObject();
					if( item instanceof ShortFile ) {
						double size = table.get( item ).doubleValue();
						int pow = 0;
						while( size >= 1024 ) {
							++pow;
							size /= 1024;
						}
						setToolTipText( String.format( "%.3f %s", size, getUnit( pow ) ) );
					}
				}
				return this;
			}
		} );
		
		table = null;
		clear();
	}
	
	public void expandAll() {
		expand( new TreePath( view.getModel().getRoot() ) );
	}
	
	private void expand( TreePath path ) {
		view.expandPath( path );
		TreeNode node = ( TreeNode )path.getLastPathComponent();
		for( Enumeration< ? > e = node.children(); e.hasMoreElements(); ) {
			expand( path.pathByAddingChild( e.nextElement() ) );
		}
	}
	
	public void clear() {
		( ( DefaultTreeModel )view.getModel() ).setRoot( null );
	}
	
	public void setTable( Hashtable< File, Long > table ) {
		this.table = table;
	}
	
	public void addResult( String title, Vector< File > items ) {
		getRoot().add( createNewNode( title, items ) );
	}
	
	private String getUnit( int pow ) {
		switch( pow ) {
		case 0:
			return "B";
		case 1:
			return "KB";
		case 2:
			return "MB";
		case 3:
			return "GB";
		default:
			return "";
		}
	}
	
	private DefaultMutableTreeNode getRoot() {
		DefaultTreeModel model = ( DefaultTreeModel )view.getModel();
		DefaultMutableTreeNode root = ( DefaultMutableTreeNode )model.getRoot();
		if( root == null ) {
			root = new DefaultMutableTreeNode( "Results" );
			model.setRoot( root );
		}
		return root;
	}
	
	private DefaultMutableTreeNode createNewNode( Object title, Vector<?> items ) {
		DefaultMutableTreeNode newNode = new DefaultMutableTreeNode( title );
		for( Object item : items ) {
			newNode.add( new DefaultMutableTreeNode( item ) );
		}
		return newNode;
	}

}
