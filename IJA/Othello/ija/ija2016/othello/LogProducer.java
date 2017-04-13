/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Trida LogProceducer
 */

package ija.ija2016.othello;

public class LogProducer {
    public static void info(Loggable l,String msg) {
        System.out.println("INFO class=" + l.getTag() + " msg=" + msg);
    }

    public interface Loggable{
        String getTag();
    }
}
