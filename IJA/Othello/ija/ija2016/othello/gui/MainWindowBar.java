/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi MainWindowBar :
 *  Horni lista obsahujici prikazy pro nacteni, ulozeni, ukonceni hry a podobne.
 */

package ija.ija2016.othello.gui;

import ija.ija2016.othello.IOmanager;
import ija.ija2016.othello.game.Game;
import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.util.List;

public class MainWindowBar extends JMenuBar {

    public MainWindowBar(Game game) {
        JMenu menu = new JMenu("Games");
        menu.setMnemonic(KeyEvent.VK_N);
        this.add(menu);

        JMenuItem item = new JMenuItem("New game");
        item.setMnemonic(KeyEvent.VK_N);
        item.addActionListener(x->new NewGameDialog());
        menu.add(item);

        item = new JMenuItem("Save game");
        item.setMnemonic(KeyEvent.VK_N);
        item.addActionListener(x -> {
            try {
                IOmanager.saveGame(game);
            } catch (IOException e) {
                e.printStackTrace();
                System.exit(1);
            }
        });
        //item.addActionListener(new SaveGameListener());
        menu.add(item);

        item = new JMenuItem("Load game");
        item.setMnemonic(KeyEvent.VK_N);
        item.addActionListener(x->{
            List<String> savedGames = IOmanager.getSavedGames();
            if(!savedGames.isEmpty()){
                new LoadGameDialog(savedGames);
            } else{
                new GenericDialog("Info","No saved games found");
            }
        });
        //item.addActionListener(new LoadGameListener());
        menu.add(item);

        item = new JMenuItem("Settings");
        item.setMnemonic(KeyEvent.VK_N);
        menu.add(item);

        item = new JMenuItem("Exit");
        item.setMnemonic(KeyEvent.VK_N);
        item.addActionListener((x)-> System.exit(0));
        menu.add(item);


        menu = new JMenu("Help");
        menu.setMnemonic(KeyEvent.VK_N);
        this.add(menu);

        item = new JMenuItem("Instructions");
        menu.add(item);
        item = new JMenuItem("Authors");
        menu.add(item);
    }
}