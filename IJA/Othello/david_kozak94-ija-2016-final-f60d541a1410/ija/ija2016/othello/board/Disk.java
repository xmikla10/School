/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi Disk :
 *  Trida, ktera reprezentuje kamen hraci desky.
 */

package ija.ija2016.othello.board;

import java.io.Serializable;

public class Disk implements Serializable{
    private boolean isWhite;

    public Disk(boolean isWhite) {
        this.isWhite = isWhite;
    }

    public boolean isWhite() {
        return isWhite;
    }

    public void turn() {
        this.isWhite = !this.isWhite;
    }
}
