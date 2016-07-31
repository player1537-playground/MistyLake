
package events;

import io.ControllerReader;

/**
 *
 * @author Andrew_2
 */
public interface ControllerReadListener {
    
    public void onRead(ControllerReader reader, int id);
    
}
