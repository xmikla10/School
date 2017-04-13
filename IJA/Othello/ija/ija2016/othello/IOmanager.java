/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Trida IOmanager
 */

package ija.ija2016.othello;

import ija.ija2016.othello.game.Game;
import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import static java.util.stream.Collectors.toList;

public class IOmanager {
    public static final String FILE_DIR = "games";
    public static final String FILE_EXTENSION = ".ser";

    public static void saveGame(Game game) throws IOException,SecurityException {
        FileOutputStream file = new FileOutputStream(FILE_DIR + File.separator + game.getName() + FILE_EXTENSION);
        ObjectOutputStream out = new ObjectOutputStream(file);
        out.writeObject(game);
        out.close();
        file.close();
    }

    public static Game loadGame(String name) throws IOException, ClassNotFoundException,SecurityException {
        FileInputStream file = new FileInputStream(FILE_DIR + File.separator + name + FILE_EXTENSION);
        ObjectInputStream in = new ObjectInputStream(file);
        Game game = (Game) in.readObject();
        game.setName(name.replace(FILE_EXTENSION,""));
        in.close();
        file.close();
        return game;
    }

    public static List<String> getSavedGames() throws SecurityException{
        List<String> res = new ArrayList<>();
        File f = new File(FILE_DIR);
        if(!f.exists() || !f.isDirectory())
            return res;

        return Arrays.stream(f.listFiles()).map(File::getName).filter(x->x.endsWith(FILE_EXTENSION)).map(x->x.replace(FILE_EXTENSION,"")).collect(toList());
    }

    public static void prepareDirectoryForSavedGames() throws IOException,SecurityException {
        File f = new File(FILE_DIR);
        if(!f.exists()){
            if(!f.mkdir())
                throw new IOException("Directory was not created successfully");
        } else if(!f.isDirectory()){
            if(!f.delete())
                throw new IOException("File was not deleted successfully");
        }
    }
}
