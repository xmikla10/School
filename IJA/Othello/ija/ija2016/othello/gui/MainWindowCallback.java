/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis rozhrani MainWindowCallback :
 *  Rozhrani specifikujici metody pro aktualizovani herniho okna pri zmenach stavu hry.
 */

package ija.ija2016.othello.gui;

import java.awt.*;

public interface MainWindowCallback {

    /**
     * end the game
     * @param isWhite indicates winner, true==white
     */
    void endGame(boolean isWhite,Point score);
    void updateInfoField(String s);
    void updateScoreField(Point score);
    void repaint();
}
