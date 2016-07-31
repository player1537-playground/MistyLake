
package events;

/**
 *
 * @author Andrew_2
 */
public interface ControllerSpecListener {
    
    public void onSpecRead(int hardwareVersion, int softwareVersion, int numButtons);
    
}
