package org.FoolproofProject;

import java.awt.Container;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Hashtable;

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
		central.setLayout( new BoxLayout( central, BoxLayout.X_AXIS ) );
		
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
		panel.setLayout( new BoxLayout( panel, BoxLayout.X_AXIS ) );
		
		size = new NeturalField( 4 );
		panel.add( size );
		
		JComboBox unit = new JComboBox();
		panel.add( unit );
		unit.addItem( "B" );
		unit.addItem( "KB" );
		unit.addItem( "MB" );
		unit.addItem( "GB" );
		
		JButton start = new JButton( "Start" );
		panel.add( start );
		start.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				result.clear();
				
				Hashtable< String, Long > table = new Hashtable< String, Long >();
				for( File file : list.getSelectedFiles() ) {
					table.put( file.getName(), Travaler.getSize( file ) );
				}
				Pair pair = Travaler.pick( size.toLong(), table );
				result.addResult( pair );
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
				self.setSize( 640, 480 );
				self.setVisible(true);
			}
		} );
	}

}
