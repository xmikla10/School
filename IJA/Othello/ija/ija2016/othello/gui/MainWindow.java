/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi MainWindow :
 *  Centralni okno obsahujici herni desku a tlacitka pro ovladani hry.
 */

package ija.ija2016.othello.gui;

import ija.ija2016.othello.IOmanager;
import ija.ija2016.othello.board.AbstractField;
import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.board.Disk;
import ija.ija2016.othello.game.*;
import ija.ija2016.othello.game.ai.AI;
import ija.ija2016.othello.game.ai.ComputerPlayerEasy;
import ija.ija2016.othello.game.ai.ComputerPlayerHard;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.IOException;

public class MainWindow extends JFrame implements MainWindowCallback {
    private JPanel root;
    private JPanel upperPane;
    private JTextField scoreField;
    private JTextField infoField;
    private JPanel downPanel;
    private JButton revertLastTurnButton;
    private JButton hintButton;
    private JPanel gameDesk;
    private JPanel sidePanel;

    private final Game game;

    private int fieldSize = 40;


    public MainWindow(Game game) {
        this.game = game;
        this.setTitle(game.getName());
        setJMenuBar(new MainWindowBar(game));
        game.setCallback(this);
        revertLastTurnButton.addActionListener((x) -> {
            game.revertLastTurn();
            updateScoreField(game.computeScore());
            repaint();
        });
        hintButton.addActionListener((x) -> {
                    AbstractField f = AI.computeHintMove(game.currentPlayer(), game.getBoard());
                    game.move(f.getRow(), f.getCol());
                }
        );

        setContentPane(root);
        scoreField.setEditable(false);
        scoreField.setFocusable(false);
        infoField.setEditable(false);
        infoField.setFocusable(false);

        // at the beginning, to score is 2:2
        this.updateScoreField(new Point(2, 2));
        this.updateInfoField("");

        this.setSize(1024, 640);
        this.setFocusable(true);
        this.setLocationRelativeTo(null);
        this.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        this.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                // kill the thread
                StoneFrozer s = game.getStoneFrozer();
                if (s != null) {
                    try {
                        s.interrupt();
                        s.join();
                    } catch (InterruptedException e1) {
                        e1.printStackTrace();
                    }
                    game.setStoneFrozer(null);
                }
                dispose();
            }
        });

    }

    @Override
    public void endGame(boolean isWhite, Point score) {
        new GenericDialog("Game over", (isWhite ? "White" : "Black") + "player won. Score was: " + score.x + ":" + score.y + " Congratulations :)", MainWindow.this::dispose, MainWindow.this::dispose);
    }

    public void updateInfoField(String s) {
        this.infoField.setText(s);
    }

    public void updateScoreField(Point score) {
        this.scoreField.setText("White: " + score.x + " Black: " + score.y);
    }

    public static void reloadGame(Game game) {
        MainWindow mainWindow = new MainWindow(game);
        game.getStoneFrozer().setCallback(mainWindow);
        SwingUtilities.invokeLater(() -> mainWindow.setVisible(true));
        mainWindow.repaint();
    }


    public static void startGame(int size, StoneFrozer.FreezerInfo freezerInfo, OpponentType opponentType) {
        startGame(Game.DEFAULT_NAME, size, freezerInfo, opponentType);
    }

    public static void startGame(String name, int size, StoneFrozer.FreezerInfo freezerInfo, OpponentType opponentType) {
        ReversiRules reversiRules = new ReversiRules(size);
        Board board = new Board(reversiRules);
        Game game = new Game(board);
        game.addPlayer(new HumanPlayer(true));
        game.setName(name);

        switch (opponentType) {
            case human:
                game.addPlayer(new HumanPlayer(false));
                break;
            case pc_easy:
                game.addPlayer(new ComputerPlayerEasy(false));
                break;
            case pc_hard:
                game.addPlayer(new ComputerPlayerHard(false));
                break;
        }

        MainWindow mainWindow = new MainWindow(game);

        System.out.println("after constructor of MainWindow");

        if (freezerInfo != null) {
            StoneFrozer stoneFrozer = new StoneFrozer(mainWindow, board.getGameFields(), freezerInfo);
            game.setStoneFrozer(stoneFrozer);
            stoneFrozer.start();
        }


        // set look and feel to the system look and feel
        try {
            for (UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace(); // If Nimbus is not available, you can set the GUI to another look and feel.
            System.exit(1);
        }

        SwingUtilities.invokeLater(() -> mainWindow.setVisible(true));

        mainWindow.repaint();
    }

    public static void main(String... args) {
        try {
            IOmanager.prepareDirectoryForSavedGames();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }
        startGame(8, null, OpponentType.human);
    }

    private void createUIComponents() {
        gameDesk = new JPanel() {

            private boolean whiteField = true;

            private void paintField(int i, int j, Graphics g) {
                g.setColor(whiteField ? new Color(0, 200, 0) : new Color(0, 150, 0));
                g.fillRect(i, j, fieldSize, fieldSize);
                g.setColor(Color.BLACK);
                g.drawRect(i, j, fieldSize, fieldSize);
                whiteField = !whiteField;
            }

            private void paintCircle(int i, int j, Graphics g, Color c) {
                g.setColor(c);
                g.fillOval(j + fieldSize / 4, i + fieldSize / 4, fieldSize / 2, fieldSize / 2);
            }

            private void paintDisk(int i, int j, Graphics g) {
                IOthelloField f = (IOthelloField) game.getBoard().getField(i, j);
                if (f == null)
                    return;
                i -= 1;
                j -= 1;
                i *= fieldSize;
                j *= fieldSize;
                if (f.isFrozen()) {
                    paintCircle(i, j, g, Color.BLUE);
                } else {
                    Disk d = f.getDisk();
                    if (d != null) {
                        if (d.isWhite()) {
                            paintCircle(i, j, g, Color.WHITE);
                        } else {
                            paintCircle(i, j, g, Color.BLACK);
                        }
                    }
                }
            }

            private void center() {
                gameDesk.setLocation((int) gameDesk.getSize().getWidth() / 2 - (game.getBoard().getSize() * fieldSize) / 2, this.getLocation().y);
            }

            private void computeFieldSize() {
                Dimension d = this.getSize();
                int boardSize = (int) (d.getHeight() < d.getWidth() ? d.getHeight() : d.getWidth());
                fieldSize = boardSize / game.getBoard().getSize();
            }

            @Override
            public void paint(Graphics g) {
                Board board = game.getBoard();
                if (board != null) {
                    this.computeFieldSize();
                    this.center();
                    int size = board.getSize() * fieldSize;
                    for (int i = 0; i < size; i += fieldSize) {
                        for (int j = 0; j < size; j += fieldSize) {
                            paintField(i, j, g);
                        }
                        this.whiteField = !this.whiteField;
                    }
                    for (int i = 1; i <= board.getSize(); i++) {
                        for (int j = 1; j <= board.getSize(); j++) {
                            paintDisk(i, j, g);
                        }
                    }
                }
            }
        };

        gameDesk.addMouseListener(new MouseListener() {
            @Override
            public void mouseClicked(MouseEvent e) {
                // x and y must be changed, internally x is row, y is line
                // but mousevent x is line and y is row
                int x = e.getY() / fieldSize + 1;
                int y = e.getX() / fieldSize + 1;

                if (game.checkMove(x, y))
                    game.move(x, y);
                else {
                    MainWindow.this.updateInfoField("Illegal move " + x + " " + y);
                }
            }

            @Override
            public void mousePressed(MouseEvent e) {

            }

            @Override
            public void mouseReleased(MouseEvent e) {

            }

            @Override
            public void mouseEntered(MouseEvent e) {

            }

            @Override
            public void mouseExited(MouseEvent e) {

            }
        });
    }
}
