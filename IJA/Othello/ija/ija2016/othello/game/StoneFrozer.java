/**
 *  @file
 *  @author xkozak15
 *  @author xmikla10    
 *  @date 23 Apr 2016       
 *  
 *  Popis tridi StoneFrozer :
 *  Vlakno, ktere implementuje mechanizmus "zamrzavani" hernich kamenu.
 */

package ija.ija2016.othello.game;

import java.io.Serializable;
import java.util.Collections;
import java.util.List;
import java.util.Random;
import static java.util.stream.Collectors.toList;
import ija.ija2016.othello.LogProducer;
import ija.ija2016.othello.LogProducer.*;
import ija.ija2016.othello.gui.MainWindowCallback;

public class StoneFrozer extends Thread implements Serializable, Loggable {
    private static final String TAG = "STONE_FROZER";

    private List<IOthelloField> fields;
    private int threadSleepingTime;
    private int stoneFreezingTime;
    private int stonesToFreeze;
    private transient MainWindowCallback callback;



    public StoneFrozer(MainWindowCallback callback, List<IOthelloField> fields, FreezerInfo freezerInfo) {
        this.fields = fields;
        this.threadSleepingTime = freezerInfo.getThreadSleepingTime();
        this.stoneFreezingTime = freezerInfo.getStoneFreezingTime();
        this.stonesToFreeze = freezerInfo.getStonesToFreeze();
        this.callback = callback;
    }

    @Override
    public void run() {
        Random rand = new Random();
        while (true) {
            try {
                Thread.sleep(rand.nextInt(threadSleepingTime) * 1000);
            } catch (InterruptedException e) {
                LogProducer.info(this,"Stone frozer sleep interrupted...");
                return;
            }
            LogProducer.info(this,"StoneFrozer is waking up...");
            List<IOthelloField> activeFields = fields.stream().filter(x -> x.getDisk() != null).collect(toList());

            int len = stonesToFreeze <= activeFields.size() ? stonesToFreeze : activeFields.size();


            Collections.shuffle(activeFields);
            List<IOthelloField> frozenFields = activeFields.subList(0, len);
            frozenFields.stream().forEach(x -> x.setFrozen(true));
            callback.repaint();
            LogProducer.info(this,"Stone frozer has just frozen " + len + "disks");
            try {
                Thread.sleep(rand.nextInt(stoneFreezingTime) * 1000);
            } catch (InterruptedException e) {
                LogProducer.info(this,"Stone frozer sleep interrupted...");
                return;
            }
            frozenFields.stream().forEach(x -> x.setFrozen(false));
            LogProducer.info(this,len + " disks are free again");
            callback.repaint();

        }
    }

    public void setCallback(MainWindowCallback callback) {
        this.callback = callback;
    }

    @Override
    public String getTag() {
        return TAG;
    }

    public static class FreezerInfo {
        private final int threadSleepingTime;
        private final int stoneFreezingTime;
        private final int stonesToFreeze;

        public FreezerInfo(int threadSleepingTime, int stoneFreezingTime, int stonesToFreeze) {
            this.threadSleepingTime = threadSleepingTime;
            this.stoneFreezingTime = stoneFreezingTime;
            this.stonesToFreeze = stonesToFreeze;
        }

        public int getThreadSleepingTime() {
            return threadSleepingTime;
        }

        public int getStoneFreezingTime() {
            return stoneFreezingTime;
        }

        public int getStonesToFreeze() {
            return stonesToFreeze;
        }
    }
}
