package org.assignment;

import java.awt.Container;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.IOException;
import javax.swing.BoxLayout;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
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
				mw.setVisible(true);
				
				if( uri.host == null ) {
					try {
						mw.listen( uri.port );
					} catch (IOException e) {
						JOptionPane.showMessageDialog(mw, "Can not open server.");
						mw.setVisible(false);
						return;
					}
					uri.host = "localhost";
				}
				try {
					mw.connect(uri);
				} catch (IOException e) {
					JOptionPane.showMessageDialog(mw, "Can not connect to: " + uri.host + ":" + uri.port);
				}
			}
		});
	}
	
	public MainWindow( String title ) {
		super(title);
		Container central = this.getContentPane();
		
		// components
		this.outputArea_ = new JTextArea(24, 80);
		this.outputArea_.setEditable(false);
		JScrollPane scroll = new JScrollPane(this.outputArea_);
		scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
		scroll.setAutoscrolls(true);
		this.outputArea_.setAutoscrolls(true);
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
		this.pack();
	}
	
	public void sendMessage() {
		synchronized(this.client_) {
			this.client_.send(this.inputArea_.getText());
		}
		this.inputArea_.setText("");
	}
	
	public void appendMessage( String msg ) {
		this.outputArea_.append(msg+"\n");
	}
	
	public void listen( int port ) throws IOException {
		this.server_ = new Server( port );
		this.server_.listen();
	}
	
	public void connect(URI uri) throws IOException {
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
	
	private void onDisconnect_() {
		JOptionPane.showMessageDialog(this, "The server has been disconnceted. Click Ok to exit.");
		this.setVisible(false);
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
			try {
				while( ( line = this.client_.receive() ) != null ) {
					synchronized( this.parent_ ) {
						this.parent_.appendMessage(line);
					}
				}
			} catch (IOException e) {
				this.parent_.onDisconnect_();
			}
		}
		
	}

}
