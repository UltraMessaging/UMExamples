/* load_config.cs, see http://ultramessaging.github.io/UMExamples */

using System;
using System.Collections.Generic;
using System.Text;
using com.latencybusters.lbm;
using System.Runtime.InteropServices;

public class RequestResponse
{
	/* Define configuration files */
        public static String flat_conf = "../flat_config.cfg";
        public static String xml_conf = "../xml_config.xml";
        public static String app_name = "processA";

	public static void main(String[] args)
        {
                /* load flat file first */
                try
                {
                        LBM.setConfiguration(flat_conf);
                }
                catch (LBMException Ex)
                {
                        System.Console.Error.WriteLine("Error setting LBM configuration: " + Ex.Message);
                        System.Environment.Exit(1);
                }

                /* then load XML file */
                try
                {
                        LBM.setConfigurationXml(xml_conf, app_name);
                }
                catch (LBMException Ex)
                {
                        System.Console.Error.WriteLine("Error setting LBM configuration: " + ex.Message);
                        System.Environment.Exit(1);
                }
        }
}

