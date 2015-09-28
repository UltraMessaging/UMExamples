/* CancellingTimers.java, see http://ultramessaging.github.io/UMExamples/cancelling_timers/java/ */

import com.latencybusters.lbm.*;
import java.util.concurrent.Semaphore;

public class CancellingTimers {
	public enum TimerState 
	{ 
		PENDING, FIRED, CANCELLED 
	}

	private static class AppTimer {	
		LBMTimer timer;
		public Semaphore sync = new Semaphore(1);
		TimerState state;
		
		public AppTimer(LBMContext ctx, int delay, Callback cb)
		{
			try {
				this.timer = new LBMTimer(ctx,delay,cb,(java.lang.Object)this);
			} 
			catch(LBMException ex) 
			{ 
				System.err.println("Error initializing LBMTimer: " + ex.toString());
                        	System.exit(1);
			}
			this.state = TimerState.PENDING;
		}
	}

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

		Callback cb = new Callback();
		AppTimer timer = new AppTimer(ctx,10000,cb);

		try {
                	Thread.sleep(1000);
                } catch (InterruptedException e) { }

		if (timer.state == TimerState.PENDING)
		{
			CancelCallback ccb = new CancelCallback();
			LBMTimer cancelTimer = new LBMTimer(ctx,0,ccb,(java.lang.Object)timer);
			try {
				timer.sync.acquire();
			} catch (InterruptedException e) { }
		}

		ctx.close();
	} /* main */

	private static class Callback implements LBMTimerCallback
	{
		public void onExpiration(java.lang.Object cbArg)
		{
			AppTimer temp = (AppTimer) cbArg;
			if (temp.state != TimerState.PENDING)
			{
				System.out.println("ERROR: Timer executed and state != PENDING.");
			}
			System.out.println("ERROR: This timer should have been cancelled by now");
		}
	}

        private static class CancelCallback implements LBMTimerCallback
        {
                public void onExpiration(java.lang.Object cbArg)
                {
			AppTimer temp = (AppTimer) cbArg;
			if (temp.state == TimerState.PENDING)
			{
				try {
					temp.timer.cancel();
				}
				catch(LBMException ex)
                        	{
                                	System.err.println("Error cancelling LBMTimer: " + ex.toString());
					System.exit(1);
                          	}
				temp.state = TimerState.CANCELLED;
				temp.sync.release();
			}
                }
        }
} /* class CancellingTimers */
