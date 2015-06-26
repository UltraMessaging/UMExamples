/* MinSrc.java, see http://ultramessaging.github.io/UMExamples/minsrc/java/ */

import com.latencybusters.lbm.*;

public class request_response {
	public static int run = 1;
	
	public static void main(String[] args) throws LBMException
	{
		/* Initialization: create necessary UM objects. */
		LBMContext ctx = new LBMContext();

		/* Initialize and create receiver and receiver callback */
		ReceiverCallback myReceiverCallback = new ReceiverCallback();

		LBMReceiverAttributes rcv_attr = new LBMReceiverAttributes();
		LBMTopic rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
		LBMReceiver rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

		/* Initialize and create source */
		LBMTopic stopic = new LBMTopic(ctx, "test.topic", new LBMSourceAttributes());
		LBMSource src = new LBMSource(ctx, stopic);

		LBMRequest req = new LBMRequest("Request".getBytes(), "Request".length());
		ResponseCallback myResponseCallback = new ResponseCallback();
		req.addResponseCallback(myResponseCallback);

		/* Sleep for 1 second to allow TR to complete */
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) { }

		/* Send a message. */
		src.send(req, LBM.MSG_FLUSH | LBM.SRC_BLOCK);

		while (run == 1) {
			try {
				Thread.sleep(1000);
                	} catch (InterruptedException e) { }
		}

		/* Cleanup: delete UM objects. */
		req.close();
		src.close();
		rcv.close();
		ctx.close();
	}  /* main */
	
	/* Class for receiving messages */
	private static class ReceiverCallback implements LBMReceiverCallback {
        	public int onReceive(Object cbArg, LBMMessage msg)
        	{
                	switch (msg.type())
                	{
                	        case LBM.MSG_REQUEST:
                	                System.out.println("Request Received");
					try {
						msg.respond("Response".getBytes(), "Response".length(), LBM.SRC_NONBLOCK);
					}
					catch (LBMException ex)
					{
						System.err.println("Error responding to request: " + ex.toString());
					}
					break;
                	}
                	return 0;
        	}
	}  /* ReceiverCallback */

	/* Class for receiving responses */
	private static class ResponseCallback implements LBMResponseCallback {
		public int onResponse(Object cbArg, LBMRequest req, LBMMessage msg)
		{
			switch (msg.type())
			{
				case LBM.MSG_RESPONSE:
					System.out.println("Response Received");
					run = 0;
					break;
			}
			return 0;
		}
	} /* ResponseCallback */
}  /* class MinSrc */
