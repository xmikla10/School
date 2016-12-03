/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi CLI :
 *  Trida implementujici primiticni uzivatelske rozhrani, 
 *  pres kterou je mozne hrat hru v terminalu/prikazove radce.
 */

package ija.ija2016.othello.cli;

import ija.ija2016.othello.board.Board;
import ija.ija2016.othello.board.Field;
import ija.ija2016.othello.game.Game;
import ija.ija2016.othello.game.AbstractPlayer;
import ija.ija2016.othello.game.HumanPlayer;
import ija.ija2016.othello.game.ReversiRules;
import java.util.Scanner;
import static java.lang.System.out;

public class CLI {
    private static final int GAME_SIZE = 8;
    private Game game;

    public static void main(String... args) {
        CLI cli = new CLI();
        cli.run();
    }

    private void run() {
        game = new Game(new Board(new ReversiRules(GAME_SIZE)));
        game.addPlayer(new HumanPlayer(true));
        game.addPlayer(new HumanPlayer(false));

        drawGame();
        while (true) {
            out.print("Prompt>");
            if (getInput())
                drawGame();
            ;
        }
    }

    private void drawGame() {
        System.
                out.println("Reversi, verze 1.0 autori: David Kozak, Peter Miklanek");
        out.print("   ");
        for (int i = 1; i <= GAME_SIZE; i++) {
            System.out.print(i + " ");
        }
        out.print("\n");
        for (int row = 1; row <= GAME_SIZE; row++) {
            out.println("  _________________");
            out.print(row + " ");
            out.print("|");
            for (int col = 1; col <= GAME_SIZE; col++) {
                Field f = game.getBoard().getField(row, col);
                out.print(f.toString() + "|");
            }

            out.print("\n");
        }
        out.println("  _________________");
    }

    private boolean getInput() {
        Scanner scanner = new Scanner(System.in);
        String command = scanner.next();

        if ("q".equals(command)) {
            System.out.println("Bye, bye...");
            System.exit(0);
        } else if ("h".equals(command)) {
            printHelp();
            return false;
        } else if ("r".equals(command)) {
            return game.revertLastTurn();
        } else if ("s".equals(command)) {
            int row, col;
            try {
                row = Integer.parseInt(scanner.next());
                col = Integer.parseInt(scanner.next());

                AbstractPlayer currentPlayer = game.currentPlayer();
                Field f = game.getBoard().getField(row, col);

                if (f != null && currentPlayer.canPutDisk(f)) {
                    currentPlayer.putDisk(f);
                    game.nextPlayer();
                    return true;
                } else {
                    System.out.println("Illegal computeHintMove, please try something else");
                    return false;
                }
            } catch (NumberFormatException e) {
                System.out.println("Command reading failed, please make sure that your command is valid and try again");
                return false;
            }
        } else {
            System.out.println("Unrecognized command, if you need help, use command h");
            return false;
        }
        return false;
    }

    private void printHelp() {
        out.println("Hra Reversi, pravidla naleznete na wikipedii.");
        out.println("Ovladani: ");
        out.println("\t s [1-8] [1-8] Prikaz pro vlozeni kamene na dane souradnice");
        out.println("\t r - vrati zpet posledni tah");
        out.println("\t h - vypise napovedu");
        out.println("\t q - vypne hru");
    }

}
