/* CancellingTimers.cs, see http://ultramessaging.github.io/UMExamples/cancelling_timers/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using com.latencybusters.lbm;

public class CancellingTimers {
        public enum TimerState
        {
                PENDING, FIRED, CANCELLED
        }

        public class AppTimer {
                public LBMTimer timer;
                public Semaphore sync = new Semaphore(0,1);
                public TimerState state;

                public AppTimer(LBMContext ctx, int delay, Callback cb)
                {
                        try {
                                this.timer = new LBMTimer(ctx,delay,cb.onExpiration,(Object)this);
                        }
                        catch(LBMException ex)
                        {
                                System.Console.Error.WriteLine("Error initializing LBMTimer: " + ex.Message);
                                System.Environment.Exit(1);
                        }
                        this.state = TimerState.PENDING;
                }
        }

        static void Main(String[] args) {
                LBMContext ctx = null; /* Context object: container for UM "instance". */

                /*** Initialization: create necessary UM objects. ***/
                try {
                        ctx = new LBMContext();
                }
                catch(LBMException ex)
                {
                        System.Console.Error.WriteLine("Error initializing LBM objects: " + ex.Message);
                        System.Environment.Exit(1);
                }

                Callback cb = new Callback();
                AppTimer timer = new AppTimer(ctx,10000,cb);

                try
                {
                    System.Threading.Thread.Sleep(1000);
                }
                catch (Exception eex)
                {
                    System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + eex.Message);
                    System.Environment.Exit(1);
                }

                if (timer.state == TimerState.PENDING)
                {
                        CancelCallback ccb = new CancelCallback();
                        LBMTimer cancelTimer = new LBMTimer(ctx,0,ccb.onExpiration,(Object)timer);
                        timer.sync.WaitOne();
                }

                ctx.close();
        } /* main */

        public class Callback
        {
                public void onExpiration(Object cbArg)
                {
                        AppTimer temp = (AppTimer) cbArg;
                        if (temp.state != TimerState.PENDING)
                        {
                                System.Console.Error.WriteLine("ERROR: Timer executed and state != PENDING.");
                        }
                        System.Console.Error.WriteLine("ERROR: This timer should have been cancelled by now");
                }
        }

        public class CancelCallback
        {
                public void onExpiration(Object cbArg)
                {
                        AppTimer temp = (AppTimer) cbArg;
                        if (temp.state == TimerState.PENDING)
                        {
                                try {
                                        temp.timer.cancel();
                                }
                                catch(LBMException ex)
                                {
                                    System.Console.Error.WriteLine("Error cancelling LBMTimer: " + ex.Message);
                                    System.Environment.Exit(1);
                                }
                                temp.state = TimerState.CANCELLED;
                                temp.sync.Release();
                        }
                }
        }
}
