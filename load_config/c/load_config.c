/* load_config.c - see http://ultramessaging.github.io/UMExamples/load_config/c/index.html
 *
 * Copyright (c) 2005-2017 Informatica Corporation. All Rights Reserved.
 * Permission is granted to licensees to use
 * or alter this software for any purpose, including commercial applications,
 * according to the terms laid out in the Software License Agreement.
 *
 * This source code example is provided by Informatica for educational
 * and evaluation purposes only.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INFORMATICA DISCLAIMS ALL WARRANTIES
 * EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
 * NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
 * PURPOSE.  INFORMATICA DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE.  INFORMATICA SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
 * LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
 * INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
 * TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF INFORMATICA HAS BEEN APPRISED OF
 * THE LIKELIHOOD OF SUCH DAMAGES.
 */

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
	int err;

	err = lbm_config((const char *)LBM_CONFIG_FLAT_FILE);
	EX_LBM_CHK(err);

	err = lbm_config_xml_file((const char *)LBM_CONFIG_XML_FILE, (const char *)LBM_APPLICATION_NAME);
	EX_LBM_CHK(err);

	return 0;
}  /* main */
