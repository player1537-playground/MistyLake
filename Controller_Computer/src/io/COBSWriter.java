
package io;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.ByteBuffer;

/**
 *
 * @author Andrew_2
 */
public class COBSWriter {

    OutputStream out;
    byte[] rawStuffed;
    ByteBuffer stuffed;

    public COBSWriter(OutputStream out, int unstuffedMessageLength) {
        this.out = out;
        rawStuffed = new byte[unstuffedMessageLength + 2];
        stuffed = ByteBuffer.wrap(rawStuffed);
    }

    public void write(ByteBuffer unstuffed) throws IOException {
        stuffBytes(unstuffed, stuffed);
        out.write(rawStuffed, 0, stuffed.remaining());
    }
    
    public static boolean stuffBytes(ByteBuffer source, ByteBuffer dest) {
        int length = source.remaining();
        if(length > dest.remaining()) {
            System.err.println("Source length greater than dest length");
            return false;
        }
        if(254 < length) {
            System.err.println("Source length greater than 254 not suppported");
            return false;
        }
        if(length == 0) {
            System.err.println("Cannot stuff Empty Source");
            return false;
        }
        
        dest.put((byte)0);
        int index = 0;
        int codeIndex = 0;
        byte current;
        while(index++ < length) {
            current = source.get();
            if(current == 0) {
                dest.put(codeIndex, (byte) (index - codeIndex));
                dest.put((byte)0);
                codeIndex = index;
            } else {
                dest.put(current);
            }
        }
        dest.put(codeIndex, (byte) (index - codeIndex));
        dest.put((byte)0);
        
        dest.flip();
        return true;
        
    }
}
