/* CleanShutdown.cs, see http://ultramessaging.github.io/UMExamples/clean_shutdown/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using com.latencybusters.lbm;

public class CleanShutdown {
        public static int cleanup = 0;

        static void Main(String[] args){
                LBMContext ctx = null; /* Context object: container for UM "instance". */
                LBMEventQueue evq = new LBMEventQueue();

                /* Initialization: create necessary UM objects. */
                try {
                        LBMContextAttributes ctxAttr = new LBMContextAttributes();
                        ctx = new LBMContext(ctxAttr);
                }
                catch(LBMException ex)
                {
                        System.Console.Error.WriteLine("Error initializing LBM objects: " + ex.Message);
                        System.Environment.Exit(1);
                }

                /* Initialize and create receiver and receiver callback */
                LBMReceiverCallback myReceiverCallback = new LBMReceiverCallback(onReceive);

                LBMReceiverAttributes rcv_attr = new LBMReceiverAttributes();
                LBMTopic rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
                LBMReceiver rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, evq);

                /* Initialize and create source */
                LBMTopic stopic = new LBMTopic(ctx, "test.topic", new LBMSourceAttributes());
                LBMSource src = new LBMSource(ctx, stopic);

                EQThread evqThread = new EQThread(evq);
                evqThread.start();

                try
                {
                    System.Threading.Thread.Sleep(1000);
                }
                catch (Exception eex)
                {
                    System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + eex.Message);
                    System.Environment.Exit(1);
                }

                src.send(Encoding.ASCII.GetBytes("hello"), 5, LBM.MSG_FLUSH);

                while (cleanup == 0)
                {  
                    try
                    {
                        System.Threading.Thread.Sleep(1000);
                    }
                    catch (Exception eex)
                    {
                        System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + eex.Message);
                        System.Environment.Exit(1);
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
        static int onReceive(Object cbArg, LBMMessage msg)
        {
                switch (msg.type())
                {
                        case LBM.MSG_DATA:
                                System.Console.Out.WriteLine("LBM_MSG_DATA received");
                                cleanup = 1;
                                break;
                }
                return 0;
        }

        public class EQThread
        {
                private LBMEventQueue _evq;
                private Thread myThread;

                public EQThread(LBMEventQueue evq)
                {
                        _evq = evq;
                }

                public void start()
                {
                    myThread = new Thread(new ThreadStart(run));
                    myThread.Start();
                }

                public void join()
                {
                        if (myThread != null)
                        {
                                try {
                                        myThread.Join();
                                }
                                catch (Exception e)
                                {
                                        System.Console.Error.WriteLine("CleanShutdown Error:" + e);
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
