package org.FoolproofProject;

import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTree;

public class ResultTree extends JPanel {

	private static final long serialVersionUID = 3366458847085663811L;
	
	public ResultTree() {
		JTree view = new JTree();
		JScrollPane scroll = new JScrollPane( view );
		setLayout( new BoxLayout( this, BoxLayout.Y_AXIS ) );
		add( scroll );
	}

}
