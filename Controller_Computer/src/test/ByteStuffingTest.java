
package test;

import io.COBSReader;
import io.COBSWriter;
import io.Utility;
import java.nio.ByteBuffer;

/**
 *
 * @author Andrew_2
 */
public class ByteStuffingTest {

    public static void main(String[] args) {
        byte[] message = {(byte) 0b01100010,
                          (byte) 0b01111100,
                          (byte) 0b00000111,
                          (byte) 0b00000000};
        ByteBuffer source = ByteBuffer.wrap(message);
        Utility.printBinary(source);
        source.rewind();
        
        ByteBuffer stuffed = ByteBuffer.allocate(message.length + 2);
        ByteBuffer unstuffed = ByteBuffer.allocate(message.length);
        
        COBSWriter.stuffBytes(source, stuffed);
        stuffed.rewind();
        Utility.printBinary(stuffed);
        stuffed.rewind();
        
        COBSReader.unstuffBytes(stuffed, unstuffed);
        unstuffed.rewind();
        Utility.printBinary(unstuffed);
        
    }

}
