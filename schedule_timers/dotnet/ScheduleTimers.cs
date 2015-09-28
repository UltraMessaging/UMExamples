/* ScheduleTimers.cs, see http://ultramessaging.github.io/UMExamples/schedule_timers/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class ScheduleTimers {
        public static int wait = 1;

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

                TimerCallback cb = new TimerCallback();
                LBMTimer timer = new LBMTimer(ctx,1000,(LBMTimerCallback)cb.onExpiration,null);

                while (wait == 1)
                {
                        try {
                                System.Threading.Thread.Sleep(500);
                        } 
                        catch (Exception eex)
                        {
                            System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + eex.Message);
                            System.Environment.Exit(1);
                        }
                }

                ctx.close();
        } /* main */

        public void onExpiration(Object cbArg)
        {
            System.Console.Out.WriteLine("Timer executed. Set wait to 0 so application can cleanly exit");
            wait = 0;
        }

        public class TimerCallback 
        {
                public void onExpiration(Object cbArg)
                {
                    System.Console.Out.WriteLine("Timer executed. Set wait to 0 so application can cleanly exit");
                    wait = 0;
                }
        }
} /* class ScheduleTimers */
