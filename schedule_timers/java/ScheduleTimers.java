/* ScheduleTimers.java, see http://ultramessaging.github.io/UMExamples/schedule_timers/java/ */

import com.latencybusters.lbm.*;

public class ScheduleTimers {
	public static int wait = 1;

	public static void main(String[] args) throws LBMException {
		LBMContext ctx = null; /* Context object: container for UM "instance". */
		
		/*** Initialization: create necessary UM objects. ***/
		try {
			ctx = new LBMContext();
		}
		catch(LBMException ex)
		{
			System.err.println("Error initializing LBM objects: " + ex.toString());
			System.exit(1);
		}

		callback cb = new callback();
		LBMTimer timer = new LBMTimer(ctx,1000,cb,null);

		while (wait == 1)
		{
			try {
                		Thread.sleep(500);
                	} catch (InterruptedException e) { }
		}

		ctx.close();
	} /* main */

	private static class callback implements LBMTimerCallback
	{
		public void onExpiration(java.lang.Object cbArg)
		{
			System.out.println("Timer executed. Set wait to 0 so application can cleanly exit");
			wait = 0;
		}
	}

} /* class ScheduleTimers */
