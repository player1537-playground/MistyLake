
package io;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 *
 * @author Andrew_2
 */
public class COBSReader {

    private InputStream in;
    private byte[] rawBuffer;
    private ByteBuffer buffer, stuffed, unstuffed;
    private int bufferSize, unstuffedLength;
    private boolean validMessage = true;

    private Parser parser;

    public COBSReader(InputStream in, int bufferSize, int unstuffedLength, Parser parser) {

        this.in = in;
        this.bufferSize = bufferSize;
        this.unstuffedLength = unstuffedLength;
        this.parser = parser;
        rawBuffer = new byte[bufferSize];
        buffer = ByteBuffer.wrap(rawBuffer);
        stuffed = ByteBuffer.allocate(unstuffedLength + 2);
        unstuffed = ByteBuffer.allocate(unstuffedLength);
    }

    public void read() throws IOException {
        //add timeout later

        if (in.available() > 0) {
            int numRead = in.read(rawBuffer);
            buffer.clear();
            buffer.limit(numRead);
            while (buffer.hasRemaining()) {
                byte current = buffer.get();
                if (stuffed.hasRemaining()) {
                    stuffed.put(current);
                } else {
                    System.err.println("Message length exceeded in reader.");
                    validMessage = false;
                }

                if (current == 0) {
                    if (validMessage) {
                        stuffed.flip();
                        if (!unstuffBytes(stuffed, unstuffed)) {
                            System.err.println("Invalid Message. Message could not be unstuffed");
                        } else {
                            parser.parse(unstuffed);
                        }
                    }
                    validMessage = true;
                    stuffed.clear();
                    unstuffed.clear();

                }

            }
        }

    }

    public static boolean unstuffBytes(ByteBuffer source, ByteBuffer dest) {
        int length = source.remaining();

        if (dest.remaining() + 2 < length) {
            System.err.println("Source length greater than dest length");
            return false;
        }
        if (256 < length) {
            System.err.println("Source length greater than 256 not suppported");
            return false;
        }
        if (2 >= length) {
            System.err.println("Empty source");
            return false;
        }

        int index = 0;
        while (index < length - 1) {
            int stop = (source.get() & 0xFF) - 1 + index;
            while (index++ < stop) {
                dest.put(source.get());
            }
            if (index + 1 != length) {
                dest.put((byte) 0);
            }
        }

        dest.flip();

        return true;
    }

}
