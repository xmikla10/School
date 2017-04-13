/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis rozhrani Field :
 *  Rozhrani k polim, ktore lze umistnit na hraci desku.
 */

package ija.ija2016.othello.board;

import java.io.Serializable;

public interface Field extends GameMoveLogic,Serializable {
    enum Direction {
        D, L, LD, LU, R, RD, RU, U
    }

    void addNextField(Direction dirs,Field field);
    boolean canPutDisk(Disk disk);
    Disk getDisk();
    boolean isEmpty();
    Field nextField(Direction dirs);
    boolean putDisk(Disk disk);
    void removeDisk();
}
