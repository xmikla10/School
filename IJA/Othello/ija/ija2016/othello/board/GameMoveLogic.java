/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis rozhrani GameMoveLogic :
 *  Rozhrani, ktere reprezentuje logiku tahu.
 */

package ija.ija2016.othello.board;

import java.util.List;

/**
 * Rozhrani obsahujici metody tykajici se mechanismu vkladani hracich kamenu
 */
public interface GameMoveLogic {
    /**
     * Metoda overi, zda je na danou pozici moznost vlozit kamen
     * @param dirs - smer, jakym z daneho pole ma prohledavani jit dal
     * @return uspech / neuspech
     */
    boolean checkMove(Field.Direction dirs,boolean isWhite, List<Field> stonesToChange);

    /**
     * Metoda obsahujici cinnost vykonanou pred vyvolanim checkMove nad dalsim objektem v rade
     * @return
     */
    void preTest();

    /**
     * Metoda obsahujici cinnost vyvolanou po skonceni metody checkMove nad dalsim objektem v rade
     * @return
     */
    void postTest();
}
