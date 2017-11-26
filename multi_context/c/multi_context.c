/* multi_context.c - see http://ultramessaging.github.io/UMExamples/multi_context/c/index.html
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
#define SLEEP(s) Sleep((s)*1000)
#else
/* Unix-only includes */
#include <stdlib.h>
#include <unistd.h>
#define SLEEP(s) sleep(s)
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

#define LBM_CONFIG_XML_FILE  "multi_context_config.xml"
#define LBM_APPLICATION_NAME "main"


int main(int argc, char **argv)
{
	lbm_context_t *ctx_1;          /* Context object */
	lbm_context_attr_t * cattr_1;  /* pointer to context attribute object */
	lbm_context_t *ctx_2;          /* Context object */
	lbm_context_attr_t * cattr_2;  /* pointer to context attribute object */
	int err;

#if defined(_WIN32)
	/* windows-specific code */
	WSADATA wsadata;
	int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
	if (wsStat != 0)
	{
		printf("line %d: wsStat=%d\n",__LINE__,wsStat);
		exit(1);
	}
#endif

	err = lbm_config_xml_file((const char *)LBM_CONFIG_XML_FILE, (const char *)LBM_APPLICATION_NAME);
	EX_LBM_CHK(err);

	err = lbm_context_attr_create_from_xml(&cattr_1, "TRD1");
	EX_LBM_CHK(err);

	err = lbm_context_create(&ctx_1, cattr_1, NULL, NULL);
	EX_LBM_CHK(err);

	err = lbm_context_attr_create_from_xml(&cattr_2, "TRD2");
	EX_LBM_CHK(err);

	err = lbm_context_create(&ctx_2, cattr_2, NULL, NULL);
	EX_LBM_CHK(err);
	
	lbm_context_attr_delete(cattr_1);
	lbm_context_attr_delete(cattr_2);
	lbm_context_delete(ctx_1);
	lbm_context_delete(ctx_2);

#if defined(_MSC_VER)
	WSACleanup();
#endif	
	return 0;
}  /* main */
