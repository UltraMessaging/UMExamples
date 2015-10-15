/* UMPReceiverCallbacks.cs, see http://ultramessaging.github.io/UMExamples/ump_receiver_callbacks/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class UMPReceiverCallbacks {
        static void Main(String[] args) {
                LBMContext ctx = null;           /* Context object: container for UM "instance". */
                LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
                LBMTopic rtopic = null;          /* Topic object */

                /* Initialize and create receiver and receiver callback */
                LBMReceiverCallback myReceiverCallback = new LBMReceiverCallback(onReceive);

                /*** Initialization: create necessary UM objects. ***/
                ctx = new LBMContext();

                rtopic = new LBMTopic(ctx, "test.topic");
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
                                System.Console.Out.Write("[" + msg.topicName() + "][" + msg.source() + "][" + msg.sequenceNumber() + "]");
                                if ((msg.flags() & LBM.MSG_FLAG_UME_RETRANSMIT) != 0) {
                                        System.Console.Out.Write("-RX-");
                                }
                                if ((msg.flags() & LBM.MSG_FLAG_OTR) != 0) {
                                        System.Console.Out.Write("-OTR-");
                                }
                                System.Console.Out.Write(", ");
                                System.Console.Out.WriteLine(msg.data().Length + " bytes");
                                break;
                        case LBM.MSG_REQUEST:
                                if ((msg.flags() & LBM.MSG_FLAG_UME_RETRANSMIT) != 0) {
                                        System.Console.Out.Write("-RX-");
                                }
                                if ((msg.flags() & LBM.MSG_FLAG_OTR) != 0) {
                                        System.Console.Out.Write("-OTR-");
                                }
                                System.Console.Out.Write(", ");
                                System.Console.Out.WriteLine(msg.data().Length + " bytes");
                                break;
                        case LBM.MSG_BOS:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
                                break;
                        case LBM.MSG_EOS:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
                                break;
                        case LBM.MSG_UNRECOVERABLE_LOSS:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "][" + msg.sequenceNumber().ToString("x") + "], LOST");
                                break;
                        case LBM.MSG_UNRECOVERABLE_LOSS_BURST:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "][" + msg.sequenceNumber().ToString("x") + "], LOST BURST");
                                break;

                        case LBM.MSG_UME_REGISTRATION_ERROR:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "] UME registration error: " + msg.dataString());
                                break;
                        case LBM.MSG_UME_REGISTRATION_SUCCESS_EX:
                                UMERegistrationSuccessInfo reg = msg.registrationSuccessInfo();
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "] store " + reg.storeIndex() + ": "
                                        + reg.store() + " UME registration successful. Sequence number is " + reg.sequenceNumber());
                                break;
                        case LBM.MSG_UME_REGISTRATION_COMPLETE_EX:
                                UMERegistrationCompleteInfo regcomplete = msg.registrationCompleteInfo();
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "] UME registration complete. SQN "
                                        + regcomplete.sequenceNumber() + ". Flags " + regcomplete.flags());
                                break;
                        case LBM.MSG_UME_DEREGISTRATION_SUCCESS_EX:
                                System.Console.Out.Write("DEREGISTRATION SUCCESSFUL ");
                                UMEDeregistrationSuccessInfo dereg = msg.deregistrationSuccessInfo();
                                System.Console.Out.Write("[" + msg.topicName() + "][" + msg.source() + "] store " + dereg.storeIndex() + ": "
                                        + dereg.store() + " UME deregistration successful. SrcRegID " + dereg.sourceRegistrationId() + " RcvRegID " + dereg.receiverRegistrationId()
                                        + ". Flags " + dereg.flags() + " ");
                                System.Console.Out.WriteLine();
                                break;
                        case LBM.MSG_UME_DEREGISTRATION_COMPLETE_EX:
                                System.Console.Out.WriteLine("DEREGISTRATION COMPLETE");
                                break;
                        case LBM.MSG_UME_REGISTRATION_CHANGE:
                                System.Console.Out.WriteLine("[" + msg.topicName() + "][" + msg.source() + "] UME registration change: " + msg.dataString());
                                break;
                        default:
                                System.Console.Out.WriteLine("Other event, type=" + msg.type());
                                break;
                }
                return 0;
        }
} /* class UMPReceiverCallbacks */
