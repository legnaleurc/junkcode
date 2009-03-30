package org.assignment;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JRadioButtonMenuItem;

public class MyPainter extends JFrame {
	
	private static final long serialVersionUID = 1321007968403635229L;

	public MyPainter( String title ) {
		super( title );
		
		central = new MyCanvas();
		this.setContentPane(central);
		
		this.initMenuBar();
		
		this.pack();
	}
	
	private void initMenuBar() {
		JMenuBar menuBar = new JMenuBar();
		
		JMenu file = new JMenu("File");
		menuBar.add(file);
		JMenuItem save = new JMenuItem( "Save" );
		save.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
				switch( fc.showSaveDialog(MyPainter.this) ) {
				case JFileChooser.APPROVE_OPTION:
					central.saveAs(fc.getSelectedFile());
					break;
				}
			}
		});
		file.add(save);
		
		JMenu mode = new JMenu("Mode");
		menuBar.add(mode);
		
		ButtonGroup group = new ButtonGroup();
		
		JRadioButtonMenuItem pen = new JRadioButtonMenuItem( "Pen" );
		pen.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				central.setMode(MyCanvas.PEN);
			}
		});
		pen.doClick();
		group.add(pen);
		mode.add(pen);
		
		JRadioButtonMenuItem text = new JRadioButtonMenuItem( "Text" );
		text.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				central.setMode(MyCanvas.TEXT);
			}
		});
		group.add(text);
		mode.add(text);
		
		JRadioButtonMenuItem rectangle = new JRadioButtonMenuItem( "Rectangle" );
		rectangle.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				central.setMode(MyCanvas.RECTANGLE);
			}
		});
		group.add(rectangle);
		mode.add(rectangle);
		
		this.setJMenuBar(menuBar);
	}
	
	private MyCanvas central;

}
