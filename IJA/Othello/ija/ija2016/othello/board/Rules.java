/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis rozhrani Rules :
 *  Rozhrani, ktere reprezentuje pravidla inicializace hry.
 */

package ija.ija2016.othello.board;

import java.io.Serializable;

public interface Rules extends Serializable{
    Field createField(int row, int col);
    int getSize();
    int numberDisks();
}
