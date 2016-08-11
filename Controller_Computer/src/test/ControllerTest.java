
package test;

import events.ControllerButtonListener;
import events.ControllerEventGenerator;
import io.ControllerReader;
import events.ControllerSpecListener;
import events.ControllerTrackListener;
import io.ControllerWriter;
import io.SerialComm;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Andrew_2
 */
public class ControllerTest {

    public static void main(String[] args) {

        try {
            SerialComm.getAvailableSerialPorts();

            SerialComm com = new SerialComm();
            com.connect("COM6");

            ControllerReader reader = new ControllerReader(com.getInputStream());

            ControllerWriter writer = new ControllerWriter(com.getOutputStream());

            ControllerEventGenerator events = new ControllerEventGenerator(reader);
            reader.addReadListener(events);

            ControllerEventEcho echo = new ControllerEventEcho(events);
            
            
            ControllerDisplay disp = new ControllerDisplay(reader);

            Thread readerThread = new Thread(reader);
            readerThread.start();
            Thread writerThread = new Thread(writer);
            writerThread.start();

            Thread.sleep(2000);
            writer.requestSpec();
            Thread.sleep(2000);
            writer.requestSpec();
        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }
}
