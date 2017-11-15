
package test;

import events.ControllerButtonListener;
import events.ControllerEventGenerator;
import io.ControllerOverMessenger;
import events.ControllerSpecListener;
import events.ControllerTrackListener;

/**
 *
 * @author Andrew_2
 */
public class ControllerEventEcho {

    ControllerEventGenerator events;
    
    public ControllerEventEcho(ControllerEventGenerator events) {
        
        this.events = events;
        
        try {

            events.addSpecListener(new ControllerSpecListener() {
                @Override
                public void onSpecRead(int hardwareVersion, int softwareVersion, int numButtons) {
                    System.out.println("Hardware " + hardwareVersion);
                    System.out.println("Software " + softwareVersion);
                    System.out.println("Number of Buttons " + numButtons);
                }
            });

            for (int i = 0; i < ControllerOverMessenger.maxButtons; i++) {
                final int buttonNumber = i;
                events.addButtonListener(new ControllerButtonListener() {

                    @Override
                    public void onPress() {
                        System.out.println("Button " + buttonNumber + " pressed");
                    }

                    @Override
                    public void onRelease() {
                        System.out.println("Button " + buttonNumber + " released");
                    }
                }, buttonNumber);
            }

            events.addTrackListener(new ControllerTrackListener() {

                @Override
                public void onMove(int x, int y, int dx, int dy) {
                    System.out.println("Track 1 moved by " + dx + " " + dy);
                }

                @Override
                public void onPress(int x, int y) {
                    System.out.println("Track 1 pressed " + x + " " + y);
                }
                @Override
                public void onRelease(int x, int y) {
                    System.out.println("Track 1 released " + x + " " + y);
                }
            }, 0);

            events.addTrackListener(new ControllerTrackListener() {

                @Override
                public void onMove(int x, int y, int dx, int dy) {
                    System.out.println("Track 2 moved by" + dx + " " + dy);
                }

                @Override
                public void onPress(int x, int y) {
                    System.out.println("Track 2 pressed " + x + " " + y);
                }
                @Override
                public void onRelease(int x, int y) {
                    System.out.println("Track 2 released " + x + " " + y);
                }
                
            }, 1);
            
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
}
