/* HandleBurst.java, see http://ultramessaging.github.io/UMExamples/handle_burst/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;


public class HandleBurst {
	/* Used to track the last sequence number received */
	public static long lastSqn = 0;
	
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null;           /* Context object: container for UM "instance". */
		LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
		LBMTopic rtopic = null;          /* Topic object */
		LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

		/*** Initialization: create necessary UM objects. ***/
		/* Initialize and create receiver and receiver callback */
                ReceiverCallback myReceiverCallback = new ReceiverCallback();

		ctx = new LBMContext();
		rcv_attr = new LBMReceiverAttributes();

		rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
		rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

		while(true)
		{

		}
	} /* main */

	/* Class for receiving messages */
	private static class ReceiverCallback implements LBMReceiverCallback {
		public int onReceive(Object cbArg, LBMMessage msg)
		{
			switch (msg.type())
			{
				case LBM.MSG_DATA:
					lastSqn = msg.sequenceNumber();
					break;
				case LBM.MSG_UNRECOVERABLE_LOSS:
					lastSqn = msg.sequenceNumber();
					break;
				case LBM.MSG_UNRECOVERABLE_LOSS_BURST:
					long burst = lastSqn - msg.sequenceNumber();
					System.out.println("WARNING: Burst loss of " + burst + " messages!");
					lastSqn = msg.sequenceNumber();
					break;
				default:
					System.out.println("Other event, type=" + msg.type());
					break;
			}
			return 0;
		}
	}  /* ReceiverCallback */
} /* class MinSrc */
