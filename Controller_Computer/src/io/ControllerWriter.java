
package io;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Andrew_2
 */
public class ControllerWriter implements Runnable {

    private OutputStream out;
    private boolean close;
    private boolean requestSpec;
    private COBSWriter cobsWriter;
    
    public static final int messageLength = 254;

    public static final byte SPEC_ID = 1;

    public ControllerWriter(OutputStream out) {
        this.out = out;
        cobsWriter = new COBSWriter(out, messageLength + 2);
    }

    @Override
    public void run() {
        
        byte[] rawBuffer = new byte[messageLength];
        ByteBuffer buffer = ByteBuffer.wrap(rawBuffer);
        try {
            while (!close) {
                
                if (requestSpec) {
                    buffer.put((byte)2);
                    buffer.put(SPEC_ID);
                    buffer.flip();
                    
                    cobsWriter.write(buffer);
                    
                    buffer.clear();
                    requestSpec = false;

                }
                Thread.sleep(16);
            }
            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException ex) {
            ex.printStackTrace();
        }
    }

    public void close() {
        close = true;
    }

    public void requestSpec() {
        requestSpec = true;
    }

}
