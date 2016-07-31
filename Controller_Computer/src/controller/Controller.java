package controller;

/**
 * 
 * @author Andrew_2
 */
public interface Controller {
    
    public Trackpad getTrack1();
    public Trackpad getTrack2();
    public boolean getButton(int button);
    
    public int getHardwareVersion();
    public int getSoftwareVersion();
    public int getNumButtons();
    
}
