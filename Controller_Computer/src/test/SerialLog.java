/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package test;

import gnu.io.CommPortIdentifier;
import io.SerialComm;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import static java.awt.event.KeyEvent.VK_ENTER;
import java.awt.event.KeyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.ScrollPaneConstants;

public class SerialLog extends JFrame implements Runnable {

    BufferedOutputStream out;
    BufferedInputStream in;

    JTextField userInput;
    JTextArea log;
    JScrollBar bar;
    
    public static void main(String[] args) {
        listPorts();
        //SerialLog sl = new SerialLog("COM7", 115200);
        //(new Thread(sl)).start();
                
    }

    public SerialLog(String port, int baud) {
        super();
        this.setVisible(true);
        
        GridBagLayout layout = new GridBagLayout();
        GridBagConstraints con = new GridBagConstraints();
        con.insets = new Insets(10, 10, 10, 10);
        this.setLayout(layout);
        
        userInput = new JTextField(50);
        this.add(userInput, con);
        
        con.gridy = 2;
        log = new JTextArea(20, 50);
        JScrollPane pane = new JScrollPane(log);
        pane.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_AS_NEEDED);
        bar = pane.getVerticalScrollBar();
        log.setEditable(false);
        this.add(pane, con);
        
        this.pack();
        

        SerialComm com = new SerialComm();
        
        //System.out.println(com.getAvailableSerialPorts().iterator().next().getName());
                
        com.connect(port, baud);

        out = new BufferedOutputStream(com.getOutputStream());
        in = new BufferedInputStream(com.getInputStream());

        userInput.addKeyListener(new KeyAdapter() {

            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == VK_ENTER) {
                    sendCommand(userInput.getText() + "\r\n");
                    userInput.setText("");
                }
            }

        });
        
        this.addWindowListener(new WindowAdapter() {

            @Override
            public void windowClosing(WindowEvent e) {
                System.exit(0);
            }
        });
    }

    @Override
    public void run() {
        try {
            while (true) {
                if (in.available() > 0) {
                    char inChar = (char)in.read();
                    log.append("" + inChar);
                }
            }
        } catch (IOException ex) {
            ex.printStackTrace();
            System.exit(1);
        }
    }

    private void sendCommand(String command) {
        try {
            log.append(command);
            bar.setValue(bar.getMaximum());
            out.write(command.getBytes());
            out.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
            System.exit(1);
        }
    }
    static void listPorts()
    {
        System.out.println("EHEHE");
        java.util.Enumeration<CommPortIdentifier> portEnum = CommPortIdentifier.getPortIdentifiers();
        while ( portEnum.hasMoreElements() ) 
        {
            CommPortIdentifier portIdentifier = portEnum.nextElement();
            System.out.println(portIdentifier.getName()  +  " - " +  getPortTypeName(portIdentifier.getPortType()) );
        }        
    }
    
    static String getPortTypeName ( int portType )
    {
        switch ( portType )
        {
            case CommPortIdentifier.PORT_I2C:
                return "I2C";
            case CommPortIdentifier.PORT_PARALLEL:
                return "Parallel";
            case CommPortIdentifier.PORT_RAW:
                return "Raw";
            case CommPortIdentifier.PORT_RS485:
                return "RS485";
            case CommPortIdentifier.PORT_SERIAL:
                return "Serial";
            default:
                return "unknown type";
        }
    }
    
}
