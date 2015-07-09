/* CheckFlightSizeEwouldblock.cs, see http://ultramessaging.github.io/UMExamples/CheckFlightSizeEwouldblock/dotnet/ */

using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;

namespace CheckFlightSizeEwouldblock
{
    class CheckFlightSizeEwouldblock
    {
        private const int maxFlightSize = 500;

        static void Main(string[] args)
        {
            LBMContext ctx = null;  /* Context object: container for UM "instance". */
            LBMSource src = null;   /* Source object: for sending messages. */

            try
            {
                LBMTopic topic = null;
                LBMSourceAttributes srcAttr = null;

                ctx = new LBMContext();
                srcAttr = new LBMSourceAttributes();
                srcAttr.setValue("ume_store", "127.0.0.1:29999");
                srcAttr.setValue("ume_flight_size", maxFlightSize.ToString());
                topic = ctx.allocTopic("test.topic", srcAttr);
                src = new LBMSource(ctx, topic);
            }
            catch (LBMException ex)
            {
                System.Console.Error.WriteLine("Error initializing LBM objects:  " + ex.Message);
                System.Environment.Exit(1);
            }

            try
            {
                System.Threading.Thread.Sleep(3000);
            }
            catch (Exception ex)
            {
                System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + ex.Message);
                System.Environment.Exit(1);
            }

            while (true)
            {
                /*** Send a message. ***/
                try
                {
                    src.send(Encoding.ASCII.GetBytes("test"), 4, LBM.MSG_FLUSH | LBM.SRC_NONBLOCK);
                }
                catch (LBMEWouldBlockException ex)
                {
                    try
                    {
                        int currentFlightSize = src.getInflight(LBM.FLIGHT_SIZE_TYPE_UME);

                        if (currentFlightSize == maxFlightSize)
                        {
                            System.Console.Error.WriteLine("Source is blocked on flight. Exiting");
                            break;
                        }
                    }
                    catch (LBMException exInflight)
                    {
                        System.Console.Error.WriteLine("Error getting flightsize: " + exInflight.Message);
                    }
                }
                catch (LBMException ex)
                {
                    System.Console.Error.WriteLine("Error sending: " + ex.Message);
                }
            }

            /*** Cleanup: delete UM objects. ***/
            try
            {
                System.Threading.Thread.Sleep(3000);
            }
            catch (Exception ex)
            {
                System.Console.Error.WriteLine("Error System.Threading.Thread.Sleep() exception:  " + ex.Message);
                System.Environment.Exit(1);
            }

            try
            {
                src.close();
                ctx.close();
            }
            catch (LBMException ex)
            {
                System.Console.Error.WriteLine("Error closing LBM objects: " + ex.Message);
                System.Environment.Exit(1);
            }

        }  /* Main */
    }  /* class CheckFlightSizeEwouldblock*/
}
