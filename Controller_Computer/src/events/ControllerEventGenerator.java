
package events;

import io.ControllerOverMessenger;
import controller.Trackpad;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author Andrew_2
 */
public class ControllerEventGenerator implements ControllerReadListener {
    
    ControllerOverMessenger reader;
    
    List<ControllerSpecListener> specListeners;
    List<List<ControllerTrackListener>> trackListeners;
    List<List<ControllerButtonListener>> buttonListeners;
    
    public static final short tapThresh = 100;
    private static final int numFingers = 1;
    
    private Trackpad t1, t2;
    private Trackpad oldT1, oldT2;
    private boolean track1Pressed, track2Pressed;
    private boolean[] oldButtons;
    
    public ControllerEventGenerator(ControllerOverMessenger reader) {
        this.reader = reader;
        specListeners = new ArrayList<>();
        
        trackListeners = new ArrayList<>();
        trackListeners.add(new ArrayList<>());
        trackListeners.add(new ArrayList<>());
        
        buttonListeners = new ArrayList<>();
        for(int i = 0; i < ControllerOverMessenger.maxButtons; i++) {
            buttonListeners.add(new ArrayList<>());
        }
        
        t1 = reader.getTrack1();
        t2 = reader.getTrack2();
        
        oldT1 = new Trackpad(numFingers);
        oldT2 = new Trackpad(numFingers);
        oldButtons = new boolean[ControllerOverMessenger.maxButtons];
    }
    
    public void addSpecListener(ControllerSpecListener listener) {
        specListeners.add(listener);
    }
    public void addTrackListener(ControllerTrackListener listener, int trackpad) {
        trackListeners.get(trackpad).add(listener);
    }
    public void addButtonListener(ControllerButtonListener listener, int button) {
        buttonListeners.get(button).add(listener);
    }

    @Override
    public void onRead(ControllerOverMessenger reader, int id) {
        if(id == ControllerOverMessenger.SPEC_ID) {
            for(ControllerSpecListener listener : specListeners) {
                listener.onSpecRead(reader.getHardwareVersion(),
                        reader.getSoftwareVersion(), reader.getButtonsUsed());
            }
        } else if (id == ControllerOverMessenger.DATA_ID) {
            
            boolean[] buttons = reader.getButtons();
            for(int i = 0; i < oldButtons.length; i++) {
                if(buttons[i] && !oldButtons[i]) {
                    for(ControllerButtonListener listener : buttonListeners.get(i)) {
                        listener.onPress();
                    }
                } else if(!buttons[i] && oldButtons[i]) {
                    for(ControllerButtonListener listener : buttonListeners.get(i)) {
                        listener.onRelease();
                    }
                }
                
                oldButtons[i] = buttons[i];
            }
   
            int dx1 = t1.getX(0) - oldT1.getX(0),
                dy1 = t1.getY(0) - oldT1.getY(0);
            int dx2 = t2.getX(0) - oldT2.getX(0),
                dy2 = t2.getY(0) - oldT2.getY(0);
            
            if(dx1 != 0 || dy1 != 0) {
                for(ControllerTrackListener listener : trackListeners.get(0)) {
                    listener.onMove(t1.getX(0), t1.getY(0), dx1, dy1);
                }
            }
            if(dx2 != 0 || dy2 != 0) {
                for(ControllerTrackListener listener : trackListeners.get(1)) {
                    listener.onMove(t2.getX(0), t2.getY(0), dx2, dy2);
                }
            }
            
            if(t1.getStrength(0) < tapThresh && track1Pressed) {
                for(ControllerTrackListener listener : trackListeners.get(0)) {
                    listener.onRelease(t1.getX(0), t1.getY(0));
                }
                track1Pressed = false;
            }
            if(t1.getStrength(0) > tapThresh && !track1Pressed) {
                for(ControllerTrackListener listener : trackListeners.get(0)) {
                    listener.onPress(t1.getX(0), t1.getY(0));
                }
                track1Pressed = true;
            }
            
            if(t2.getStrength(0) < tapThresh && track2Pressed) {
                for(ControllerTrackListener listener : trackListeners.get(1)) {
                    listener.onRelease(t2.getX(0), t2.getY(0));
                }
                track2Pressed = false;
            }
            if(t2.getStrength(0) > tapThresh && !track2Pressed) {
                for(ControllerTrackListener listener : trackListeners.get(1)) {
                    listener.onPress(t2.getX(0), t2.getY(0));
                }
                track2Pressed = true;
            }
            
            t1.copy(oldT1);
            t2.copy(oldT2);
        }
    }
    
}
