/* SrcNotifyCbs.cs, see http://ultramessaging.github.io/UMExamples/seq_number_callback/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class SrcNotifyCbs {
        static void Main(String[] args) {
                LBMContext ctx = null;           /* Context object: container for UM "instance". */
                LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
                LBMTopic rtopic = null;          /* Topic object */
                LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */
                LBM.setLicenseFile("lbm6.lic");
                /* Initialize and create receiver and receiver callback */
                LBMReceiverCallback myReceiverCallback = new LBMReceiverCallback(onReceive);
                LBMSourceCreationCallback mySourceCreate = new LBMSourceCreationCallback(onNewSource);
                LBMSourceDeletionCallback mySourceDelete = new LBMSourceDeletionCallback(onSourceDelete);

                /* Initialization: create necessary UM objects. */
                ctx = new LBMContext();
                rcv_attr = new LBMReceiverAttributes();

                rcv_attr.setSourceNotificationCallbacks(mySourceCreate, mySourceDelete, null);

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
                case LBM.MSG_BOS:
                        System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
                        break;
                case LBM.MSG_EOS:
                        System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
                        break;
                default:
                        System.Console.Out.WriteLine("Other event, type=" + msg.type());
                        break;
                }
                return 0;
        }

        /* Class for delivery controller create callback */
        public static Object onNewSource(String source, Object cbObj) {
                System.Console.Out.WriteLine("Delivery Controller Created: " + source);
                return 0;
        }  /* Delivery Controller Create */

        /* Class for delivery controller delete callback */
        public static int onSourceDelete(String source, Object cbObj, Object sourceCbObj) {
                System.Console.Out.WriteLine("Delivery Controller Deleted: " + source);
                return 0;
        }  /* Delivery Controller Delete */
} /* class srcNotifyCbs */
