/* CleanShutdown.java, see http://ultramessaging.github.io/UMExamples/clean_shutdown/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;

public class CleanShutdown {
	public static int cleanup = 0;

	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		LBMEventQueue evq = new LBMEventQueue();

		/* Initialization: create necessary UM objects. */
		try {
			LBMContextAttributes ctxAttr = new LBMContextAttributes();
			ctx = new LBMContext(ctxAttr);
		}
		catch(LBMException ex)
		{
			System.err.println("Error initializing LBM objects: " + ex.toString());
			System.exit(1);
		}

		/* Initialize and create receiver and receiver callback */
		ReceiverCallback myReceiverCallback = new ReceiverCallback();

		LBMReceiverAttributes rcv_attr = new LBMReceiverAttributes();
		LBMTopic rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
		LBMReceiver rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, evq);

		/* Initialize and create source */
		LBMTopic stopic = new LBMTopic(ctx, "test.topic", new LBMSourceAttributes());
		LBMSource src = new LBMSource(ctx, stopic);

		EQThread evqThread = new EQThread(evq);
		evqThread.start();

		try {
			Thread.sleep(1000);
		}
		catch(InterruptedException tex) {
			System.err.println("Error Thread.sleep interrupted: " + tex.toString());
			System.exit(1);
		}
		
		src.send("hello".getBytes(), 5, LBM.MSG_FLUSH);

		while (cleanup == 0)
		{
			try {
                        	Thread.sleep(1000);
                	}
                	catch(InterruptedException tex) {
                	        System.err.println("Error Thread.sleep interrupted: " + tex.toString());
                	        System.exit(1);
                	}
		}

		evq.stop();
		evqThread.join();

		src.close();
		rcv.close();
		ctx.close();
		evq.close();

	} /* main */

	/* Class for receiving messages */
	private static class ReceiverCallback implements LBMReceiverCallback {
		public int onReceive(Object cbArg, LBMMessage msg)
		{
			switch (msg.type())
			{
				case LBM.MSG_DATA:
					System.out.println("LBM_MSG_DATA received");
					cleanup = 1;
					break;
			}
			return 0;
		}
	}  /* ReceiverCallback */

	public static class EQThread implements Runnable
	{
		private LBMEventQueue _evq;
		private Thread myThread;

		public EQThread(LBMEventQueue evq)
		{
			_evq = evq;
		}

		public void start()
		{
			myThread = new Thread(this);
			myThread.start();
		}

		public void join()
		{
			if (myThread != null)
			{
				try {
					myThread.join();
				}
				catch (Exception e)
				{
					System.err.println("lbmmsrc: error\n" + e);
				}
			}
		}

		public void run()
		{
			while (cleanup == 0)
			{
				_evq.run(1000);
			}
		}
	}
}
