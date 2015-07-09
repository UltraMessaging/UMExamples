/* load_config.java, see http://ultramessaging.github.io/UMExamples */

import com.latencybusters.lbm.*;

public class load_config {
	/* Define configuration files */
	public static String flat_conf = "../flat_config.cfg";
	public static String xml_conf = "../xml_config.xml";
	public static String app_name = "processA";

        public static void main(String[] args) throws LBMException
        {
		/* load flat file first */
		try 
		{
			LBM.setConfiguration(flat_conf);
		}
		catch (LBMException ex)
		{
			System.err.println("Error setting LBM configuration: " + ex.toString());
			System.exit(1);
		}

		/* then load XML file */
		try
                {
			LBM.setConfigurationXml(xml_conf, app_name);
		}
		catch (LBMException ex)
                {
                        System.err.println("Error setting LBM configuration: " + ex.toString());
                        System.exit(1);
                }
	}
}
