/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi LoadGameDialog :
 *  Dialog pro zobrazeni a znovuspusteni ulozene hry.
 */

package ija.ija2016.othello.gui;

import ija.ija2016.othello.IOmanager;
import ija.ija2016.othello.game.Game;
import javax.swing.*;
import java.awt.event.*;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import static java.util.stream.Collectors.toList;

public class LoadGameDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JButton buttonCancel;
    private JPanel mainPanel;

    public LoadGameDialog(List<String> savedGames) {
        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);

        buttonOK.addActionListener(e -> onOK());

        buttonCancel.addActionListener(e -> onCancel());

        // call onCancel() when cross is clicked
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                onCancel();
            }
        });

        // call onCancel() on ESCAPE
        contentPane.registerKeyboardAction(e -> onCancel(), KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);


        ButtonGroup buttonGroup = new ButtonGroup();
        savedGames.stream().
                map(x->x.replace(IOmanager.FILE_EXTENSION,"")).
                map(JRadioButton::new).
                forEach(
                        x -> {
                            buttonGroup.add(x);
                            mainPanel.add(x);
                        });


        this.pack();
        this.setLocationRelativeTo(null);
        this.setVisible(true);
    }

    private void onOK() {
        List<String> l = Arrays.stream(mainPanel.getComponents()).map(x->(JRadioButton)x).filter(JRadioButton::isSelected).map(JRadioButton::getText).collect(toList());
        assert l.size() == 1;
        try {
            Game game = IOmanager.loadGame(l.get(0));
            MainWindow.reloadGame(game);
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
            System.exit(1);
        } finally {
            dispose();
        }
    }

    private void onCancel() {

        dispose();
    }

    private void createUIComponents() {
        mainPanel = new JPanel();
    }
}
