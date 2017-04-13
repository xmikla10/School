/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi GenericDialog:
 *  Obecny dialog, ktery je schopen informovat uzivatele o libovolne situaci 
 *  s parametrizovatelnymi operacemi pro tlacitko OK a Cacncel
 */

package ija.ija2016.othello.gui;

import javax.swing.*;
import java.awt.event.*;
import java.util.function.Consumer;

public class GenericDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JButton buttonCancel;
    private JTextField msgField;

    private final Runnable okAction;
    private final Runnable cancelAction;

    public GenericDialog(String title,String msg){
        this.setTitle(title);
        msgField.setText(msg);

        okAction = this::dispose;
        cancelAction = this::dispose;
    }

    public GenericDialog(String title,String msg,Runnable okAction,Runnable cancelAction) {
        this(title,msg,okAction,cancelAction,"OK","Cancel");
    }

    public GenericDialog(String title,String msg,Runnable okAction,Runnable cancelAction,String okButtonText, String cancelButtonText) {
        this.okAction = okAction;
        this.cancelAction = cancelAction;
        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);

        buttonOK.setText(okButtonText);
        buttonCancel.setText(cancelButtonText);

        this.setTitle(title);
        msgField.setText(msg);

        buttonOK.addActionListener(x->onOK());
        buttonCancel.addActionListener(x->onCancel());

// call onCancel() when cross is clicked
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                onCancel();
            }
        });

// call onCancel() on ESCAPE
        contentPane.registerKeyboardAction(e -> onCancel(), KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
        this.setLocationRelativeTo(null);
        this.setSize(200,100);
        this.setVisible(true);
    }

    private void onOK() {
// add your code here
        okAction.run();
        dispose();
    }

    private void onCancel() {
// add your code here if necessary
        cancelAction.run();
        dispose();
    }
}
