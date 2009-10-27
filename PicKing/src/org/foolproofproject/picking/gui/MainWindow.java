package org.foolproofproject.picking.gui;

import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.KeyStroke;
import javax.swing.filechooser.FileNameExtensionFilter;

import org.foolproofproject.picking.Performer;
import org.foolproofproject.picking.ShortFile;
import org.foolproofproject.picking.UnitUtility;

public class MainWindow extends JFrame {
	
	private static final long serialVersionUID = 6869079478547863579L;
	private FileList list;
	private NeturalField size;
	private ResultTree result;
	private JComboBox unit;
	private JDialog about;
	private Preference preference;
	private DirectoryTree tree;
	private JCheckBox hidden;
	
	public MainWindow( String title ) {
		super( title );
		
		setDefaultCloseOperation( EXIT_ON_CLOSE );
		setSize( 800, 600 );
		setLocationRelativeTo( null );
		
		Container pane = getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		initViewPort();
		initControlPanel();
		initPreference();
		initAbout();
		
		initMenuBar();
		
		this.addWindowListener( new WindowAdapter() {
			public void windowClosing( WindowEvent e ) {
				try {
					Configuration.sync();
				} catch (Exception e1) {
					JOptionPane.showMessageDialog( e.getComponent(), e1.getMessage(), "Error on saving configuration!", JOptionPane.ERROR_MESSAGE );
				}
			}
		} );
	}
	
	private void initPreference() {
		preference = new Preference( this );
	}
	
	private void initAbout() {
		about = new JDialog( this );
		about.setTitle( "About PacKing" );
		about.setSize( 320, 240 );
		about.setLocationRelativeTo( this );
		
		Container pane = about.getContentPane();
		pane.setLayout( new BoxLayout( pane, BoxLayout.Y_AXIS ) );
		
		JPanel center = new JPanel();
		pane.add( center );
		center.setLayout( new BoxLayout( center, BoxLayout.Y_AXIS ) );
		center.setMaximumSize( new Dimension( Integer.MAX_VALUE, Integer.MAX_VALUE ) );

		center.add( new JLabel( "Version: 0.2.0" ) );
		center.add( new JLabel( "Author: legnaleurc (FoolproofProject)" ) );
		center.add( new JLabel( "License: LGPLv3" ) );
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
		
		JMenuItem save_ = new JMenuItem( "Save Result" );
		file.add( save_ );
		save_.setMnemonic( KeyEvent.VK_S );
		save_.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_S, KeyEvent.CTRL_DOWN_MASK ) );
		save_.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				save();
			}
		} );
		
		JMenuItem export = new JMenuItem( "Export to K3B" );
		file.add( export );
		export.setMnemonic( KeyEvent.VK_E );
		export.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_E, KeyEvent.CTRL_DOWN_MASK ) );
		export.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				File file = FileDialog.getExistingDirectory( (Component)e.getSource() );
				if( file != null ) {
					result.exportK3BProjectsTo( file );
				}
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
				preference.exec( size.toLong(), unit.getSelectedIndex(), hidden.isSelected() );
			}
		} );
		
		JMenu help = new JMenu( "Help" );
		menuBar.add( help );
		help.setMnemonic( KeyEvent.VK_H );
		
		JMenuItem debugLog = new JMenuItem( "Debug Log" );
		help.add( debugLog );
		debugLog.setMnemonic( KeyEvent.VK_D );
		debugLog.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				LogDialog.getDebugLog().setVisible( true );
			}
		} );
		
		JMenuItem errorLog = new JMenuItem( "Error Log" );
		help.add( errorLog );
		errorLog.setMnemonic( KeyEvent.VK_E );
		errorLog.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				LogDialog.getErrorLog().setVisible( true );
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
		
		JPanel limitPanel = new JPanel();
		panel.add( limitPanel );
		limitPanel.setLayout( new GridLayout( 1, 2 ) );
		limitPanel.setBorder( BorderFactory.createTitledBorder( "Limit" ) );
		
		size = new NeturalField();
		limitPanel.add( size );
		
		unit = UnitUtility.createComboBox();
		limitPanel.add( unit );
		
		JPanel viewPanel = new JPanel();
		panel.add( viewPanel );
		viewPanel.setLayout( new GridLayout( 1, 1 ) );
		viewPanel.setBorder( BorderFactory.createTitledBorder( "View" ) );
		
		hidden = new JCheckBox( "Hidden" );
		viewPanel.add( hidden );
		hidden.addActionListener( new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				tree.setHidden( hidden.isSelected() );
				tree.refresh();
			}
		} );
		
		JButton start = new JButton( "Start" );
		panel.add( start );
		start.addMouseListener( new MouseAdapter() {
			public void mouseClicked( MouseEvent e ) {
				perform();
			}
		} );
		
		Container pane = getContentPane();
		pane.add( panel );
		
		read();
	}
	
	public void read() {
		size.setLong( (Long) Configuration.get( "limit" ) );
		unit.setSelectedIndex( (Integer) Configuration.get( "unit" ) );
		hidden.setSelected( (Boolean) Configuration.get( "hidden" ) );
	}
	
	public void save() {
		File file = FileDialog.getSaveFileName( this, new FileNameExtensionFilter( "Plain Text", "txt" ));
		if( file != null ) {
			try {
				PrintStream fout = new PrintStream( file, "UTF-8" );
				result.save( fout );
				fout.close();
			} catch (FileNotFoundException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
			} catch (UnsupportedEncodingException e) {
				LogDialog.getErrorLog().log( e.getMessage() );
			}
		}
	}
	
	public void perform() {
		result.clear();
		
		Performer p = new Performer( UnitUtility.extract( size.toLong(), unit.getSelectedIndex() ), list.getSelectedFiles() );
		
		result.setTable( p.getTable() );
		
		while( !p.noItem() ) {
			Performer.Result r = p.once();
			result.addResult( r.getSize(), unit.getSelectedIndex(), r.getItems() );
			p.remove( r.getItems() );
		}
		
		if( !p.noOverflow() ) {
			result.addOverflow( p.getOverflow() );
		}
		
		result.expandAll();
	}

}
