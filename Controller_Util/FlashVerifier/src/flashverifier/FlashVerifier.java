package flashverifier;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Andrew_2
 */
public class FlashVerifier {

    public static void main(String[] args) throws IOException {
        File file = new File("flash_console_data/firm2.txt");
        int[][] data = new int[240][64];
        for (int i = 0; i < 240; i++) {
            for (int j = 0; j < 64; j++) {
                data[i][j] = -1;
            }
        }

        BufferedReader br = new BufferedReader(new FileReader(file));
        String line;
        while ((line = br.readLine()) != null) {
            if (line.startsWith("Block")) {
                String[] spl = line.split(" ");
                String blockNumString = spl[1];
                blockNumString = blockNumString.substring(0, blockNumString.length() - 1);
                System.out.println(blockNumString);
                int blockNum = Integer.parseInt(blockNumString);
                int offset = 3;
                for (int i = 0; i < data[blockNum].length; i++) {
                    String dataStr = spl[offset + i];
                    int dataInt = Integer.parseInt(dataStr);
                    if (data[blockNum][i] == -1) {
                        data[blockNum][i] = dataInt;
                    } else if (data[blockNum][i] != dataInt) {
                        System.err.println("Disagreement at " + blockNum + " " + i);
                    }
                }
            }
        }
        br.close();

        File raw = new File("raw_data/firm2.dat");
        BufferedWriter rawWriter = new BufferedWriter(new FileWriter(raw));

        for (int i = 0; i < 240; i++) {
            for (int j = 0; j < 64; j++) {
                rawWriter.write("" + data[i][j] + " ");
            }
            rawWriter.write("\n");
        }
        rawWriter.flush();
        rawWriter.close();

        File header = new File("header_data/firm2.h");
        BufferedWriter headerWriter = new BufferedWriter(new FileWriter(header));

        headerWriter.write("#ifndef HEX_ARRAY_H\n");
        headerWriter.write("#define HEX_ARRAY_H\n");
        headerWriter.write("\n");
        headerWriter.write("const uint8 ICACHE_FLASH_ATTR firm_array[] = {\n");

        for (int i = 0; i < 240; i++) {
            for (int j = 0; j < 64; j++) {
                headerWriter.write("" + data[i][j] + ", ");
            }
            headerWriter.write("\n");
        }

        headerWriter.write("};\n");
        headerWriter.write("#endif\n");

        headerWriter.flush();
        headerWriter.close();
        
    }

}
