/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi ReversiRules :
 *  Trida, ktera reprezentuje aktivni pole hraci desky.
 */

package ija.ija2016.othello.game;

import ija.ija2016.othello.board.BorderField;
import ija.ija2016.othello.board.Field;
import ija.ija2016.othello.board.Rules;

public class ReversiRules implements Rules{
    private int size;

    public ReversiRules(int size) {
        this.size = size;
    }

    @Override
    public Field createField(int row, int col) {
        if(row == 0 || col == 0 || row == size + 1 || col == size + 1)
            return new BorderField();
        else
            return new BoardField(row,col);
    }

    @Override
    public int getSize() {
        return size;
    }

    @Override
    public int numberDisks() {
        return size*size  / 2;
    }
}
