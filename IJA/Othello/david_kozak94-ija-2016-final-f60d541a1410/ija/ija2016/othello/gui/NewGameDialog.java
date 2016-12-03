/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi NewGameDialog :
 *  Dialog, pres ktery se spousti nova hra.
 */

package ija.ija2016.othello.gui;

import ija.ija2016.othello.game.OpponentType;
import ija.ija2016.othello.game.StoneFrozer;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.text.NumberFormat;
import java.util.Arrays;

public class NewGameDialog extends JDialog {
    private JPanel contentPane;
    private JButton buttonOK;
    private JButton buttonCancel;
    private JTextField gameNameField;
    private JTextField boardSizeTextField;
    private JComboBox boxSizeComboBox;
    private JRadioButton humanRadioButton;
    private JRadioButton PCEasyRadioButton;
    private JRadioButton PCHardRadioButton;
    private JTextField typeOfGameTextField;
    private JTextField isWinterCommingTextField;
    private JCheckBox yesCheckBox;
    private JTextField numberOfStonesTextField;
    private JTextField threadSleepTextField;
    private JTextField freezeTimeTextField;
    private JPanel freezerInfoPain;
    private JTextField gameName;
    private JTextField stonesToFreeze;
    private JTextField threadSleepTime;
    private JTextField freezeTime;

    public NewGameDialog() {
        setTitle("Start new game");
        setContentPane(contentPane);
        setModal(true);
        getRootPane().setDefaultButton(buttonOK);

        boxSizeComboBox.addItem("6");
        boxSizeComboBox.addItem("8");
        boxSizeComboBox.addItem("10");
        boxSizeComboBox.addItem("12");
        boxSizeComboBox.setSelectedIndex(1);

        gameName.setMinimumSize(new Dimension(50,10));
        gameName.setMaximumSize(new Dimension(50,10));
        gameName.setSize(new Dimension(50,10));

        buttonOK.addActionListener(e -> onOK());

        buttonCancel.addActionListener(e -> onCancel());

        // call onCancel() when cross is clicked
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                onCancel();
            }
        });

        yesCheckBox.addChangeListener((x)->{
            Arrays.stream(freezerInfoPain.getComponents()).forEach(y->y.setEnabled(yesCheckBox.isSelected()));
        });

        humanRadioButton.addChangeListener((x)->{
            if(humanRadioButton.isSelected()){
                PCHardRadioButton.setSelected(false);
                PCEasyRadioButton.setSelected(false);
            }
        });

        PCEasyRadioButton.addChangeListener((x)->{
            if(PCEasyRadioButton.isSelected()){
                humanRadioButton.setSelected(false);
                PCHardRadioButton.setSelected(false);
            }
        });

        PCHardRadioButton.addChangeListener((x)->{
            if(PCHardRadioButton.isSelected()){
                humanRadioButton.setSelected(false);
                PCEasyRadioButton.setSelected(false);
            }
        });



        // call onCancel() on ESCAPE
        contentPane.registerKeyboardAction(e -> onCancel(), KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0), JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
        this.setLocationRelativeTo(null);
        this.setFocusable(true);
        this.pack();
        this.setResizable(false);
        this.setVisible(true);
        this.setMinimumSize(new Dimension(800,600));
    }

    private void onOK() {
        // FIXME check if the input values are correct
        int size = Integer.parseInt(boxSizeComboBox.getSelectedItem().toString());
        StoneFrozer.FreezerInfo freezerInfo = null;
        if(yesCheckBox.isSelected()) {
            freezerInfo =
                    new StoneFrozer.FreezerInfo(
                            Integer.parseInt(threadSleepTime.getText()),
                            Integer.parseInt(freezeTime.getText()),
                            Integer.parseInt(stonesToFreeze.getText()));
        }

        OpponentType opponentType;
        if(humanRadioButton.isSelected())
            opponentType = OpponentType.human;
        else if(PCEasyRadioButton.isSelected())
            opponentType = OpponentType.pc_easy;
        else opponentType = OpponentType.pc_hard;

        MainWindow.startGame(gameName.getText(),size,freezerInfo,opponentType);

        dispose();
    }

    private void onCancel() {

        dispose();
    }

    private void createUIComponents() {
        stonesToFreeze = new JFormattedTextField(NumberFormat.getIntegerInstance());
        threadSleepTime = new JFormattedTextField(NumberFormat.getIntegerInstance());
        freezeTime = new JFormattedTextField(NumberFormat.getIntegerInstance());
    }
}
