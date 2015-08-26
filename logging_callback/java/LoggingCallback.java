/* LoggingCallback.java, see http://ultramessaging.github.io/UMExamples/logging_callback/java/ */

import com.latencybusters.lbm.*;
import java.lang.*;

public class LoggingCallback {
	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		LBMSource src = null; /* Source object: for sending messages. */
		LBM lbm = new LBM();

		/*** Initialization: create necessary UM objects. ***/
		try {
			LBMTopic topic = null;
			LBMSourceAttributes srcAttr = null;

			LogCB logger = new LogCB();
			lbm.setLogger(logger);

			ctx = new LBMContext();
			srcAttr = new LBMSourceAttributes();
			srcAttr.setValue("late_join", "1");
			topic = ctx.allocTopic("test.topic",srcAttr);
			src = ctx.createSource(topic, null, null, null);
		}
		catch(LBMException ex)
		{
			System.err.println("Error initializing LBM objects: " + ex.toString());
			System.exit(1);
		}

		/* Cleanup */
		src.close();
		ctx.close();
} /* main */
	private static class LogCB implements LBMLogging
	{
		public void LBMLog(int level, java.lang.String message)
		{
			System.out.println("LOG Level " + level + ": " + message);
		}
	}
} /* class LoggingCallback */
