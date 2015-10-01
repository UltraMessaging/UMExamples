/* SequentialMode.cs, see http://ultramessaging.github.io/UMExamples/sequential_mode/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using com.latencybusters.lbm;

public class SequentialMode {
        static void Main(String[] args){
                LBMContext ctx = null; /* Context object: container for UM "instance". */

                /* Initialization: create necessary UM objects. */
                try {
                        LBMContextAttributes ctxAttr = new LBMContextAttributes();
                        ctxAttr.setValue("operational_mode", "sequential");
                        ctx = new LBMContext(ctxAttr);
                }
                catch(LBMException ex)
                {
                        System.Console.Error.WriteLine("Error initializing LBM objects: " + ex.Message);
                        System.Environment.Exit(1);
                }

                SeqThread ThreadHandler = new SeqThread(ctx);
                ThreadHandler.start();

                while(true)
                {
                        try {
                            System.Threading.Thread.Sleep(1000);
                        }
                        catch (Exception eex) {
                            System.Console.Error.WriteLine("Error Thread.sleep interrupted: " + eex.Message);
                            System.Environment.Exit(1);
                        }
                }
        } /* main */

        public class SeqThread
        {
                private LBMContext _ctx;
                private Thread myThread;

                public SeqThread(LBMContext ctx)
                {
                        _ctx = ctx;
                }

                public void start()
                {
                    myThread = new Thread(new ThreadStart(run));
                    myThread.Start();
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
                                    System.Console.Error.WriteLine("Error processing context events: " + ex.Message);
                                    System.Environment.Exit(1);
                                }
                        }
                }
        }
}
