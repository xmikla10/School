/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi Game :
 *  Trida, ktera reprezentuje hru, spojuje dohromady desku a hrace.
 */

package ija.ija2016.othello.game;

import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.game.ai.AI;
import ija.ija2016.othello.gui.MainWindowCallback;
import java.awt.*;
import java.io.Serializable;
import java.util.EmptyStackException;
import java.util.HashMap;
import java.util.List;
import static java.util.stream.Collectors.toList;

public class Game implements Serializable{
    private Board board;
    private StoneFrozer stoneFrozer;
    private transient MainWindowCallback callback;

    // hashmap to allow further expansion (other players)
    private HashMap<Boolean, AbstractPlayer> players;
    private boolean whiteTurn = true;

    public static final String DEFAULT_NAME = "no_name_lekna";

    private String name;

    public Game(Board board) {
        this.board = board;
        players = new HashMap<>();
    }

    public boolean checkMove(int x, int y) {
        return currentPlayer().canPutDisk(board.getField(x, y));
    }

    public Point computeScore(){
        List<IOthelloField> activeFields = getBoard().
                getGameFields().stream().filter(x -> x.forceGetDisk() != null).collect(toList());

        int size = activeFields.size();
        int whiteStonesCount = (int) activeFields.stream().filter((x) -> x.forceGetDisk().isWhite()).count();
        return new Point(whiteStonesCount,size - whiteStonesCount);
    }

    public void move(int x, int y) {
        currentPlayer().putDisk(board.getField(x, y));
        this.nextPlayer();
        callback.updateScoreField(computeScore());
        callback.updateInfoField("Last move: " + x  + " " + y);
        callback.repaint();

            // check if player can move...
        if (playerLost(currentPlayer().isWhite())) {
            callback.endGame(!currentPlayer().isWhite(),computeScore());
        } else if (board.isFull()) {
           callback.endGame(!currentPlayer().isWhite(),computeScore());
        }
    }

    /***
     * checks wheter player lost(which means that he has not disks on the board)
     *
     * @param isWhite
     * @return boolean true=lost
     */
    private boolean playerLost(boolean isWhite) {
        return board.getGameFields().stream().filter(x -> x.forceGetDisk() != null && x.forceGetDisk().isWhite() == isWhite).count() == 0;
    }

    public boolean addPlayer(AbstractPlayer player) {
        AbstractPlayer p = players.get(player.isWhite());
        if (p != null)
            return false;
        else
            players.put(player.isWhite(), player);
        player.init(board);
        return true;
    }

    public AbstractPlayer currentPlayer() {
        return players.get(whiteTurn);
    }

    public Board getBoard() {
        return board;
    }

    public AbstractPlayer nextPlayer() {
        whiteTurn = !whiteTurn;
        AbstractPlayer newPlayer = players.get(whiteTurn);

        // pokud je druhy hrac pocitac, hned zahrej tah a zmen hrace zpet
        if (newPlayer instanceof AI) {
            ((AI) newPlayer).nextMove(board);
            whiteTurn = !whiteTurn;
        }
        return currentPlayer();
    }

    public boolean revertLastTurn() {
        whiteTurn = !whiteTurn;
        try {
            currentPlayer().revertLastTurn();
            return true;
        } catch (EmptyStackException e) {
            whiteTurn = !whiteTurn; // revert back to the current player
            callback.updateInfoField("There are no moves left to revert, you are at the beginning of the game");
            System.out.println("There are no moves left to revert, you are at the beginning of the game");
            return false;
        }
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public StoneFrozer getStoneFrozer() {
        return stoneFrozer;
    }

    public void setStoneFrozer(StoneFrozer stoneFrozer) {
        this.stoneFrozer = stoneFrozer;
    }

    public void setCallback(MainWindowCallback callback) {
        this.callback = callback;
    }
}
