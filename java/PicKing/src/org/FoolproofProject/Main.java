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
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.util.Collections;
import java.util.Hashtable;
import java.util.Vector;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import org.FoolproofProject.Configuration.Record;
import org.FoolproofProject.Travaler.Result;

public class Main extends JFrame {
	
	private class ItemTable {
		
		private final Long limit;
		private Long size;
		private Hashtable< File, Long > table, all;
		private Vector< File > overflow;
		
		public ItemTable( Long limit ) {
			this.limit = limit;
			size = 0L;
			table = new Hashtable< File, Long >();
			all = new Hashtable< File, Long >();
			overflow = new Vector< File >();
		}
		
		public Long getLimit() {
			return limit;
		}
		public Long getSize() {
			return size;
		}
		public Hashtable< File, Long > getTable() {
			return table;
		}
		public Hashtable< File, Long > getAll() {
			return all;
		}
		public void remove( File key ) {
			size -= table.get( key );
			table.remove( key );
		}
		public void put( File key, Long value ) {
			if( value < limit ) {
				size += value;
				table.put( key, value );
			} else {
				overflow.add( key );
			}
			all.put( key, value );
		}
		public boolean isEmpty() {
			return table.isEmpty();
		}
		public boolean noOverflow() {
			return overflow.isEmpty();
		}
		public Vector< File > getKeys() {
			return new Vector< File >( table.keySet() );
		}
		public Vector< File > getOverflow() {
			return overflow;
		}
	}
	
	private static final long serialVersionUID = 6869079478547863579L;
	private FileList list;
	private NeturalField size;
	private ResultTree result;
	private JComboBox unit;
	private JDialog about;
	private Preference preference;
	private DirectoryTree tree;
	
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
		center.add( new JLabel( "Author: legnaleurc (FoolproofProject)" ) );
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
		
		JMenu file = new JMenu( "File" );
		menuBar.add( file );
		file.setMnemonic( KeyEvent.VK_F );
		
		JMenuItem save_ = new JMenuItem( "Save" );
		file.add( save_ );
		save_.setMnemonic( KeyEvent.VK_S );
		save_.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_S, KeyEvent.CTRL_DOWN_MASK ) );
		save_.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				save();
			}
		} );
		
		JMenu edit = new JMenu( "Edit" );
		menuBar.add( edit );
		edit.setMnemonic( KeyEvent.VK_E );
		
		JMenuItem refresh = new JMenuItem( "Refresh" );
		edit.add( refresh );
		refresh.setMnemonic( KeyEvent.VK_R );
		refresh.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_F5, 0 ) );
		refresh.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				tree.refresh();
			}
		} );
		
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
		
		tree = new DirectoryTree();
		central.add( tree );
		
		list = new FileList( tree );
		central.add( list );
		
		result = new ResultTree();
		central.add( result );
		
		Container pane = getContentPane();
		pane.add( central );
		
		tree.open( new ShortFile( System.getProperty( "user.home" ) ) );
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
				ItemTable table = new ItemTable( size.toLong() * eng );
				for( File item : items ) {
					table.put( item, Travaler.getSize( item ) );
				}
				
				result.setTable( table.getAll() );
				
				while( !table.isEmpty() ) {
					Result pair = ( table.getSize() < table.getLimit() ) ? new Result( table.getSize(), table.getKeys() ) : Travaler.pick( table.getLimit(), table.getTable() );
					Long title = pair.size / eng;
					Collections.sort( pair.items );
					result.addResult( title + " " + unit.getSelectedItem(), pair.items );
					for( File file : pair.items ) {
						table.remove( file );
					}
				}
				
				if( !table.noOverflow() ) {
					result.addResult( "Overflow", table.getOverflow() );
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
	
	public void save() {
		JFileChooser fc = new JFileChooser();
		switch( fc.showSaveDialog( this ) ) {
		case JFileChooser.APPROVE_OPTION:
			try {
				PrintStream fout = new PrintStream( fc.getSelectedFile(), "UTF-8" );
				result.save( fout );
				fout.close();
			} catch (FileNotFoundException e) {
				ErrorLog.getInstance().log( e.getMessage() );
			} catch (UnsupportedEncodingException e) {
				ErrorLog.getInstance().log( e.getMessage() );
			}
			break;
		case JFileChooser.CANCEL_OPTION:
			break;
		case JFileChooser.ERROR_OPTION:
			break;
		default:
			;
		}
	}

	public static void main(String[] args) {
		SwingUtilities.invokeLater( new Runnable() {
			public void run() {
				try {
					UIManager.setLookAndFeel( UIManager.getSystemLookAndFeelClassName() );
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
