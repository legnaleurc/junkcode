package org.FoolproofProject;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class Main extends JFrame {
	
	private static final long serialVersionUID = 6869079478547863579L;
	private FileList list;
	private NeturalField size;
	private ResultTree result;
	private JComboBox unit;
	
	public Main( String title ) {
		super( title );
		
		setDefaultCloseOperation( EXIT_ON_CLOSE );
		
		Container pane = getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		initViewPort();
		initControlPanel();
	}

	private void initViewPort() {
		JPanel central = new JPanel();
		central.setLayout( new GridLayout( 1, 3 ) );
		central.setMaximumSize( new Dimension( Integer.MAX_VALUE, Integer.MAX_VALUE ) );
		
		DirectoryTree tree = new DirectoryTree();
		central.add( tree );
		
		list = new FileList( tree );
		central.add( list );
		
		result = new ResultTree();
		central.add( result );
		
		Container pane = getContentPane();
		pane.add( central );
	}
	
	private void initControlPanel() {
		JPanel panel = new JPanel();
		panel.setLayout( new GridLayout( 1, 3 ) );
		
		size = new NeturalField( 4 );
		panel.add( size );
		
		unit = new JComboBox();
		panel.add( unit );
		unit.addItem( "B" );
		unit.addItem( "KB" );
		unit.addItem( "MB" );
		unit.addItem( "GB" );
		unit.setSelectedIndex( 2 );
		
		JButton start = new JButton( "Start" );
		panel.add( start );
		start.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				result.clear();
				
				Vector< File > items = list.getSelectedFiles();
				long eng = ( long )Math.pow( 1024, unit.getSelectedIndex() );
				Long limit = size.toLong() * eng;
				Vector< File > overflow = new Vector< File >();
				Hashtable< File, Long > table = new Hashtable< File, Long >();
				Hashtable< File, Long > total = new Hashtable< File, Long >();
				for( File item : items ) {
					Long fileSize = Travaler.getSize( item );
					if( fileSize > limit ) {
						overflow.add( item );
					} else {
						table.put( item, fileSize );
					}
					total.put( item , fileSize );
				}
				
				result.setTable( total );
				
				while( !table.isEmpty() ) {
					Pair pair = Travaler.pick( limit, table );
					Long title = pair.size / eng;
					result.addResult( title + " " + unit.getSelectedItem(), pair.items );
					for( File file : pair.items ) {
						table.remove( file );
					}
				}
				
				if( !overflow.isEmpty() ) {
					result.addResult( "Overflow", overflow );
				}
				
				result.expandAll();
			}
		} );
		
		Container pane = getContentPane();
		pane.add( panel );
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				try {
					UIManager.setLookAndFeel(UIManager.getLookAndFeel());
				} catch( Exception e ) {
					System.err.println( e.getMessage() );
				}
				
				Main self = new Main( "PicKing" );
				self.setSize( 800, 600 );
				self.setVisible(true);
			}
		} );
	}

}
