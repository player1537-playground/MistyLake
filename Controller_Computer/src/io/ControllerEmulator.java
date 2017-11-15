package io;

import controller.Controller;
import io.ControllerOverMessenger;
import controller.Trackpad;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;
import message.Messenger;
import util.DefaultThread;

/**
 *
 * @author Andrew_2
 */
public class ControllerEmulator implements Controller, Messenger.MessageReceivedCallback {

    private Messenger messenger;
    private DefaultThread writeThread;

    private int hardwareVersion;
    private int softwareVersion;
    private int numButtons;

    private byte[] rawOutBuffer;
    private ByteBuffer outBuffer;

    private Trackpad t1, t2;
    private boolean[] buttons;

    private boolean closed;
    private boolean writeSpec;

    private int inSize;
    private int inMessageSize;
    private int outMessageSize;

    private static final int numFingers = 1;

    private static final int defaultInSize = 1024;
    private static final int defaultInMessageSize = 254;
    private static final int defaultOutMessageSize = 254;

    private static final int defaultHardwareVersion = 144;
    private static final int defaultSoftwareVersion = 351;
    private static final int defaultNumButtons = 6;

    private static final byte dataMessageSize = 16; //1 + 1 + 2 * 3 * 2 + 2
    private static final byte specMessageSize = 16; //1 + 1 + 4 + 4 + 4 + 2

    private static final int maxButtons = 16;

    public ControllerEmulator(Messenger messenger) {
        this(messenger, defaultHardwareVersion, defaultSoftwareVersion, defaultNumButtons);
    }

    public ControllerEmulator(Messenger messenger,
            int hardwareVersion, int softwareVersion, int numButtons) {
        this(messenger, hardwareVersion, softwareVersion, numButtons,
                defaultInSize, defaultInMessageSize, defaultOutMessageSize);
    }

    public ControllerEmulator(Messenger messenger,
            int hardwareVersion, int softwareVersion, int numButtons,
            int inSize, int inMessageSize, int outMessageSize) {
        this.messenger = messenger;

        this.hardwareVersion = hardwareVersion;
        this.softwareVersion = softwareVersion;
        this.numButtons = numButtons;

        this.inSize = inSize;
        this.inMessageSize = inMessageSize;
        this.outMessageSize = outMessageSize;

        t1 = new Trackpad(numFingers);
        t2 = new Trackpad(numFingers);
        buttons = new boolean[maxButtons];

        rawOutBuffer = new byte[outMessageSize];
        outBuffer = ByteBuffer.wrap(rawOutBuffer);

        writeThread = new DefaultThread(new Runnable() {
            @Override
            public void run() {
                try {
                    if (writeSpec) {
                        writeSpec();
                        writeSpec = false;
                    }
                    writeData();
                } catch (IOException ex) {
                    ex.printStackTrace();
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
    public Trackpad getTrack1() {
        return t1;
    }

    @Override
    public Trackpad getTrack2() {
        return t2;
    }

    @Override
    public boolean getButton(int button) {
        return buttons[button];
    }

    @Override
    public int getHardwareVersion() {
        return hardwareVersion;
    }

    @Override
    public int getSoftwareVersion() {
        return softwareVersion;
    }

    @Override
    public int getNumButtons() {
        return numButtons;
    }

    public void setButton(int button, boolean val) {
        buttons[button] = val;
    }

    public void close() {
        writeThread.stop();
        messenger.disconnect();
        closed = true;
    }

    private void writeData() throws IOException {
        outBuffer.clear();
        outBuffer.put(ControllerOverMessenger.DATA_ID);
        outBuffer.put((byte)(t1.getActive(0) ? 0 : 1));
        outBuffer.putShort((short)t1.getX(0));
        outBuffer.putShort((short)t1.getY(0));
        outBuffer.putShort((short)t1.getStrength(0));
        outBuffer.put((byte)(t2.getActive(0) ? 0 : 1));
        outBuffer.putShort((short)t2.getX(0));
        outBuffer.putShort((short)t2.getY(0));
        outBuffer.putShort((short)t2.getStrength(0));
        outBuffer.putShort(booleansToShort(buttons));
        outBuffer.putShort((short)0);
        outBuffer.putShort((short)0);
        outBuffer.putShort((short)0);
        outBuffer.putShort((short)0);
        outBuffer.flip();

        messenger.sendMessage(outBuffer);
    }

    private void writeSpec() throws IOException {

        outBuffer.clear();
        outBuffer.put(ControllerOverMessenger.SPEC_ID);
        outBuffer.putInt(hardwareVersion);
        outBuffer.putInt(softwareVersion);
        outBuffer.putInt(numButtons);
        outBuffer.put((byte) 0);
        outBuffer.put((byte) 0);
        outBuffer.flip();

        messenger.sendMessage(outBuffer);
    }

    public static short booleansToShort(boolean[] values) {
        short buttonShort = 0;
        for (int i = 0; i < 16; i++) {
            buttonShort |= values[i] ? 1 : 0;

            if (i != 15) {
                buttonShort <<= 1;
            }
        }
        return buttonShort;
    }

    @Override
    public void onMessageReceived(Messenger messenger, ByteBuffer msg) {
        int dataLength = msg.remaining();
        
        int id = msg.get() & 0xFF;
        switch (id) {
            case (ControllerOverMessenger.SPEC_ID): {

                writeSpec = true;
                break;
            }
        }
    }

}
