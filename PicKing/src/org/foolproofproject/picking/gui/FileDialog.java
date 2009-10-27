package org.foolproofproject.picking.gui;

import java.awt.Component;
import java.io.File;

import javax.swing.JFileChooser;
import javax.swing.filechooser.FileFilter;

public class FileDialog {
	
	public static File getExistingDirectory( Component parent ) {
		JFileChooser fc = new JFileChooser();
		fc.setFileFilter( new FileFilter() {
			@Override
			public boolean accept( File f ) {
				return f.isDirectory();
			}
			@Override
			public String getDescription() {
				return "Directories";
			}
		} );
		fc.setFileSelectionMode( JFileChooser.DIRECTORIES_ONLY );
		
		switch( fc.showOpenDialog( parent ) ) {
		case JFileChooser.APPROVE_OPTION:
			return fc.getSelectedFile();
		default:
			return null;
		}
	}
	
	public static File getSaveFileName( Component parent, FileFilter filter ) {
		JFileChooser fc = new JFileChooser();
		fc.setFileFilter( filter );
		
		switch( fc.showSaveDialog( parent ) ) {
		case JFileChooser.APPROVE_OPTION:
			return fc.getSelectedFile();
		default:
			return null;
		}
	}

}
