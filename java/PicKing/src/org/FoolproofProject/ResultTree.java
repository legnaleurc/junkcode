package org.FoolproofProject;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L;
	private JTree view;
	
	public ResultTree() {
		view = new JTree();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
		
		clear();
	}
	
	public void clear() {
		( ( DefaultTreeModel )view.getModel() ).setRoot( null );
	}
	
	public void addResult( Pair result ) {
		DefaultTreeModel model = ( DefaultTreeModel )view.getModel();
		DefaultMutableTreeNode root = ( DefaultMutableTreeNode )model.getRoot();
		if( root == null ) {
			root = new DefaultMutableTreeNode( "Results" );
			model.setRoot( root );
		}
		
		DefaultMutableTreeNode newNode = new DefaultMutableTreeNode( result.size );
		root.add( newNode );
		for( String name : result.items ) {
			newNode.add( new DefaultMutableTreeNode( name ) );
		}
		view.expandPath( new TreePath( newNode.getPath() ) );
	}

}
