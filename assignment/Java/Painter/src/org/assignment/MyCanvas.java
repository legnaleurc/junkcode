package org.assignment;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Point;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.io.Serializable;
import java.util.Vector;

import javax.imageio.ImageIO;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

public class MyCanvas extends JPanel {

	private static final long serialVersionUID = -3768985931346091426L;
	public static final int PEN = 1, TEXT = 2, RECTANGLE = 3;
	
	public MyCanvas() {
		this.setBackground(Color.WHITE);
		this.addMouseListener(new MouseAdapter() {
			public void mousePressed( MouseEvent e ) {
				startPoint = e.getPoint();
				currentPoint = startPoint;
			}
			
			public void mouseClicked( MouseEvent e ) {
				tmpText = JOptionPane.showInputDialog(null, "Enter your text:", "Text input", JOptionPane.QUESTION_MESSAGE);
				if( tmpText != null && !tmpText.isEmpty() ) {
					texts.add(new Coordinate( tmpText, e.getPoint() ));
					repaint();
				}
			}
			
			public void mouseReleased( MouseEvent e ) {
				switch( mode ) {
				case PEN:
					pens.add(tmpTrace);
					tmpTrace = new Vector<Coordinate>();
					break;
				case RECTANGLE:
					if( startPoint.x <= currentPoint.x ) {
						if( startPoint.y <= currentPoint.y ) {
							rectangles.add( new Coordinate( startPoint, currentPoint ) );
						} else {
							rectangles.add( new Coordinate( new Point( startPoint.x, currentPoint.y ), new Point( currentPoint.x, startPoint.y ) ) );
						}
					} else {
						if( startPoint.y <= currentPoint.y ) {
							rectangles.add( new Coordinate( new Point( currentPoint.x, startPoint.y ), new Point( startPoint.x, currentPoint.y ) ) );
						} else {
							rectangles.add( new Coordinate( currentPoint, startPoint ) );
						}
					}
					break;
				}
			}
		});
		
		this.addMouseMotionListener(new MouseAdapter() {
			public void mouseDragged( MouseEvent e ) {
				if( mode == PEN ) {
					tmpTrace.add(new Coordinate(currentPoint, e.getPoint()));
				}
				currentPoint = e.getPoint();
				repaint();
			}
		});
	}
	
	public void saveAs( File file ) {
		try {
			BufferedImage image = new BufferedImage(this.getSize().width, this.getSize().height, BufferedImage.TYPE_BYTE_GRAY );
			
			Graphics g = image.createGraphics();
			g.setColor(Color.WHITE);
			g.fillRect(0, 0, image.getWidth(), image.getHeight());
			this.redrawObjects(g);
			g.dispose();
			
			ImageIO.write(image, "jpg", file);
		} catch( IOException e ) {
			System.exit(1);
		}
	}
	
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		this.redrawObjects(g);
		switch( this.mode ) {
		case PEN:
			for( Coordinate c : this.tmpTrace ) {
				g.drawLine(c.getBegin().x, c.getBegin().y, c.getEnd().x, c.getEnd().y);				
			}
			break;
		case RECTANGLE:
			if( startPoint.x <= currentPoint.x ) {
				if( startPoint.y <= currentPoint.y ) {
					g.drawRect(startPoint.x, startPoint.y, currentPoint.x - startPoint.x, currentPoint.y - startPoint.y);
				} else {
					g.drawRect(startPoint.x, currentPoint.y, currentPoint.x - startPoint.x, startPoint.y - currentPoint.y);
				}
			} else {
				if( startPoint.y <= currentPoint.y ) {
					g.drawRect(currentPoint.x, startPoint.y, startPoint.x - currentPoint.x, currentPoint.y - startPoint.y);
				} else {
					g.drawRect(currentPoint.x, currentPoint.y, startPoint.x - currentPoint.x, startPoint.y - currentPoint.y);
				}
			}
			break;
		}
	}
	
	private void redrawObjects(Graphics g) {
		g.setColor(Color.BLACK);
		for( Vector<Coordinate> trace : pens ) {
			for( Coordinate c : trace ) {
				g.drawLine(c.getBegin().x, c.getBegin().y, c.getEnd().x, c.getEnd().y);
			}
		}
		
		for( Coordinate c : texts ) {
			g.drawString(c.getText(), c.getBegin().x, c.getBegin().y);
		}
		
		for( Coordinate c : rectangles ) {
			g.drawRect(c.getBegin().x, c.getBegin().y, c.getDiff().width, c.getDiff().height);
		}
	}
	
	public Dimension getPreferredSize() {
		return new Dimension( 640, 480 );
	}
	
	public void setMode( int mode ) {
		this.mode = mode;
	}
	
	private Point startPoint = new Point();
	private Point currentPoint = new Point();
	private Vector<Coordinate> tmpTrace = new Vector<Coordinate>();
	private String tmpText;
	private Vector<Vector<Coordinate>> pens = new Vector<Vector<Coordinate>>();
	private Vector<Coordinate> texts = new Vector<Coordinate>();
	private Vector<Coordinate> rectangles = new Vector<Coordinate>();
	private int mode = 0;
	
	private class Coordinate implements Serializable {
		private static final long serialVersionUID = -571463647716040800L;

		public Coordinate( Point begin, Point end ) {
			this.setBegin(begin);
			this.setEnd(end);
			this.diff = new Dimension( end.x - begin.x, end.y - begin.y );
		}
		
		public Coordinate( String text, Point begin ) {
			this.setBegin(begin);
			this.setEnd(begin);
			this.setText(text);
		}
		
		public void setBegin(Point begin) {
			this.begin = begin;
		}

		public Point getBegin() {
			return begin;
		}

		public void setEnd(Point end) {
			this.end = end;
		}

		public Point getEnd() {
			return end;
		}
		
		public Dimension getDiff() {
			return diff;
		}

		public void setText(String text) {
			this.text = text;
		}

		public String getText() {
			return text;
		}

		private Point begin, end;
		private Dimension diff;
		private String text;
	}

}
