/* minsrc.c, see http://ultramessaging.github.io/UMExamples/minsrc/c/ */

import com.latencybusters.lbm.*;

public class MinSrc {
  public static void main(String[] args) throws LBMException{
    LBMContext ctx = nil;  /* Context object: container for UM "instance". */
    LBMSource src = nil;   /* Source object: for sending messages. */


    /*** Initialization: create necessary UM objects. ***/

    ctx = new LBMContext();

    {
      LBMTopic topic = nil;
      topic = new LBMTopic(ctx, "Greeting");
      src = new LBMSource(ctx, topic);
    }

    Thread.sleep(1000);


  /*** Send a message. ***/

    src.send("Hello!".getBytes(), 7, LBM.MSG_FLUSH | LBM.SRC_BLOCK);
 

  /*** Cleanup: delete UM objects. ***/

    Thread.sleep(3000);

    src.close();
    ctx.close();
  }  /* main */
}  /* class MinSrc */
