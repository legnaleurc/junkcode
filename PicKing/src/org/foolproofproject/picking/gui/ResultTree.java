package org.foolproofproject.picking.gui;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JFileChooser;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.ToolTipManager;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreeNode;
import javax.swing.tree.TreePath;
import javax.xml.stream.XMLOutputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamWriter;

import org.foolproofproject.picking.ShortFile;


public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L;
	private static XMLOutputFactory xmlFactory = XMLOutputFactory.newInstance(); 
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
		
		// setup popup context menu
		popup = new JPopupMenu();
		JMenuItem k3b = new JMenuItem( "Export to K3B project file" );
		k3b.addActionListener( new ActionListener() {
			public void actionPerformed( ActionEvent e ) {
				if( selectedNode == null ) {
					return;
				}
				JFileChooser fc = new JFileChooser();
				switch( fc.showSaveDialog( view ) ) {
				case JFileChooser.APPROVE_OPTION:
					exportToK3B( fc.getSelectedFile(), selectedNode );
					break;
				case JFileChooser.CANCEL_OPTION:
					break;
				case JFileChooser.ERROR_OPTION:
					break;
				default:
					;
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
	
	private void exportToK3B( File file, DefaultMutableTreeNode node ) {
		try {
			FileOutputStream fout = new FileOutputStream( file );
			ZipOutputStream zout = new ZipOutputStream( new BufferedOutputStream( fout ) );
			
			zout.putNextEntry( new ZipEntry( "mimetype" ) );
			zout.write( "application/x-k3b".getBytes( "UTF-8" ) );
			zout.closeEntry();
			
			zout.putNextEntry( new ZipEntry( "maindata.xml" ) );
			writeK3BXML( zout, node );
			zout.closeEntry();
			
			zout.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void writeK3BXML( ZipOutputStream zout, DefaultMutableTreeNode node ) {
		try {
			XMLStreamWriter xout = xmlFactory.createXMLStreamWriter( new OutputStreamWriter( zout, "UTF-8" ) );
			xout.writeStartDocument( "UTF-8", "1.0" );
			xout.writeDTD( "k3b_dvd_project" );
			xout.writeStartElement( "k3b_dvd_project" );
			
			xout.writeStartElement( "general" );
			xout.writeStartElement( "writing_mode" );
			xout.writeCharacters( "auto" );
			xout.writeEndElement();
			xout.writeEmptyElement( "dummy" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "on_the_fly" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEmptyElement( "only_create_images" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "remove_images" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEndElement();

			xout.writeStartElement( "options" );
			xout.writeEmptyElement( "rock_ridge" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEmptyElement( "joliet" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEmptyElement( "udf" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "joliet_allow_103_characters" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEmptyElement( "iso_allow_lowercase" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_allow_period_at_begin" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_allow_31_char" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEmptyElement( "iso_omit_version_numbers" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_omit_trailing_period" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_max_filename_length" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_relaxed_filenames" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_no_iso_translate" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_allow_multidot" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "iso_untranslated_filenames" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "follow_symbolic_links" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "create_trans_tbl" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "hide_trans_tbl" );
			xout.writeAttribute( "activated", "no");
			xout.writeStartElement( "iso_level" );
			xout.writeCharacters( "2" );
			xout.writeEndElement();
			xout.writeEmptyElement( "discard_symlinks" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "discard_broken_symlinks" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "preserve_file_permissions" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "force_input_charset" );
			xout.writeAttribute( "activated", "no");
			xout.writeEmptyElement( "do_not_cache_inodes" );
			xout.writeAttribute( "activated", "yes");
			xout.writeStartElement( "input_charset" );
			xout.writeCharacters( "iso8859-1" );
			xout.writeEndElement();
			xout.writeStartElement( "whitespace_treatment" );
			xout.writeCharacters( "noChange" );
			xout.writeEndElement();
			xout.writeStartElement( "whitespace_replace_string" );
			xout.writeCharacters( "_" );
			xout.writeEndElement();
			xout.writeStartElement( "data_track_mode" );
			xout.writeCharacters( "auto" );
			xout.writeEndElement();
			xout.writeStartElement( "multisession" );
			xout.writeCharacters( "auto" );
			xout.writeEndElement();
			xout.writeEmptyElement( "verify_data" );
			xout.writeAttribute( "activated", "yes");
			xout.writeEndElement();

			xout.writeStartElement( "header" );
			xout.writeStartElement( "volume_id" );
			xout.writeCharacters( "PicKing" );
			xout.writeEndElement();
			xout.writeStartElement( "volume_set_id" );
			xout.writeEndElement();
			xout.writeStartElement( "volume_set_size" );
			xout.writeCharacters( "1" );
			xout.writeEndElement();
			xout.writeStartElement( "volume_set_number" );
			xout.writeCharacters( "1" );
			xout.writeEndElement();
			xout.writeStartElement( "system_id" );
			xout.writeCharacters( "LINUX" );
			xout.writeEndElement();
			xout.writeStartElement( "application_id" );
			xout.writeCharacters( "K3B THE CD KREATOR (C) 1998-2006 SEBASTIAN TRUEG AND THE K3B TEAM" );
			xout.writeEndElement();
			xout.writeStartElement( "publisher" );
			xout.writeEndElement();
			xout.writeStartElement( "preparer" );
			xout.writeEndElement();
			xout.writeEndElement();
			
			xout.writeStartElement( "files" );
			for( Enumeration< ? > e = node.children(); e.hasMoreElements(); ) {
				DefaultMutableTreeNode child = (DefaultMutableTreeNode)e.nextElement();
				writeK3BFilesNode( xout, (ShortFile)child.getUserObject() );
			}
			xout.writeEndElement();
			
			xout.writeEndElement();
			xout.writeEndDocument();
			xout.flush();
			xout.close();
		} catch (XMLStreamException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void writeK3BFilesNode( XMLStreamWriter xout, ShortFile file ) throws XMLStreamException {
		if( file.isDirectory() ) {
			xout.writeStartElement( "directory" );
			xout.writeAttribute( "name", file.toString() );
			for( ShortFile child : file.listFiles() ) {
				writeK3BFilesNode( xout, child );
			}
			xout.writeEndElement();
		} else {
			xout.writeStartElement( "file" );
			xout.writeAttribute( "name", file.toString() );
			xout.writeStartElement( "url" );
			xout.writeCharacters( file.getAbsolutePath() );
			xout.writeEndElement();
			xout.writeEndElement();
		}
	}

}
