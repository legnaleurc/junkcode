package org.assignment;

import java.awt.Container;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Vector;

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
					try {
						mw.listen( uri.port );
						mw.connect(new URI( "localhost", uri.port ));
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				} else {
					try {
						mw.connect(uri);
					} catch (UnknownHostException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
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
		PrintWriter sout = null;
		try {
			sout = new PrintWriter( this.client_.getOutputStream() );
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		sout.println(this.inputArea_.getText()+"\n");
		this.inputArea_.setText("");
	}
	
	public void appendMessage( String msg ) {
		this.outputArea_.append(msg);
		for( Socket client : this.clients_ ) {
			synchronized(client) {
				try {
					PrintWriter sout = new PrintWriter( client.getOutputStream() );
					sout.println(msg);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	public void listen( int port ) throws IOException {
		if(this.server_ != null) {
			this.server_.close();
		}
		this.server_ = new ServerSocket( port );
		
		while(true) {
			Socket client = this.server_.accept();
			this.clients_.add(client);
			
			ClientReader person = new ClientReader(client, this);
			person.start();
		}
	}
	
	public void connect(URI uri) throws UnknownHostException, IOException {
		if(this.client_ != null) {
			this.client_.close();
		}
		this.client_ = new Socket(uri.host, uri.port);
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
	private ServerSocket server_;
	private Vector<Socket> clients_;
	private Socket client_;

}
