/* load_config.c - http://ultramessaging.github.io/UMExamples */

#include <stdio.h>

#if defined(_MSC_VER)
/* Windows-only includes */
#include <winsock2.h>
#else
/* Unix-only includes */
#include <stdlib.h>
#include <unistd.h>
#endif

#include <lbm/lbm.h>

/* Example error checking macro.  Include after each UM call. */
#define EX_LBM_CHK(err) do { \
        if ((err) < 0) { \
                fprintf(stderr, "%s:%d, lbm error: '%s'\n", \
                __FILE__, __LINE__, lbm_errmsg()); \
                exit(1); \
        }  \
} while (0)

#define LBM_CONFIG_FLAT_FILE "../flat_config.cfg"
#define LBM_CONFIG_XML_FILE  "../xml_config.xml"
#define LBM_APPLICATION_NAME "processA"

int main(int argc, char **argv)
{
	int err;	/* Used for checking API return codes */

	err = lbm_config((const char *)LBM_CONFIG_FLAT_FILE);
	EX_LBM_CHK(err);

	err = lbm_config_xml_file((const char *)LBM_CONFIG_XML_FILE, (const char *)LBM_APPLICATION_NAME);
	EX_LBM_CHK(err);
        return 0;
}
