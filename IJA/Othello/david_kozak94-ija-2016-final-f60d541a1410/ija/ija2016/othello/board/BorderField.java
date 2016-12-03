/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi BorderdField :
 *  Trida, ktera reprezentuje neaktivni pole hraci desky.
 */

package ija.ija2016.othello.board;

import java.util.List;

public class BorderField implements Field {

    @Override
    public void addNextField(Direction dirs, Field field) {

    }

    @Override
    public boolean canPutDisk(Disk disk) {
        return false;
    }

    @Override
    public Disk getDisk() {
        return null;
    }

    @Override
    public boolean isEmpty() {
        return true;
    }

    @Override
    public Field nextField(Direction dirs) {
        return null;
    }

    @Override
    public boolean putDisk(Disk disk) {
        return false;
    }

    @Override
    public boolean equals(Object obj) {
        return false;
    }

    @Override
    public boolean checkMove(Direction dirs, boolean isWhite, List<Field> stonesToChange) {
        return false;
    }

    @Override
    public void removeDisk() {

    }

    @Override
    public void preTest() {
    }

    @Override
    public void postTest() {
    }
}
