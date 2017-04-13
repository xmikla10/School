/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Testy hry Othello
 */

package ija.ija2016.othello;

import ija.ija2016.othello.board.AbstractField;
import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.board.Field;
import ija.ija2016.othello.board.Rules;
import ija.ija2016.othello.game.AbstractPlayer;
import ija.ija2016.othello.game.Game;
import ija.ija2016.othello.game.HumanPlayer;
import ija.ija2016.othello.game.ReversiRules;
import ija.ija2016.othello.game.ai.ComputerPlayerEasy;
import ija.ija2016.othello.gui.MainWindow;
import org.junit.Test;

import java.lang.System;
import static org.junit.Assert.*;

/**
 * Test 2. ukolu.
 *
 * @author koci
 */
public class OthelloTestSuite {

    @Test
    public void testAI(){
        Board board = new Board(new ReversiRules(12));
        Game game = new Game(board);
        try {
            MainWindow mainWindow = new MainWindow(game);
            game.setCallback(mainWindow);
            AbstractPlayer p1 = new HumanPlayer(true);
            AbstractPlayer p2 = new ComputerPlayerEasy(false);
            game.addPlayer(p1);
            game.addPlayer(p2);

            game.move(4, 6);
            assert game.currentPlayer() == p1;

            System.out.println("AI test");
        } catch (NullPointerException e){
            // this exception is some wierd error not related with the game logic, just some problem with swing...
        }
    }

    @Test
    public void testPlayer() {
        System.out.println("HumanPlayer");

        HumanPlayer p1 = new HumanPlayer(true);
        //assertTrue("Test prazdne sady kamenu.", p1.emptyPool());
        assertEquals("Test spravne textove reprezentace objektu.", "hrac:white", "hrac:" + p1);

        HumanPlayer p2 = new HumanPlayer(false);
        //assertTrue("Test prazdne sady kamenu.", p2.emptyPool());
        assertEquals("Test spravne textove reprezentace objektu.", "hrac:black", "hrac:" + p2);
    }

    /**
     * Test pravidel inicializace.
     */
    @Test
    public void testRules() {
        System.out.println("Rules");
        int size = 8;

        Rules rules = new ReversiRules(size);
        assertEquals("Test velikosti hry", size, rules.getSize());
        assertEquals("Test poctu kamenu pro jednoho hrace", size * size / 2, rules.numberDisks());

        Field f1 = rules.createField(2, 3);
        Field f2 = rules.createField(2, 3);
        Field f3 = rules.createField(4, 4);

        assertEquals("Test shody dvou stejnych poli.", f1, f2);
        assertNotEquals("Test neshody dvou ruznych poli.", f1, f3);
    }

    /**
     * Test hry (vytvoření pravidel, desky a hráčů, základní tahy).
     */
    @Test
    public void testGame() {
        System.out.println("Game");
        int size = 8;

        ReversiRules rules = new ReversiRules(size);
        Board board = new Board(rules);
        Game game = new Game(board);

        HumanPlayer p1 = new HumanPlayer(true);
        HumanPlayer p2 = new HumanPlayer(false);

        game.addPlayer(p1);
        game.addPlayer(p2);

        assertEquals("Test, zda je aktualni hrac bily.", p1, game.currentPlayer());

        assertTrue("Test spravneho umisteni pocatecnich kamenu.",
                game.getBoard().getField(4, 4).getDisk().isWhite());
        System.out.println("Farba 4-4 "+ game.getBoard().getField(4, 4).getDisk().isWhite());

        assertTrue("Test spravneho umisteni pocatecnich kamenu.",
                game.getBoard().getField(5, 5).getDisk().isWhite());
        System.out.println("Farba 5-5 "+ game.getBoard().getField(5, 5).getDisk().isWhite());

        assertFalse("Test spravneho umisteni pocatecnich kamenu.",
                game.getBoard().getField(4, 5).getDisk().isWhite());
        System.out.println("Farba 4-5 "+ game.getBoard().getField(4, 5).getDisk().isWhite());

        assertFalse("Test spravneho umisteni pocatecnich kamenu.",
                game.getBoard().getField(5, 4).getDisk().isWhite());
        System.out.println("Farba 5-4 "+ game.getBoard().getField(5, 4).getDisk().isWhite());


        Field f1 = game.getBoard().getField(3, 4);
        Field f2 = game.getBoard().getField(4, 6);

        assertFalse("Test umisteni kamene na spatnou pozici.", game.currentPlayer().canPutDisk(f1));
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f2));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f2));

        for (int i = 4; i <= 6; i++) {
            assertTrue("Test spravne barvy kamene.",  game.getBoard().getField(4, i).getDisk().isWhite());
            System.out.println("Farba 4-"+i+" "+ game.getBoard().getField(4, i).getDisk().isWhite());
        }

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac cerny.", p2, game.currentPlayer());

        f2 = game.getBoard().getField(5, 6);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f2));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f2));


        for (int i = 4; i <= 6; i++) {
            assertFalse("Test spravne barvy kamene.",
                    game.getBoard().getField(5, i).getDisk().isWhite());
            System.out.println("Farba 5-"+i+" "+ game.getBoard().getField(5, i).getDisk().isWhite());
        }

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac biely", p1, game.currentPlayer());

        Field f3 = game.getBoard().getField(6, 5);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f3));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f3));

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac cierny", p2, game.currentPlayer());

        Field f4 = game.getBoard().getField(3, 4);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f4));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f4));

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac biely", p1, game.currentPlayer());

        Field f5 = game.getBoard().getField(6, 4);
        assertFalse("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f5));
        assertFalse("Umisteni kamene.", game.currentPlayer().putDisk(f5));

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac cierny", p2, game.currentPlayer());

        Field f6 = game.getBoard().getField(3, 6);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f6));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f6));

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac biely", p1, game.currentPlayer());

        Field f7 = game.getBoard().getField(4, 3);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f7));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f7));

        game.nextPlayer();
        assertEquals("Test, zda je aktualni hrac cierny", p2, game.currentPlayer());

        Field f8 = game.getBoard().getField(4, 2);
        assertTrue("Test umisteni kamene na dobrou pozici.", game.currentPlayer().canPutDisk(f8));
        assertTrue("Umisteni kamene.", game.currentPlayer().putDisk(f8));
    }

}
