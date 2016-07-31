
package test;

import controller.Controller;
import controller.Trackpad;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.util.Timer;
import java.util.TimerTask;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.BevelBorder;

/**
 *
 * @author Andrew_2
 */
public class ControllerDisplay extends JFrame {

    private static final int frameWidth = 640, frameHeight = 480;
    private static final int buttonWidth = 400, buttonHeight = 100;
    private static final int trackDispSize = 200;
    private static final int trackScale = 200;

    Timer repaint;

    public ControllerDisplay(Controller cont) {
        super();

        this.setLayout(new GridBagLayout());
        GridBagConstraints gc = new GridBagConstraints();
        gc.insets = new Insets(10, 20, 0, 20);

        gc.gridx = 0;
        gc.gridy = 0;
        this.add(new JLabel("Track 1"), gc);
        gc.gridy = 1;
        TrackpadDisplay td1 = new TrackpadDisplay(trackDispSize, trackDispSize, trackScale, trackScale, cont.getTrack1());
        this.add(td1, gc);

        gc.gridx = 1;
        gc.gridy = 0;
        this.add(new JLabel("Track 2"), gc);
        gc.gridy = 1;
        TrackpadDisplay td2 = new TrackpadDisplay(trackDispSize, trackDispSize, trackScale, trackScale, cont.getTrack2());
        this.add(td2, gc);

        gc.gridx = 0;
        gc.gridy = 2;
        gc.gridwidth = 2;
        ButtonDisplay bd = new ButtonDisplay(buttonWidth, buttonHeight, cont);
        this.add(bd, gc);

        repaint = new Timer("repaint");
        repaint.scheduleAtFixedRate(new TimerTask() {

            @Override
            public void run() {
                ControllerDisplay.this.invalidate();
                ControllerDisplay.this.repaint();
            }
        }, 100, 100);

        this.setSize(frameWidth, frameHeight);
        this.setVisible(true);
    }

    private static class ButtonDisplay extends JPanel {

        private int width, height;
        private Controller c;
        private Font f;
        
        private static final Color released = Color.red;
        private static final Color pressed = Color.green;
        private static final Color fontColor = Color.black;

        public ButtonDisplay(int width, int height, Controller c) {
            super();
            this.width = width;
            this.height = height;
            this.c = c;
            f = new Font("Arial", Font.BOLD, 12);

            this.setPreferredSize(new Dimension(width, height));
            setBorder(new BevelBorder(BevelBorder.LOWERED, Color.lightGray, Color.gray));

        }

        @Override
        public void paint(Graphics g) {
            super.paint(g);

            int numButtons = c.getNumButtons();
            float textSize = 16;
            f = f.deriveFont(textSize);
            g.setFont(f);
            if (numButtons > 0) {
                int diam = width / (c.getNumButtons()+2) / 2;
                int y = height / 2 - diam / 2;

                for (int i = 0; i < numButtons; i++) {
                    int x = (i+1) * width / (numButtons + 2);

                    g.setColor(c.getButton(i) ? pressed : released);
                    g.fillOval(x, y, diam, diam);
                    
                    g.setColor(fontColor);
                    g.drawString("" + (i + 1), x + diam / 2 - (int)textSize / 4, y - diam /2);
                }
            }
        }

    }

    private static class TrackpadDisplay extends JPanel {

        private int width, height;
        private float xscale, yscale;
        private Trackpad track;

        public TrackpadDisplay(int width, int height, float xscale, float yscale, Trackpad track) {
            super();
            this.width = width;
            this.height = height;
            this.xscale = xscale;
            this.yscale = yscale;
            this.track = track;
            this.setPreferredSize(new Dimension(width, height));
            setBorder(new BevelBorder(BevelBorder.LOWERED, Color.lightGray, Color.gray));
        }

        @Override
        public void paint(Graphics g) {
            super.paint(g);

            for (int i = 0; i < track.getNumFingers(); i++) {
                int diam = track.getStrength(i) / 5;
                int x = (int)(track.getX(i) * width  / xscale - diam / 2.0);
                int y = (int)(track.getY(i) * height / yscale - diam / 2.0);

                g.fillOval(x, y, diam, diam);
            }
        }

    }

}
