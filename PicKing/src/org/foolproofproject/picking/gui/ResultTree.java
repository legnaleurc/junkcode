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
import org.foolproofproject.picking.UnitUtility;


public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L; 
	private JTree view;
	private Hashtable< ShortFile, Long > table;
	private JPopupMenu popup;
	private DefaultMutableTreeNode selectedNode;
	
	private class LabelNode extends DefaultMutableTreeNode {
		private static final long serialVersionUID = -3736698920372921805L;
		private int pow;
		public LabelNode( Long size, int pow ) {
			super( size );
			this.pow = pow;
		}
		public String toString() {
			return UnitUtility.toString( (Long)getUserObject(), pow );
		}
	}
	
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
					if( item instanceof ShortFile && table.containsKey( item ) ) {
						setToolTipText( UnitUtility.toString( table.get( item ) ) );
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
				if( tp != null && tp.getPathCount() == 2 ) {
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
	
	public void addResult( long size, int eng, Vector< ShortFile > items ) {
		getRoot().add( createNewNode( size, eng, items ) );
	}
	
	public void addOverflow( Vector< ShortFile > overflow ) {
		getRoot().add( createNewNode( "Overflow", overflow ) );
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
	
	private DefaultMutableTreeNode createNewNode( long size, int eng, Vector<?> items ) {
		DefaultMutableTreeNode newNode = new LabelNode( size, eng );
		for( Object item : items ) {
			newNode.add( new DefaultMutableTreeNode( item ) );
		}
		return newNode;
	}
	
	private DefaultMutableTreeNode createNewNode( String title, Vector<?> items ) {
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
	
	public void exportK3BProjectsTo( File dout ) {
		Long k3bBound = UnitUtility.extract( (Long)Configuration.get( "k3b_export_lower_bound" ), (Integer)Configuration.get( "k3b_export_bound_unit" ) );
		Vector< DefaultMutableTreeNode > tmp = new Vector< DefaultMutableTreeNode >();
		for( Enumeration< ? > e = getRoot().children(); e.hasMoreElements(); ) {
			DefaultMutableTreeNode node = (DefaultMutableTreeNode)e.nextElement();
			if( node instanceof LabelNode ) {
				Long size = (Long)node.getUserObject();
				if( size >= k3bBound ) {
					tmp.add( node );
				}
			}
		}
		String template = String.format( "%%%dd.k3b", String.valueOf( tmp.size() ).length() );
		for( int i = 0; i < tmp.size(); ++i ) {
			try {
				K3BUtility.export( new File( dout, String.format( template, i ) ), tmp.get( i ) );
			} catch (IOException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
			} catch (XMLStreamException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
			}
		}
	}

}
