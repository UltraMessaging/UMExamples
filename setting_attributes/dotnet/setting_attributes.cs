using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;

namespace SettingAttributes
{
    class SettingAttributes
    {
        static void Main(string[] args)
        {
            LBMContext ctx = null;      /* Container for UM instance */
            LBMTopic topic1 = null;		/* Object used to create topic */
            LBMTopic topic2 = null;	    /* Object used to create topic */
            LBMSource src1 = null;      /* Used for sending messages on a single topic */
            LBMSource src2 = null;      /* Used for sending messages on a single topic */
            LBMSourceAttributes srcAttr = null; 	/* Object used to configure sources */
            int desPort;

            /* We are using certain lbm objects that require an application level 	*/
            /* memory manager called the LBMObjectRecycler to cleanup 				*/
            LBMObjectRecycler objRec = new LBMObjectRecycler();

            try
            {
                /* create ctxAttr to set config values before creating context */
                LBMContextAttributes ctxAttr = new LBMContextAttributes();
                ctxAttr.setObjectRecycler(objRec, null);
                /* Modify resolver address by setting attributes */
                ctxAttr.setValue("resolver_multicast_address", "224.10.11.12");
                /* Create a context with default attributes */
                ctx = new LBMContext(ctxAttr);
                /* Create source attributes object, used to configure sources */
                srcAttr = new LBMSourceAttributes();
                srcAttr.setObjectRecycler(objRec, null);
                /* Set configuration value using strings */
                srcAttr.setValue("transport", "lbtrm");

                /* The Java API only accepts string values. ints, for example */
                /* must be converted to strings */
                desPort = 14001;
                srcAttr.setValue("transport_lbtrm_destination_port", desPort.ToString());

                /* Create topic for the first source with configured attributes */
                topic1 = new LBMTopic(ctx, "test.topic1", srcAttr);
                src1 = new LBMSource(ctx, topic1);
                /* Modify Configuration for second topic to use a new port */
                desPort = 14002;
                srcAttr.setValue("transport_lbtrm_destination_port", desPort.ToString());

                /* Create second topic and source using modified configuration values */
                topic2 = new LBMTopic(ctx, "test.topic2", srcAttr);
                src2 = new LBMSource(ctx, topic2);
            }
            catch (LBMException ex)
            {
                System.Console.Error.WriteLine("Error initializing LBM objects: " + ex.Message);
                System.Environment.Exit(1);
            }

            /* Cleanup all LBM objects */
            try
            {
                objRec.close();
                src2.close();
                src1.close();
                ctx.close();
            }
            catch (LBMException ex)
            {
                System.Console.Error.WriteLine("Error closing LBM objects: " + ex.Message);
                System.Environment.Exit(1);
            }
        }
    }
}
