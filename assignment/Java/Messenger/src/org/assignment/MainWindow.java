package org.assignment;

import java.awt.Container;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;

import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

public class MainWindow extends JFrame {

	private static final long serialVersionUID = 8880002421871642870L;

	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				// set style
				String style = UIManager.getSystemLookAndFeelClassName();
				try {
					UIManager.setLookAndFeel(style);
				} catch( Exception e ) {
					System.err.println(e.getMessage());
				}
				
				ModeDialog md = new ModeDialog();
				
				// create main window
				MainWindow mw = new MainWindow("Messenger");
				mw.setDefaultCloseOperation(EXIT_ON_CLOSE);
				mw.setSize(640, 480);
				mw.setVisible(true);
			}
		});
	}
	
	/**
	 * @param title Window title
	 */
	public MainWindow( String title ) {
		super(title);
		Container central = this.getContentPane();
		
		// components
		this.outputArea_ = new JTextArea(24, 80);
		this.outputArea_.setEditable(false);
		JScrollPane scroll = new JScrollPane(this.outputArea_);
		scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		this.inputArea_ = new JTextField();
		
		// layout
		central.setLayout(new BoxLayout(central, BoxLayout.Y_AXIS));
		
		JPanel tmp;
		
		tmp = new JPanel();
		tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
		tmp.add(this.outputArea_);
		tmp.add(scroll);
		central.add(tmp);
		
		tmp = new JPanel();
		tmp.setLayout(new BoxLayout(tmp, BoxLayout.X_AXIS));
		tmp.add(this.inputArea_);
		central.add(tmp);
		
		this.setupAction_();
	}
	
	public void sendMessage() {
		this.outputArea_.append(this.inputArea_.getText()+"\n");
		this.inputArea_.setText("");
	}
	
	private void setupAction_() {
		this.inputArea_.addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent e) {
				if( e.getKeyCode() == KeyEvent.VK_ENTER ) {
					sendMessage();
				}
			}
		});
	}
	
	private JTextField inputArea_;
	private JTextArea outputArea_;

}
