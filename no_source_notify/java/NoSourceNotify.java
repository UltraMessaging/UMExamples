/* NoSourceNotify.java, see http://ultramessaging.github.io/UMExamples/no_source_notify/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;


public class NoSourceNotify {
	public static String noSourceQueries = "25";

	public static int noSource = 0;

	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null;           /* Context object: container for UM "instance". */
		LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
		LBMTopic rtopic = null;          /* Topic object */
		LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

		/* Initialize and create receiver and receiver callback */
		ReceiverCallback myReceiverCallback = new ReceiverCallback();

		/*** Initialization: create necessary UM objects. ***/
		ctx = new LBMContext();
	
		rcv_attr = new LBMReceiverAttributes();
		rcv_attr.setValue("resolution_no_source_notification_threshold", noSourceQueries);

		rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
		rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

		while(noSource == 0) {
			try{
				Thread.sleep(1000);
			}
			catch(InterruptedException ex)
			{
				System.err.println("Error Thread.sleep interrupted: " + ex.toString());
				System.exit(1);
			}
		}
		
		rcv.close();
		ctx.close();
	} /* main */

	/* Class for receiving messages */
	private static class ReceiverCallback implements LBMReceiverCallback {
		public int onReceive(Object cbArg, LBMMessage msg)
		{
			switch (msg.type())
			{
				case LBM.MSG_NO_SOURCE_NOTIFICATION:
					System.out.println("[" + msg.topicName() + "], no sources found for topic after " + noSourceQueries +" queries");
					noSource++;
					break;
				default:
					System.out.println("Other event, type=" + msg.type());
					break;
			}
			return 0;
		}
	}  /* ReceiverCallback */
} /* class NoSourceNotify */
