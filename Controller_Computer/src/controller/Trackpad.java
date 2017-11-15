
package controller;

/**
 *
 * @author Andrew_2
 */
public class Trackpad {
    
    private int numFingers;
    private boolean[] active;
    private int[] x, y, strengths;
    
    public Trackpad(int numFingers) {
        this.numFingers = numFingers;
        active = new boolean[numFingers];
        x = new int[numFingers];
        y = new int[numFingers];
        strengths = new int[numFingers];
    }
    
    public void setActive(int finger, boolean pactive) {
        active[finger] = pactive;
    }
    
    public void set(int finger, int px, int py, int pstrength) {
        x[finger] = px;
        y[finger] = py;
        strengths[finger] = pstrength;
    }
    public void set(int finger, int[] pdata) {
        x[finger] = pdata[0];
        y[finger] = pdata[1];
        strengths[finger] = pdata[2];
    }
    
    public void setX(int finger, int px) {
        x[finger] = px;
    }
    public void setY(int finger, int py) {
        y[finger] = py;
    }
    public void setStrength(int finger, int pstrength) {
        strengths[finger] = pstrength;
    }
    
    
    public int getX(int finger) {
        return x[finger];
    }
    
    public int getY(int finger) {
        return y[finger];
    }
    
    public int getStrength(int finger) {
        return strengths[finger];
    }
    
    public int[] get(int finger) {
        return new int[] {x[finger], y[finger], strengths[finger]};
    }
    
    public int getNumFingers() {
        return numFingers;
    }
    
    public boolean getActive(int finger) {
        return active[finger];
    }
    
    public void copy(Trackpad loc) {
        for(int i = 0; i < numFingers; i++) {
            loc.set(i, get(i));
            loc.setActive(i, getActive(i));
        }
    }
}
