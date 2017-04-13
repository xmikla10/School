/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi AbstractPlayer :
 *  Abstraktni trida implementujici metody spolecne pro lidske hrace i pocitac.
 */

package ija.ija2016.othello.game;

import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.board.Disk;
import ija.ija2016.othello.board.Field;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.EmptyStackException;
import java.util.List;
import java.util.Stack;
import static java.util.stream.Collectors.toList;

public abstract class AbstractPlayer implements Serializable{
    protected final boolean isWhite;
    private final Stack<Turn> oldTurns;

    public AbstractPlayer(boolean isWhite){
        this.isWhite = isWhite;
        oldTurns = new Stack<>();
    }

    public boolean isWhite() {
        return isWhite;
    }

    public void init(Board board) {
        int size = board.getSize();
        if (isWhite) {
            board.getField(size / 2, size / 2).putDisk(new Disk(true));
            board.getField(size / 2 + 1, size / 2 + 1).putDisk(new Disk(true));
        } else {
            board.getField(size / 2, size / 2 + 1).putDisk(new Disk(false));
            board.getField(size / 2 + 1, size / 2).putDisk(new Disk(false));
        }
    }

    @Override
    public String toString() {
        if (isWhite)
            return "white";
        else
            return "black";
    }

    public void revertLastTurn() throws EmptyStackException {
        Turn turn = oldTurns.pop();
        turn.getChangesStones().stream().forEach(x->x.forceGetDisk().turn());
        turn.getNewStoneField().removeDisk();
    }

    public boolean canPutDisk(Field field) {
        if (field == null || !field.isEmpty())
            return false;
        List<Field> l = new ArrayList<>();

        for (Field.Direction dir : Field.Direction.values()) {
            Field f = field.nextField(dir);
            if (f != null) {
                if (f.checkMove(dir, isWhite, l))
                    return true;
            }

        }
        return false;
    }

    public boolean putDisk(Field field) {
        List<Field> l = new ArrayList<>();
        List<Field> res = new ArrayList<>();


        for (Field.Direction dir : Field.Direction.values()) {
            Field f = field.nextField(dir);
            if (f != null) {
                f.checkMove(dir, isWhite, l);
            }
            res.addAll(l);
            l.clear();
        }
        if (res.isEmpty())
            return false;

        res.stream().forEach(x->x.getDisk().turn());
        oldTurns.push(new Turn(res.stream().map(x->(IOthelloField)x).collect(toList()),field));
        field.putDisk(new Disk(isWhite));
        return true;
    }

    private final class Turn implements Serializable {
        private final Field newStoneField;
        private final List<IOthelloField> changesStones;

        public Turn(List<IOthelloField> changesStones,Field f) {
            this.changesStones = changesStones;
            newStoneField = f;
        }

        public List<IOthelloField> getChangesStones() {
            return changesStones;
        }

        public Field getNewStoneField() {
            return newStoneField;
        }
    }
}
