package org.FoolproofProject;

import java.io.File;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

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
	
	public void addResult( String title, Vector< File > items ) {
		getRoot().add( createNewNode( title, items ) );
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
