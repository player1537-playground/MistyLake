package io;

import controller.Controller;
import controller.Trackpad;
import events.ControllerReadListener;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import message.Messenger;
import util.BlockInput;
import util.DefaultThread;

/**
 *
 * @author Andrew_2
 */
public class ControllerOverMessenger implements Messenger.MessageReceivedCallback, Controller {

    private Messenger messenger;
    private boolean close;

    DefaultThread writeThread;

    //Specification
    private int hardwareVersion, softwareVersion;
    private int buttonsUsed;

    //Data
    public static final int maxButtons = 16;

    private Trackpad t1, t2;
    private boolean[] buttons;
    private int jx, jy;

    private boolean requestSpec;

    private List<ControllerReadListener> readListeners;

    public static final byte SPEC_ID = 1,
            DATA_ID = 2, ERROR_ID = 3;

    public static final int messageLength = 254;

    public ControllerOverMessenger(Messenger messenger) {
        this.messenger = messenger;

        buttons = new boolean[maxButtons];
        readListeners = new ArrayList<>();
        t1 = new Trackpad(1);
        t2 = new Trackpad(1);

        writeThread = new DefaultThread(new Runnable() {
            @Override
            public void run() {

                byte[] rawBuffer = new byte[messageLength];
                ByteBuffer buffer = ByteBuffer.wrap(rawBuffer);
                if (requestSpec) {
                    buffer.put(SPEC_ID);
                    buffer.flip();

                    messenger.sendMessage(buffer);

                    buffer.clear();
                    requestSpec = false;

                }
            }
        });
    }

    public void start() {
        messenger.setMessageReceivedCallback(this);
        messenger.connect();
        writeThread.start();
    }

    @Override
    public void onMessageReceived(Messenger messenger, ByteBuffer msg) {

        int dataLength = msg.remaining();

        int id = (msg.get() & 0xFF);
        switch (id) {
            case (SPEC_ID): {

                hardwareVersion = msg.getInt();
                softwareVersion = msg.getInt();
                buttonsUsed = msg.getInt();
                break;
            }
            case (DATA_ID): {
                if (dataLength > 20) {
                    //Utility.printBytes(msg);
                    t1.setActive(0, msg.get() != 0);
                    t1.set(0, msg.getShort() & 0xFFFF, msg.getShort() & 0xFFFF, msg.getShort() & 0xFFFF);
                    
                    t2.setActive(0, msg.get() != 0);
                    t2.set(0, msg.getShort() & 0xFFFF, msg.getShort() & 0xFFFF, msg.getShort() & 0xFFFF);

                    int buttonShort = msg.getShort() & 0xFFFF;
                    for (int j = 0; j < maxButtons; j++) {
                        buttons[maxButtons - j - 1] = (buttonShort & 1) != 0;
                        buttonShort >>>= 1;
                    }

                    jx = msg.getShort() & 0xFFFF;
                    // System.out.println(jx);
                    jy = msg.getShort() & 0xFFFF;
                    //System.out.println(jy);
                } else {
                    System.err.println("Malformed data message received");
                }
                break;
            }
            default: {
                System.err.println("Message contained invalid ID: " + id);
                return;
            }

        }
        for (ControllerReadListener listener : readListeners) {
            listener.onRead(this, id);
        }

    }

    public void addReadListener(ControllerReadListener readListener) {
        readListeners.add(readListener);
    }

    @Override
    public Trackpad getTrack1() {
        return t1;
    }

    @Override
    public Trackpad getTrack2() {
        return t2;
    }

    public boolean[] getButtons() {
        return buttons;
    }

    public int getJoyStickX() {
        return jx;
    }

    public int getJoyStickY() {
        return jy;
    }

    @Override
    public int getHardwareVersion() {
        return hardwareVersion;
    }

    @Override
    public int getSoftwareVersion() {
        return softwareVersion;
    }

    public void close() {
        close = true;
        writeThread.stop();
        messenger.disconnect();

    }

    public int getButtonsUsed() {
        return buttonsUsed;
    }

    @Override
    public boolean getButton(int button) {
        return buttons[button];
    }

    @Override
    public int getNumButtons() {
        return buttonsUsed;
    }
    
    public void requestSpec() {
        this.requestSpec = true;
    }
}
