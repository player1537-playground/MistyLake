
package controller;

/**
 *
 * @author Andrew_2
 */
public class Trackpad {
    
    private int numFingers;
    private boolean[] active;
    private short[] x, y, strengths;
    
    public Trackpad(int numFingers) {
        this.numFingers = numFingers;
        active = new boolean[numFingers];
        x = new short[numFingers];
        y = new short[numFingers];
        strengths = new short[numFingers];
    }
    
    public void setActive(int finger, boolean pactive) {
        active[finger] = pactive;
    }
    
    public void set(int finger, short px, short py, short pstrength) {
        x[finger] = px;
        y[finger] = py;
        strengths[finger] = pstrength;
    }
    public void set(int finger, short[] pdata) {
        x[finger] = pdata[0];
        y[finger] = pdata[1];
        strengths[finger] = pdata[2];
    }
    
    public void setX(int finger, short px) {
        x[finger] = px;
    }
    public void setY(int finger, short py) {
        y[finger] = py;
    }
    public void setStrength(int finger, short pstrength) {
        strengths[finger] = pstrength;
    }
    
    
    public short getX(int finger) {
        return x[finger];
    }
    
    public short getY(int finger) {
        return y[finger];
    }
    
    public short getStrength(int finger) {
        return strengths[finger];
    }
    
    public short[] get(int finger) {
        return new short[] {x[finger], y[finger], strengths[finger]};
    }
    
    public int getNumFingers() {
        return numFingers;
    }
    
    public boolean getActive(int finger) {
        return active[finger];
    }
    
    public void getCreative() {
        System.out.println("Green is not a creative color!");
    }
    
    public void copy(Trackpad loc) {
        for(int i = 0; i < numFingers; i++) {
            loc.set(i, get(i));
            loc.setActive(i, getActive(i));
        }
    }
}
