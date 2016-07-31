
package io;

import controller.Controller;
import io.COBSReader;
import io.COBSWriter;
import io.ControllerReader;
import io.Parser;
import controller.Trackpad;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;

/**
 *
 * @author Andrew_2
 */
public class ControllerEmulator implements Controller, Runnable, Parser {

    private InputStream in;
    private OutputStream out;
    private COBSReader reader;
    private COBSWriter writer;

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

    public ControllerEmulator(InputStream in, OutputStream out) {
        this(in, out, defaultHardwareVersion, defaultSoftwareVersion, defaultNumButtons);
    }

    public ControllerEmulator(InputStream in, OutputStream out,
            int hardwareVersion, int softwareVersion, int numButtons) {
        this(in, out, hardwareVersion, softwareVersion, numButtons,
                defaultInSize, defaultInMessageSize, defaultOutMessageSize);
    }

    public ControllerEmulator(InputStream in, OutputStream out,
            int hardwareVersion, int softwareVersion, int numButtons,
            int inSize, int inMessageSize, int outMessageSize) {
        this.in = in;
        this.out = out;

        this.hardwareVersion = hardwareVersion;
        this.softwareVersion = softwareVersion;
        this.numButtons = numButtons;

        this.inSize = inSize;
        this.inMessageSize = inMessageSize;
        this.outMessageSize = outMessageSize;

        reader = new COBSReader(in, inSize, inMessageSize, this);
        writer = new COBSWriter(out, outMessageSize);

        t1 = new Trackpad(numFingers);
        t2 = new Trackpad(numFingers);
        buttons = new boolean[maxButtons];

        rawOutBuffer = new byte[outMessageSize];
        outBuffer = ByteBuffer.wrap(rawOutBuffer);

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
        closed = true;
    }

    @Override
    public void run() {
        try {

            while (!closed) {

                reader.read();

                if (writeSpec) {
                    writeSpec();
                    writeSpec = false;
                }
                writeData();
                Thread.sleep((long) 100);
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }
    }

    private void writeData() throws IOException {
        outBuffer.clear();
        outBuffer.put(dataMessageSize);
        outBuffer.put(ControllerReader.DATA_ID);
        outBuffer.putShort(t1.getX(0));
        outBuffer.putShort(t1.getY(0));
        outBuffer.putShort(t1.getStrength(0));
        outBuffer.putShort(t2.getX(0));
        outBuffer.putShort(t2.getY(0));
        outBuffer.putShort(t2.getStrength(0));
        outBuffer.putShort(booleansToShort(buttons));
        outBuffer.flip();

        writer.write(outBuffer);
    }

    private void writeSpec() throws IOException {

        outBuffer.clear();
        outBuffer.put(specMessageSize);
        outBuffer.put(ControllerReader.SPEC_ID);
        outBuffer.putInt(hardwareVersion);
        outBuffer.putInt(softwareVersion);
        outBuffer.putInt(numButtons);
        outBuffer.put((byte)0);
        outBuffer.put((byte)0);
        outBuffer.flip();

        writer.write(outBuffer);
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
    public void parse(ByteBuffer message) {
        int dataLength = message.remaining();
        int readLength = message.get() & 0xFF;
        
        if (readLength != dataLength) {
            System.err.println("Invalid Message. Read length did not match data length");
            return;
        }

        int id = message.get() & 0xFF;
        switch (id) {
            case (ControllerReader.SPEC_ID): {

                writeSpec = true;
                break;
            }
        }
    }

}
