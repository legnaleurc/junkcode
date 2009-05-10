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

				URI uri = ModeDialog.getSelection();
				if( uri == null ) {
					return;
				}
				
				// create main window
				MainWindow mw = new MainWindow("Messenger");
				mw.setDefaultCloseOperation(EXIT_ON_CLOSE);
				mw.setSize(640, 480);
				mw.setVisible(true);
				
				if( uri.host == null ) {
					mw.listen( uri.port );
					mw.connect(new URI( "localhost", uri.port ));
				} else {
					mw.connect(uri);
				}
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
		scroll.setAutoscrolls(true);
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
		synchronized(this.client_) {
			this.client_.send(this.inputArea_.getText());
		}
		System.err.println("sended: " + this.inputArea_.getText());
		this.inputArea_.setText("");
	}
	
	public void appendMessage( String msg ) {
		this.outputArea_.append(msg);
	}
	
	public void listen( int port ) {
		this.server_ = new Server( port );
		this.server_.listen();
	}
	
	public void connect(URI uri) {
		this.client_ = new Client(uri.host, uri.port);
		Receiver r = new Receiver(this.client_, this);
		r.start();
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
	private Server server_;
	private Client client_;
	
	private class Receiver extends Thread {
		
		private Client client_;
		private MainWindow parent_;

		public Receiver( Client client, MainWindow parent ) {
			this.client_ = client;
			this.parent_ = parent;
		}
		
		public void run() {
			String line;
			while( ( line = this.client_.receive() ) != null ) {
				synchronized( this.parent_ ) {
					this.parent_.appendMessage(line);
				}
			}
		}
		
	}

}
