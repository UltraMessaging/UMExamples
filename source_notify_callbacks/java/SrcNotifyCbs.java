/* SrcNotifyCbs.java, see http://ultramessaging.github.io/UMExamples/seq_number_callback/java/ */
import com.latencybusters.lbm.*;
import java.lang.*;

public class SrcNotifyCbs {
        public static void main(String[] args) throws LBMException {
                LBMContext ctx = null;           /* Context object: container for UM "instance". */
                LBMReceiver rcv = null;          /* Receiver object: for receiving messages. */
                LBMTopic rtopic = null;          /* Topic object */
                LBMReceiverAttributes rcv_attr;  /* Receiver attribute object */

                /* Initialize and create receiver and receiver callback */
                ReceiverCallback myReceiverCallback = new ReceiverCallback();
                CreateSourceCbs mySourceCreate = new CreateSourceCbs();
                DeleteSourceCbs mySourceDelete = new DeleteSourceCbs();

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
        private static class ReceiverCallback implements LBMReceiverCallback {
                public int onReceive(Object cbArg, LBMMessage msg)
                {
                        switch (msg.type())
                        {
                        case LBM.MSG_BOS:
                                System.out.println("[" + msg.topicName() + "][" + msg.source() + "], Beginning of Transport Session");
                                break;
                        case LBM.MSG_EOS:
                                System.out.println("[" + msg.topicName() + "][" + msg.source() + "], End of Transport Session");
                                break;
                        default:
                                System.out.println("Other event, type=" + msg.type());
                                break;
                        }
                        return 0;
                }
        }  /* ReceiverCallback */

        /* Class for delivery controller create callback */
        private static class CreateSourceCbs implements LBMSourceCreationCallback {
                public Object onNewSource(String source, Object cbObj) {
                        System.out.println("Delivery Controller Created: " + source);
                        return 0;
                }
        }  /* Delivery Controller Create */

	/* Class for delivery controller delete callback */
        private static class DeleteSourceCbs implements LBMSourceDeletionCallback {
                public int onSourceDelete(String source, Object cbObj, Object sourceCbObj) {
                        System.out.println("Delivery Controller Deleted: " + source);
                        return 0;
                }
        }  /* Delivery Controller Delete */
} /* class srcNotifyCbs */
