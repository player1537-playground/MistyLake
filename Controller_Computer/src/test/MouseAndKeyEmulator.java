
package test;

import io.ControllerEmulator;
import controller.Trackpad;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.BevelBorder;
import message.Messenger;

/**
 *
 * @author Andrew_2
 */
public class MouseAndKeyEmulator extends ControllerEmulator {

    public static final int hardwareVersion = 144;
    public static final int softwareVersion = 351;
    public static final int numButtons = 8;

    public static final int trackSize = 200;

    public MouseAndKeyEmulator(Messenger messenger) {
        super(messenger, hardwareVersion, softwareVersion, numButtons);

        JFrame frame = new JFrame("Controller Emulator");
        frame.setLayout( new GridBagLayout());
        GridBagConstraints gc = new GridBagConstraints();
        gc.insets = new Insets(10, 20, 0, 20);
        
        gc.gridx = 0;
        gc.gridy = 0;
        frame.add(new JLabel("Track 1"), gc);
        gc.gridy = 1;
        PanelTrackpad pt1 = new PanelTrackpad(trackSize, trackSize, getTrack1());
        frame.add(pt1, gc);
        
        gc.gridx = 1;
        gc.gridy = 0;
        frame.add(new JLabel("Track 2"), gc);
        gc.gridy = 1;
        PanelTrackpad pt2 = new PanelTrackpad(trackSize, trackSize, getTrack2());
        frame.add(pt2, gc);
        
        
        getTrack1().setStrength(0,(short)50);
        getTrack2().setStrength(0,(short)50);
        
        frame.addKeyListener(new KeyAdapter() {

            @Override
            public void keyPressed(KeyEvent e) {
                char key = e.getKeyChar();
                int number = key - '0';
                if (number >= 1 && number <= numButtons) {
                    setButton(number - 1, true);
                }
            }

            @Override
            public void keyReleased(KeyEvent e) {
                char key = e.getKeyChar();
                int number = key - '0';
                if (number >= 1 && number <= numButtons) {
                    setButton(number - 1, false);
                }
            }
        });

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                close();
                System.exit(0);
            }
        });

        frame.setSize(600, 340);
        frame.setVisible(true);

    }

    private static class PanelTrackpad extends JPanel {

        int width;
        int height;
        Trackpad track;

        public static final short pressStrength = 150;
        public static final short releaseStrength = 50;

        public PanelTrackpad(int width, int height, Trackpad track) {
            super();
            this.width = width;
            this.height = height;
            this.track = track;
            
            setBorder(new BevelBorder(BevelBorder.LOWERED, Color.lightGray, Color.gray));
            setPreferredSize(new Dimension(width, height));
            this.addMouseListener(new MouseAdapter() {
                @Override
                public void mousePressed(MouseEvent e) {
                    track.setStrength(0, pressStrength);
                }

                @Override
                public void mouseReleased(MouseEvent e) {
                    track.setStrength(0, releaseStrength);
                }
            });
            
            this.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseMoved(MouseEvent e) {
                track.setX(0, scaleX(e.getX()));
                track.setY(0, scaleY(e.getY()));
            }

            @Override
            public void mouseDragged(MouseEvent e) {
                track.setX(0, scaleX(e.getX()));
                track.setY(0, scaleY(e.getY()));
            }
            
            private short scaleX(int x) {
                return (short)(780 / 200 * x);
            }
            private short scaleY(int y) {
                return (short)(1030 / 200 * y);
            }

        });
        }

    }

}
