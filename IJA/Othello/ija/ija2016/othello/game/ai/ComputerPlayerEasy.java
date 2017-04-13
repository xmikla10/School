/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi ComputerPlayerEasy :
 *  Trida impementujici algoritmus pre hru s pocitacem s lehci obtiznosti
 */

package ija.ija2016.othello.game.ai;

import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.board.Field;
import ija.ija2016.othello.game.AbstractPlayer;
import java.util.List;
import static java.util.stream.Collectors.toList;

public class ComputerPlayerEasy extends AbstractPlayer implements AI  {

    public ComputerPlayerEasy(boolean isWhite){
        super(isWhite);
    }

    @Override
    public void nextMove(Board board) {
        List<Field> f = board.getGameFields().stream().
                filter(super::canPutDisk)
                .collect(toList());

        super.putDisk(f
                .get(0));
    }
}
