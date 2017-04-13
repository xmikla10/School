/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi BoardField :
 *  Trida, ktera reprezentuje akticni pole hraci desky.
 */

package ija.ija2016.othello.game;

import ija.ija2016.othello.board.AbstractField;
import ija.ija2016.othello.board.Disk;
import ija.ija2016.othello.board.Field;
import java.util.ArrayList;
import java.util.List;

public class BoardField extends AbstractField implements IOthelloField {
    private volatile boolean isFrozen = false;

    public BoardField(int row, int col) {
        super(row, col);
    }

    @Override
    public boolean canPutDisk(Disk disk) {
        return this.disk == null;
    }

    @Override
    public boolean putDisk(Disk disk) {
        if (this.disk == null) {
            this.disk = disk;
            return true;
        } else return false;
    }

    @Override
    public int countChangedDisks(Boolean isWhite) {
        if (!this.isEmpty())
            return 0;

        List<Field> l = new ArrayList<>();

        for (Field.Direction dir : Field.Direction.values()) {
            Field f = this.nextField(dir);
            if (f != null) {
                f.checkMove(dir, isWhite, l);
            }
        }
        return l.size();
    }


    @Override
    public boolean checkMove(Direction dirs, boolean isWhite, List<Field> stonesToChange) {

        // pokud na poli neni kamen, neni vkladani timto smerem mozne
        Disk d = getDisk();
        if (d != null) {
            // srovnani barvy kamenu
            if (d.isWhite() != isWhite) {
                stonesToChange.add(this);
                Field f = this.nextField(dirs);
                if (f != null)
                    return f.checkMove(dirs, isWhite, stonesToChange);
                else // pokud na dalsim poli jiz nic neni, nelze timto smerem kameny soupere menit
                {
                    stonesToChange.clear();
                    return false;
                }
            } else // pokud byl nalezen kamen shodne barvy, uspech lze poznat podle toho, zda mezi temito kameny lezi nejake kameny soupere
                return !stonesToChange.isEmpty(); // TODO mozna nebude fungovat pro dalsi smery , pokus uz z predchoziho
        } else {
            stonesToChange.clear();
            return false;
        }
    }

    @Override
    public Disk getDisk() {
        // TODO potencial problem with canPutDisk directly on frozen field
        if (!isFrozen)
            return super.getDisk();
        else return null;
    }

    public boolean isFrozen() {
        return isFrozen;
    }

    public void setFrozen(boolean frozen) {
        isFrozen = frozen;
    }

    @Override
    public void preTest() {
    }

    @Override
    public void postTest() {
    }


    @Override
    public Disk forceGetDisk() {
        return super.getDisk();
    }
}
