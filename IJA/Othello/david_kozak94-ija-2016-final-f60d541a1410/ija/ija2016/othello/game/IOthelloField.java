/**
 *  @file
 *	@author xkozak15
 *	@author xmikla10	
 * 	@date 23 Apr 2016		
 *	
 *  Popis rozhrani IOthelloField :
 *	Rozhrani specifikujici dalsi metody pole vazajici se ciste na hru Othello.
 */

package ija.ija2016.othello.game;

import ija.ija2016.othello.board.Disk;
import ija.ija2016.othello.board.Field;

public interface IOthelloField extends Field {
    int countChangedDisks(Boolean isWhite);

    boolean isFrozen();
    void setFrozen(boolean frozen);
    Disk forceGetDisk();
}
