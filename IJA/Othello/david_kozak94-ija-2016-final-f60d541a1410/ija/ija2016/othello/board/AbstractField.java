/**
 *  @file
 *	@author xkozak15
 *	@author xmikla10	
 * 	@date 23 Apr 2016		
 *	
 *  Popis rozhrani AbstractField :
 *	Abstraktni trida reprezentujici obecne herni pole,
 *	obsahuje metody pro praci s hernim polem pro lubovolnou deskovou hru.
 */

package ija.ija2016.othello.board;

import java.util.HashMap;

public abstract class AbstractField implements Field {
    protected final int row, col;
    private HashMap<Direction, Field> neighbours = new HashMap<>();
    protected Disk disk = null;

    public AbstractField(int row, int col) {
        this.row = row;
        this.col = col;
    }

    @Override
    public void addNextField(Direction dirs, Field field) {
        neighbours.put(dirs, field);
    }

    @Override
    public Field nextField(Direction dirs) {
        return neighbours.get(dirs);
    }

    @Override
    public Disk getDisk() {
        return disk;
    }

    @Override
    public boolean isEmpty() {
        return disk == null;
    }


    @Override
    public void removeDisk() {
        this.disk = null;
    }



    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        AbstractField that = (AbstractField) o;
        return row == that.row && col == that.col;
    }

    @Override
    public int hashCode() {
        int result = row;
        result = 31 * result + col;
        return result;
    }

    @Override
    public String toString() {
        return isEmpty() ? " " : getDisk().isWhite() ? "W" : "B";
    }

    public int getRow() {
        return row;
    }

    public int getCol() {
        return col;
    }
}
