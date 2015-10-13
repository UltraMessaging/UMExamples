/* SeqNumberCallback.cs, see http://ultramessaging.github.io/UMExamples/seq_number_callback/dotnet/ */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using com.latencybusters.lbm;

public class SeqNumberCallback {
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

                UMERcvRecInfo umerecinfocb = new UMERcvRecInfo();
                rcv_attr.setRecoverySequenceNumberCallback(umerecinfocb.setRecoverySequenceNumberInfo, null);

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
                        default:
                                System.Console.Out.WriteLine("Other event, type=" + msg.type());
                                break;
                }

                return 0;
        }

        class UMERcvRecInfo {
                public int setRecoverySequenceNumberInfo(Object cbArg, UMERecoverySequenceNumberCallbackInfo cbInfo) {

                        long new_low = 100;

                        System.Console.Out.WriteLine("lowSequenceNumber["+cbInfo.lowSequenceNumber()+"] is the starting sequence number as determined by registration consensus.");
                        System.Console.Out.WriteLine("highSequenceNumber["+cbInfo.highSequenceNumber()+"] is the highest sequence number available in the persisted stores.");
                        System.Console.Out.WriteLine("lowRxReqMaxSequenceNumber["+cbInfo.lowRxReqMaxSequenceNumber()+"] is the lowest sequence number to be requested.");
                        
                        cbInfo.setLowSequenceNumber((uint)new_low);
                        System.Console.Out.Flush();
                        return 0;
                }
        }


} /* class MinSrc */
