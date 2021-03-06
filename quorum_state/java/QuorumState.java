/* quorum_state.java, see http://ultramessaging.github.io/UMExamples/quorum_state/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;


public class QuorumState {
	public static int srcReady = 0;
	
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		LBMSource src = null; /* Source object: for sending messages. */

		SrcCB srccb = new SrcCB();

		/*** Initialization: create necessary UM objects. ***/
		try {
			LBMTopic topic = null;
			LBMSourceAttributes srcAttr = null;
			
			ctx = new LBMContext();
			srcAttr = new LBMSourceAttributes();
			srcAttr.setValue("ume_store", "127.0.0.1:29999");
			srcAttr.setValue("ume_store_behavior", "qc");
			topic = ctx.allocTopic("test.topic",srcAttr);
			src = ctx.createSource(topic, srccb, null, null);
		}
		catch(LBMException ex)
		{
			System.err.println("Error initializing LBM objects: " + ex.toString());
			System.exit(1);
		}
		
		while(true)
		{
			if (srcReady == 1)
			{
				/*** Send a message. ***/
				try {
					src.send("test".getBytes(), "test".getBytes().length, LBM.MSG_FLUSH | LBM.SRC_NONBLOCK);
				}
				catch(LBMException ex)
				{
					/* Error trying to send, wait 1 second and try again */
					try {
						Thread.sleep(1000);
					}
					catch(InterruptedException tex) {
						System.err.println("Error Thread.sleep interrupted: " + tex.toString());
						System.exit(1);
					}
				}
			}
			else
			{
				/* No quorum, wait 1 second and check again */
				System.out.println("Source is not ready to send (no quorum)");
				try {
					Thread.sleep(1000);
				}
				catch(InterruptedException tex) {
					System.err.println("Error Thread.sleep interrupted: " + tex.toString());
					System.exit(1);
				}
			}
		}
	} /* main */

	private static class SrcCB implements LBMSourceEventCallback
	{
		public int onSourceEvent(Object arg, LBMSourceEvent sourceEvent)
		{
			switch (sourceEvent.type())
			{
				case LBM.SRC_EVENT_UME_REGISTRATION_COMPLETE_EX:
					srcReady = 1;
					break;
				case LBM.SRC_EVENT_UME_STORE_UNRESPONSIVE:
					if (sourceEvent.dataString().contains("quorum lost"))
						srcReady = 0;
					break;
			}
			sourceEvent.dispose();
			return 0;
		}
	}
} /* class QuorumState */
