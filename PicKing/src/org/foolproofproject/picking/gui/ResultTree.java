package org.foolproofproject.picking.gui;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.xml.stream.XMLStreamException;
import org.foolproofproject.picking.K3BUtility;
import org.foolproofproject.picking.ShortFile;


public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L; 
	private JTree view;
	private Hashtable< ShortFile, Long > table;
	private JPopupMenu popup;
	private DefaultMutableTreeNode selectedNode;
	
	public ResultTree() {
		view = new JTree();
		view.setRootVisible( false );
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		setBorder( BorderFactory.createTitledBorder( "Result" ) );
		add( scroll );
		
		selectedNode = null;
		
		// setup pop up context menu
		popup = new JPopupMenu();
		JMenuItem k3b = new JMenuItem( "Export to K3B project file" );
		k3b.addActionListener( new ActionListener() {
			public void actionPerformed( ActionEvent e ) {
				if( selectedNode == null ) {
					return;
				}
				File file = FileDialog.getSaveFileName( view, new FileNameExtensionFilter( "K3B project", "k3b" ) );
				if( file != null ) {
					try {
						K3BUtility.export( file, selectedNode );
					} catch (IOException e1) {
						LogDialog.getErrorLog().log( e1.getMessage() );
					} catch (XMLStreamException e1) {
						LogDialog.getErrorLog().log( e1.getMessage() );
					}
				}
			}
		} );
		popup.add( k3b );
		
		// setup tool tip
		ToolTipManager.sharedInstance().registerComponent( view );
		view.setCellRenderer( new DefaultTreeCellRenderer() {
			private static final long serialVersionUID = 8169622028702532699L;
			public Component getTreeCellRendererComponent( JTree tree, Object value, boolean sel, boolean expanded, boolean leaf, int row, boolean hasFocus ) {
				super.getTreeCellRendererComponent( tree, value, sel, expanded, leaf, row, hasFocus);
				if( table != null && leaf ) {
					DefaultMutableTreeNode node = ( DefaultMutableTreeNode )value;
					Object item = node.getUserObject();
					if( item instanceof String && table.containsKey( item ) ) {
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
		
		// setup mouse listener
		view.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				if( e.getButton() != MouseEvent.BUTTON3 ) {
					return;
				}
				TreePath tp = view.getPathForLocation( e.getX(), e.getY() );
				if( tp != null && view.getRowForPath( tp ) == 0 ) {
					selectedNode = ( DefaultMutableTreeNode )tp.getLastPathComponent();
					popup.show( view, e.getX(), e.getY());
				}
			}
		} );
		
		table = null;
		clear();
	}
	
	public void expandAll() {
		expand( new TreePath( getRoot() ) );
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
	
	public void setTable( Hashtable< ShortFile, Long > table ) {
		this.table = table;
	}
	
	public void addResult( String title, Vector< ShortFile > items ) {
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
	
	public void save( PrintStream fout ) {
		if( fout != null ) {
			savePath( new TreePath( getRoot() ), 0, fout );
		}
	}
	
	private void savePath( TreePath path, int indent, PrintStream fout ) {
		DefaultMutableTreeNode node = ( DefaultMutableTreeNode )path.getLastPathComponent();
		StringBuilder sb = new StringBuilder();
		for( int i = 0; i < indent; ++i ) {
			sb.append( '\t' );
		}
		sb.append( node.getUserObject() );
		fout.println( sb.toString() );
		for( Enumeration< ? > e = node.children(); e.hasMoreElements(); ) {
			savePath( path.pathByAddingChild( e.nextElement() ), indent + 1, fout );
		}
	}

}
