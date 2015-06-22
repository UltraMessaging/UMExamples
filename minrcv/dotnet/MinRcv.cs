/* MinSrc.cs, see http://ultramessaging.github.io/UMExamples/minsrc/dotnet/ */

using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;

namespace MinSrc {
  class MinSrc {
    static void Main(string[] args) {
      LBMContext ctx = nil;  /* Context object: container for UM "instance". */
      LBMSource src = nil;   /* Source object: for sending messages. */


      /*** Initialization: create necessary UM objects. ***/

      ctx = new LBMContext();

      {
        LBMTopic topic = nil;
        topic = new LBMTopic(ctx, "Greeting");
        src = new LBMSource(ctx, topic);
      }

      System.Threading.Thread.Sleep(3000);


  /*** Send a message. ***/

      src.send(Encoding.ASCII.GetBytes("Hello!"), 7, LBM.MSG_FLUSH | LBM.SRC_BLOCK);


  /*** Cleanup: delete UM objects. ***/

      System.Threading.Thread.Sleep(2000);

      src.close();
      ctx.close();
    }  /* Main */
  }  /* class MinSrc */
}
