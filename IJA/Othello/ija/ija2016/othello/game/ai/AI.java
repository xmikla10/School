/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis rozhrani Artifical intelligence :
 *  Funkcionalni rozhrani specifikujici signaturu metody AI pro vypocet tahu.
 */

package ija.ija2016.othello.game.ai;

import ija.ija2016.othello.board.AbstractField;
import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.game.IOthelloField;
import ija.ija2016.othello.game.AbstractPlayer;

public interface AI {
    void nextMove(Board board);

    static AbstractField computeHintMove(AbstractPlayer player, Board board) {
        IOthelloField f = board.getGameFields().stream().
                filter(player::canPutDisk).
                max((x, y) ->
                        Integer.compare(
                                x.countChangedDisks(player.isWhite()),
                                y.countChangedDisks(player.isWhite()))).
                get();

        return (AbstractField) f;
    }
}
