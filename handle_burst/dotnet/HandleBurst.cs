/* HandleBurst.cs, see http://ultramessaging.github.io/UMExamples/handle_burst/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class HandleBurst {
        /* Used to track the last sequence number received */
        public static long lastSqn = 0;

        static void Main(String[] args) {
                LBMContext ctx = null;           /* Context object: container for UM "instance". */
                LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
                LBMTopic rtopic = null;          /* Topic object */
                LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

                /*** Initialization: create necessary UM objects. ***/
                /* Initialize and create receiver and receiver callback */
                LBMReceiverCallback myReceiverCallback = new LBMReceiverCallback(onReceive);

                ctx = new LBMContext();
                rcv_attr = new LBMReceiverAttributes();

                rtopic = new LBMTopic(ctx, "test.topic", rcv_attr);
                rcv = new LBMReceiver(ctx, rtopic, myReceiverCallback, null);

                while(true)
                {

                }
        } /* main */

        /* Class for receiving messages */
        static int onReceive(Object cbArg, LBMMessage msg)
        {
                switch (msg.type())
                {
                        case LBM.MSG_DATA:
                                lastSqn = msg.sequenceNumber();
                                break;
                        case LBM.MSG_UNRECOVERABLE_LOSS:
                                lastSqn = msg.sequenceNumber();
                                break;
                        case LBM.MSG_UNRECOVERABLE_LOSS_BURST:
                                long burst = lastSqn - msg.sequenceNumber();
                                System.Console.Out.WriteLine("WARNING: Burst loss of " + burst + " messages!");
                                lastSqn = msg.sequenceNumber();
                                break;
                        default:
                                System.Console.Out.WriteLine("Other event, type=" + msg.type());
                                break;
                }
                return 0;
        } /* ReceiverCallback */
} /* class MinSrc */
