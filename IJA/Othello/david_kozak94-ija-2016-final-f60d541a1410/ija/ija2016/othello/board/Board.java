/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi Board :
 *  Trida, ktera reprezentuje hraci desku a rozlisuje aktivni a neakticni pole.
 */

package ija.ija2016.othello.board;

import ija.ija2016.othello.game.IOthelloField;
import java.io.Serializable;
import java.util.Arrays;
import java.util.List;
import static java.util.stream.Collectors.toList;

public class Board implements Serializable{
    private final Rules rules;
    private Field[][] fields;

    public Board(Rules rules) {
        this.rules = rules;
        int size = rules.getSize();
        fields = new Field[size + 2][size + 2];

        for (int row = 0; row < size + 2; row++) {
            for (int col = 0; col < size + 2; col++) {
                fields[row][col] = rules.createField(row, col);
            }
        }

        for (int row = 1; row < size + 1; row++) {
            for (int col = 1; col < size + 1; col++) {
                Field f = fields[row][col];

                f.addNextField(Field.Direction.D, fields[row + 1][col]);
                f.addNextField(Field.Direction.L, fields[row][col - 1]);
                f.addNextField(Field.Direction.U, fields[row - 1][col]);
                f.addNextField(Field.Direction.R, fields[row][col + 1]);

                f.addNextField(Field.Direction.LD, fields[row + 1][col - 1]);
                f.addNextField(Field.Direction.LU, fields[row - 1][col - 1]);

                f.addNextField(Field.Direction.RD, fields[row + 1][col + 1]);
                f.addNextField(Field.Direction.RU, fields[row - 1][col + 1]);
            }
        }
    }

    public Field getField(int row, int col) {
        if (row < 1 || row > rules.getSize() || col < 1 || col > rules.getSize())
            return null;
        else
            return fields[row][col];
    }

    public Field[][] getFields() {
        return fields;
    }

    public List<IOthelloField> getGameFields(){
        return Arrays.stream(fields).flatMap(Arrays::stream).filter(
                x -> x instanceof IOthelloField).map(x->(IOthelloField)x).collect(toList());
    }

    public Rules getRules() {
        return rules;
    }

    public int getSize() {
        return rules.getSize();
    }

    public boolean isFull(){
        return this.getGameFields().stream().filter(z -> z.forceGetDisk() != null).count() == this.getSize() * this.getSize();
    }
}
