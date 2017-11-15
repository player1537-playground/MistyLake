package test;

import comm.PipedBlockComm;
import comm.UDPBlockComm;
import events.ControllerEventGenerator;
import io.ControllerOverMessenger;
import java.net.InetAddress;
import message.Messenger;
import message.TransparentBlockMessenger;

/**
 *
 * @author Andrew_2
 */
public class ControllerEventTest {

    public static void main(String[] args) {

        try {
            
            //UDPBlockComm blockComm = new UDPBlockComm(InetAddress.getByName("192.168.4.1"), 1234);
            //Messenger messengerComputer = new TransparentBlockMessenger(blockComm);
            Messenger messengerComputer = createEmulatorAndMessenger();
            
            ControllerOverMessenger controller = new ControllerOverMessenger(messengerComputer);

            ControllerEventGenerator events = new ControllerEventGenerator(controller);
            controller.addReadListener(events);

            ControllerEventEcho echo = new ControllerEventEcho(events);

            ControllerDisplay disp = new ControllerDisplay(controller);

            controller.start();

            Thread.sleep(1000);
            controller.requestSpec();

        } catch (Exception ex) {
            ex.printStackTrace();
        }

    }

    public static Messenger createEmulatorAndMessenger() {
        PipedBlockComm commEmulator = new PipedBlockComm();
        PipedBlockComm commComputer = new PipedBlockComm();
        commComputer.setTarget(commEmulator);

        Messenger messengerEmulator = new TransparentBlockMessenger(commEmulator);
        Messenger messengerComputer = new TransparentBlockMessenger(commComputer);

        MouseAndKeyEmulator emulator = new MouseAndKeyEmulator(messengerEmulator);
        emulator.start();
        
        return messengerComputer;
    }
}
