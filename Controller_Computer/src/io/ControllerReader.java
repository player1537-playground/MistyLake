
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

/**
 *
 * @author Andrew_2
 */
public class ControllerReader implements Runnable, Parser, Controller {

    private InputStream in;
    private boolean close;

    private COBSReader cobsReader;
    
    private static final int inSize = 1024;
    private static final int inMessageSize = 254;
    
    //Specification
    private int hardwareVersion, softwareVersion;
    private int buttonsUsed;

    //Data
    public static final int maxButtons = 16;

    Trackpad t1, t2;
    private boolean[] buttons;

    private List<ControllerReadListener> readListeners;

    public static final byte SPEC_ID = 1,
            DATA_ID = 2, ERROR_ID = 3;

    public ControllerReader(InputStream in) {
        this.in = in;
        cobsReader = new COBSReader(in, inSize, inMessageSize, this);
        buttons = new boolean[maxButtons];
        readListeners = new ArrayList<>();
        t1 = new Trackpad(1);
        t2 = new Trackpad(1);
    }

    @Override
    public void run() {
        try {
            //add timeout later

            while (!close) {
                cobsReader.read();
                Thread.sleep(16);
            }
            in.close();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public void parse(ByteBuffer unstuffed) {

        int dataLength = unstuffed.remaining();
        int readLength = (unstuffed.get() & 0xFF);

        if (readLength != dataLength) {
            System.err.println("Invalid Message. Read length " + readLength + " did not match data length " + dataLength);
            return;
        }

        int id = (unstuffed.get() & 0xFF);
        switch (id) {
            case (SPEC_ID): {

                hardwareVersion = unstuffed.getInt();
                softwareVersion = unstuffed.getInt();
                buttonsUsed = unstuffed.getInt();
                break;
            }
            case (DATA_ID): {
                t1.set(0, unstuffed.getShort(),unstuffed.getShort(),unstuffed.getShort());
                t2.set(0, unstuffed.getShort(),unstuffed.getShort(),unstuffed.getShort());

                
                short buttonShort = unstuffed.getShort();
                for (int j = 0; j < maxButtons; j++) {
                    buttons[maxButtons - j - 1] = (buttonShort & 1) != 0;
                    buttonShort >>>= 1;
                }
                break;
            }
            default: {
                System.err.println("Message contained invalid ID");
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

    public Trackpad getTrack1() {
        return t1;
    }

    public Trackpad getTrack2() {
        return t2;
    }

    public boolean[] getButtons() {
        return buttons;
    }

    public int getHardwareVersion() {
        return hardwareVersion;
    }

    public int getSoftwareVersion() {
        return softwareVersion;
    }

    public void close() {
        close = true;
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
}
