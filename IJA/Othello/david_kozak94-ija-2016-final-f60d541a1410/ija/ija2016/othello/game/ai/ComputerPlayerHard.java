/**
 *  @file
 *	@author xkozak15
 *	@author xmikla10	
 * 	@date 23 Apr 2016		
 *	
 *  Popis tridi ComputerPlayerHard :
 *  Trida impementujici algoritmus pre hru s pocitacem s tezsi obtiznosti
 */

package ija.ija2016.othello.game.ai;

import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.game.IOthelloField;
import ija.ija2016.othello.game.AbstractPlayer;

public class ComputerPlayerHard extends AbstractPlayer implements AI {

    public ComputerPlayerHard(boolean isWhite) {
        super(isWhite);
    }


    @Override
    public void nextMove(Board board) {
        IOthelloField f = board.getGameFields().stream().filter(super::canPutDisk).max((x,y)->
                Integer.compare(x.countChangedDisks(isWhite),y.countChangedDisks(isWhite))).get();

        super.putDisk(f);

    }
}
