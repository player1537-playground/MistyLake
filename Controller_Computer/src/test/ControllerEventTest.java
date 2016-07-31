
package test;

import controller.Controller;
import io.ControllerEmulator;
import events.ControllerEventGenerator;
import io.ControllerReader;
import io.ControllerWriter;
import io.SerialComm;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;

/**
 *
 * @author Andrew_2
 */
public class ControllerEventTest {

    public static void main(String[] args) {

        try {
            
            PipedOutputStream contOut = new PipedOutputStream();
            PipedInputStream compIn = new PipedInputStream(contOut);

            PipedOutputStream compOut = new PipedOutputStream();
            PipedInputStream contIn = new PipedInputStream(compOut);

            ControllerReader reader = new ControllerReader(compIn);

            ControllerWriter writer = new ControllerWriter(compOut);

            ControllerEventGenerator events = new ControllerEventGenerator(reader);
            reader.addReadListener(events);

            ControllerEventEcho echo = new ControllerEventEcho(events);

            MouseAndKeyEmulator emulator = new MouseAndKeyEmulator(contIn, contOut);
            
            ControllerDisplay disp = new ControllerDisplay(reader);
            
            Thread readerThread = new Thread(reader);
            Thread writerThread = new Thread(writer);
            Thread controllerThread = new Thread(emulator);
            readerThread.start();
            writerThread.start();
            controllerThread.start();
            
            
            Thread.sleep(1000);
            writer.requestSpec();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }
}
