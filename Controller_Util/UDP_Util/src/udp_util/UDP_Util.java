/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package udp_util;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

/**
 *
 * @author Andrew_2
 */
public class UDP_Util {

    public static final int localPort = 1234;
    public static final int remotePort = 1234;

    public static void main(String[] args) throws Exception {
        InetAddress ipAddress = InetAddress.getByName("192.168.4.1");
        DatagramSocket socket = new DatagramSocket(localPort);
        
        DatagramPacket initPacket = new DatagramPacket(new byte[]{0}, 0, 1, ipAddress, remotePort);
        socket.send(initPacket);
        
        byte[] buffer = new byte[1024];

        while (true) {

            DatagramPacket receivePacket = new DatagramPacket(buffer, buffer.length);
            socket.receive(receivePacket);

            byte[] rec = receivePacket.getData();
            int offset = receivePacket.getOffset();
            int len = receivePacket.getLength();

            /*
            String recString = new String(rec, offset, len);
            System.out.println(recString);
            */
            
            int x1 = ((rec[0] << 8) & 0xFF00) | ((rec[1] & 0xFF));
            int y1 = ((rec[2] << 8) & 0xFF00) | ((rec[3] & 0xFF));
            int s1 = ((rec[4] << 8) & 0xFF00) | ((rec[5] & 0xFF));
            int x2 = ((rec[6] << 8) & 0xFF00) | ((rec[7] & 0xFF));
            int y2 = ((rec[8] << 8) & 0xFF00) | ((rec[9] & 0xFF));
            int s2 = ((rec[10] << 8) & 0xFF00) | ((rec[11] & 0xFF));
            System.out.println("x1: " + x1 + ", y1: " + y1 + ", s1: " + s1);
            System.out.println("x2: " + x2 + ", y2: " + y2 + ", s2: " + s2);

            Thread.yield();
        }

    }

}
