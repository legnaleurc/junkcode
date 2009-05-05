package org.assignment;

import javax.swing.BoxLayout;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JTextField;

public class ModeDialog extends JDialog {

	private static final long serialVersionUID = -684903256850725954L;
	
	public ModeDialog() {
		super((java.awt.Frame)null, "Select connection mode", true);
		this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
		
		JRadioButton serverMode = new JRadioButton("Server listening port:");
		JRadioButton clientMode = new JRadioButton("Client connect to:");
		
		ButtonGroup choise = new ButtonGroup();
		choise.add(serverMode);
		choise.add(clientMode);
		
		JTextField serverField = new JTextField();
		JTextField clientField = new JTextField();
		
		JButton ok = new JButton();
		JButton cancel = new JButton();
		
		// layout
		this.setLayout(new BoxLayout(this.getContentPane(), BoxLayout.Y_AXIS));
		
		JPanel tmp = null;
		
		tmp = new JPanel();
		tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
		tmp.add(serverMode);
		tmp.add(serverField);
		this.add(tmp);
		
		tmp = new JPanel();
		tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
		tmp.add(clientMode);
		tmp.add(clientField);
		this.add(tmp);
		
		tmp = new JPanel();
		tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
		this.add(tmp);
	}

}
