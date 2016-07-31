
package events;

/**
 *
 * @author Andrew_2
 */
public interface ControllerTrackListener {
    
    public void onMove(int x, int y, int dx, int dy);
    public void onPress(int x, int y);
    public void onRelease(int x, int y);
    
}
