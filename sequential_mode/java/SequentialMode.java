/* LoggingCallback.java, see http://ultramessaging.github.io/UMExamples/logging_callback/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;

public class SequentialMode {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		
		/* Initialization: create necessary UM objects. */
		try {
			LBMContextAttributes ctxAttr = new LBMContextAttributes();
			ctxAttr.setValue("operational_mode", "sequential");
			ctx = new LBMContext(ctxAttr);
		}
		catch(LBMException ex)
		{
			System.err.println("Error initializing LBM objects: " + ex.toString());
			System.exit(1);
		}

		(new Thread(new SeqThread(ctx))).start();

		while(true) 
		{ 
			try {
				Thread.sleep(1000);
			}
			catch(InterruptedException tex) {
				System.err.println("Error Thread.sleep interrupted: " + tex.toString());
				System.exit(1);
			}
		}
	} /* main */

	public static class SeqThread implements Runnable
	{
		private LBMContext _ctx;

		public SeqThread(LBMContext ctx)
		{
			_ctx = ctx;
		}

		public void run()
		{
			while (true)
			{
				try {
					_ctx.processEvents(500);
				}
				catch(LBMException ex)
				{
					System.err.println("Error processing context events: " + ex.toString());
					System.exit(1);
				}
			}
		}
	}
}
