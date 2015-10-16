/* NoSourceNotify.cs, see http://ultramessaging.github.io/UMExamples/no_source_notify/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class NoSourceNotify {
        public static String noSourceQueries = "25";

        public static int noSource = 0;

        static void Main(String[] args) {
                LBMContext ctx = null;           /* Context object: container for UM "instance". */
                LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
                LBMTopic rtopic = null;          /* Topic object */
                LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

                /* Initialize and create receiver and receiver callback */
                LBMReceiverCallback myReceiverCallback = new LBMReceiverCallback(onReceive);

                /*** Initialization: create necessary UM objects. ***/
                ctx = new LBMContext();

                rcv_attr = new LBMReceiverAttributes();
                rcv_attr.setValue("resolution_no_source_notification_threshold", noSourceQueries);

                rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
                rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

                while(noSource == 0) {
                    try
                    {
                        System.Threading.Thread.Sleep(1000);
                    }
                    catch (Exception eex)
                    {
                        System.Console.Error.WriteLine("Error Thread.sleep interrupted: " + eex.Message);
                        System.Environment.Exit(1);
                    }
                }

                rcv.close();
                ctx.close();
        } /* main */

        /* Class for receiving messages */
        static int onReceive(Object cbArg, LBMMessage msg)
        {
                switch (msg.type())
                {
                        case LBM.MSG_NO_SOURCE_NOTIFICATION:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "], no sources found for topic after " + noSourceQueries +" queries");
                                noSource++;
                                break;
                        default:
                                System.Console.Out.WriteLine("Other event, type=" + msg.type());
                                break;
                }
                return 0;
        } /* ReceiverCallback */
} /* class NoSourceNotify */
