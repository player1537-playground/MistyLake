
package io;

import java.nio.ByteBuffer;

/**
 *
 * @author Andrew_2
 */
public class Utility {
    
    public static void printBytes(ByteBuffer buffer) {
        int pos = buffer.position();
        StringBuilder sb = new StringBuilder();
        while(buffer.hasRemaining()) {
            sb.append(buffer.get() & 0xFF);
            sb.append(' ');
        }
        System.out.println(sb.toString());
        buffer.position(pos);
        
    }
    
    public static void printBinary(ByteBuffer buffer) {
        int pos = buffer.position();
        StringBuilder sb = new StringBuilder();
        
        while(buffer.hasRemaining()) {
            byte current = buffer.get();
            for(int j = 0; j < 8; j++) {
                sb.append((current & 0b10000000) == 0b10000000 ? '1' : '0');
                current >>>= 1;
            }
            sb.append(' ');
        }
        System.out.println(sb.toString());
        buffer.position(pos);
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
    
}
