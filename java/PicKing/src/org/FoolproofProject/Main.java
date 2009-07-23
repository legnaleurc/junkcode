package org.FoolproofProject;

import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import org.FoolproofProject.Configuration.Record;
import org.FoolproofProject.Travaler.Result;

public class Main extends JFrame {
	
	private static final long serialVersionUID = 6869079478547863579L;
	private FileList list;
	private NeturalField size;
	private ResultTree result;
	private JComboBox unit;
	private JDialog about;
	private Preference preference;
	
	public Main( String title ) {
		super( title );
		
		setDefaultCloseOperation( EXIT_ON_CLOSE );
		
		Container pane = getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		initViewPort();
		initControlPanel();
		initPreference();
		initAbout();
		
		initMenuBar();
	}
	
	private void initPreference() {
		preference = new Preference( this );
	}
	
	private void initAbout() {
		about = new JDialog( this );
		about.setTitle( "About PacKing" );
		about.setSize( 320, 240 );
		
		Container pane = about.getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		JPanel center = new JPanel();
		pane.add( center );
		center.setLayout( new BoxLayout( center, BoxLayout.Y_AXIS ) );
		center.setMaximumSize( new Dimension( Integer.MAX_VALUE, Integer.MAX_VALUE ) );

		center.add( new JLabel( "Version: 0.1.0" ) );
		center.add( new JLabel( "Author: legnaleurc" ) );
		center.add( new JLabel( "License: GPLv3" ) );
		center.add( new JLabel( "e-mail: legnaleurc@gmail.com" ) );
		center.add( new JLabel( "blog: http://legnaleurc.blogspot.com/" ) );
		
		JPanel bottom = new JPanel();
		pane.add( bottom );
		bottom.setLayout( new GridLayout( 1, 1 ) );
		
		JButton ok = new JButton( "OK" );
		bottom.add( ok );
		ok.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				about.setVisible( false );
			}
		} );
	}
	
	private void initMenuBar() {
		JMenuBar menuBar = new JMenuBar();
		setJMenuBar( menuBar );
		
		JMenu edit = new JMenu( "Edit" );
		menuBar.add( edit );
		edit.setMnemonic( KeyEvent.VK_E );
		
		JMenuItem preferences = new JMenuItem( "Preferences" );
		edit.add( preferences );
		preferences.setMnemonic( KeyEvent.VK_P );
		preferences.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				preference.exec( size.toLong(), unit.getSelectedIndex() );
			}
		} );
		
		JMenu help = new JMenu( "Help" );
		menuBar.add( help );
		help.setMnemonic( KeyEvent.VK_H );
		
		JMenuItem errorLog = new JMenuItem( "Error Log" );
		help.add( errorLog );
		errorLog.setMnemonic( KeyEvent.VK_E );
		errorLog.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				ErrorLog.getInstance().setVisible( true );
			}
		} );
		
		JMenuItem about_ = new JMenuItem( "About ..." );
		help.add( about_ );
		about_.setMnemonic( KeyEvent.VK_A );
		about_.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				about.setVisible( true );
			}
		} );
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
		
		size = new NeturalField();
		panel.add( size );
		
		unit = new JComboBox();
		panel.add( unit );
		unit.addItem( "B" );
		unit.addItem( "KB" );
		unit.addItem( "MB" );
		unit.addItem( "GB" );
		
		read();
		
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
				Long totalSize = 0L;
				for( File item : items ) {
					Long fileSize = Travaler.getSize( item );
					totalSize += fileSize;
					if( fileSize > limit ) {
						overflow.add( item );
					} else {
						table.put( item, fileSize );
					}
					total.put( item , fileSize );
				}
				
				result.setTable( total );
				
				if( totalSize < limit ) {
					Long title = totalSize / eng;
					result.addResult( title + " " + unit.getSelectedItem(), items );
				} else {
					while( !table.isEmpty() ) {
						Result pair = Travaler.pick( limit, table );
						Long title = pair.size / eng;
						result.addResult( title + " " + unit.getSelectedItem(), pair.items );
						for( File file : pair.items ) {
							table.remove( file );
						}
					}
					
					if( !overflow.isEmpty() ) {
						result.addResult( "Overflow", overflow );
					}
				}
				
				result.expandAll();
			}
		} );
		
		Container pane = getContentPane();
		pane.add( panel );
	}
	
	public void read() {
		Record r = Configuration.load();
		size.setLong( r.limit );
		unit.setSelectedIndex( r.unit );
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
