
package events;

import io.ControllerOverMessenger;

/**
 *
 * @author Andrew_2
 */
public interface ControllerReadListener {
    
    public void onRead(ControllerOverMessenger reader, int id);
    
}
