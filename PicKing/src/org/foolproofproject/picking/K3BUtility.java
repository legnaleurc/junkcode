package org.foolproofproject.picking;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.swing.tree.DefaultMutableTreeNode;
import javax.xml.stream.XMLOutputFactory;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamWriter;

public class K3BUtility {

	private static XMLOutputFactory xmlFactory = XMLOutputFactory.newInstance();
	private DefaultMutableTreeNode node;
	private ZipOutputStream zout;
	private XMLStreamWriter xout;

	public static void export( File file, DefaultMutableTreeNode node ) throws IOException, XMLStreamException {
		K3BUtility k3b = new K3BUtility( node );
		
		k3b.zout = new ZipOutputStream( new BufferedOutputStream( new FileOutputStream( file ) ) );	
		
		k3b.zout.putNextEntry( new ZipEntry( "mimetype" ) );
		k3b.zout.write( "application/x-k3b".getBytes( "UTF-8" ) );
		k3b.zout.closeEntry();
		
		k3b.zout.putNextEntry( new ZipEntry( "maindata.xml" ) );
		k3b.writeXML();
		k3b.zout.closeEntry();
		
		k3b.zout.close();
	}
	
	private K3BUtility( DefaultMutableTreeNode node ) {
		this.node = node;
		this.zout = null;
		this.xout = null;
	}
	
	private void writeXML() throws UnsupportedEncodingException, XMLStreamException {
		xout = xmlFactory.createXMLStreamWriter( new OutputStreamWriter( zout, "UTF-8" ) );
		xout.writeStartDocument( "UTF-8", "1.0" );
		xout.writeDTD( "<!DOCTYPE k3b_dvd_project>" );
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
			writeK3BFilesNode( (ShortFile)child.getUserObject() );
		}
		xout.writeEndElement();
		
		xout.writeEndElement();
		xout.writeEndDocument();
		xout.flush();
		xout.close();
	}
	
	private void writeK3BFilesNode( ShortFile file ) throws XMLStreamException {
		if( file.isDirectory() ) {
			xout.writeStartElement( "directory" );
			xout.writeAttribute( "name", file.toString() );
			for( ShortFile child : file.listFiles() ) {
				writeK3BFilesNode( child );
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
