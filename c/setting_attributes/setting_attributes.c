/* Example on setting Ultra Messaging attributes */

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

main()
{
    lbm_context_t *ctx;             /* pointer to context object */
    lbm_topic_t *topic_1;           /* pointer to topic object */
    lbm_topic_t *topic_2;           /* pointer to topic object */
    lbm_src_t *src_1;               /* pointer to source object */
    lbm_src_t *src_2;               /* pointer to source object */
    lbm_src_topic_attr_t *tattr_1;  /* pointer to source attribute object */
    lbm_src_topic_attr_t *tattr_2;  /* pointer to source attribute object */
    lbm_context_attr_t * cattr;     /* pointer to context attribute object */
    lbm_uint16_t res_port;          /* Int to set resolver port */
    lbm_uint16_t des_port;          /* Int to set lbtrm destination port */
    int err;                        /* return status of lbm functions (true=error) */

#if defined(_MSC_VER)
    /* windows-specific code */
    WSADATA wsadata;
    int wsStat = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (wsStat != 0)
    {
        printf("line %d: wsStat=%d\n",__LINE__,wsStat);
        exit(1);
    }
#endif

    /* Initialize the defaults for the context attribute object */
    if (lbm_context_attr_create(&cattr) != 0)
    {
    	fprintf(stderr, "lbm_context_attr_create: %s\n", lbm_errmsg());
	exit(1);
    }

    /* Setting the resolver address using the string method */
    if (lbm_context_attr_str_setopt(cattr, "resolver_multicast_address", "224.10.11.12") != 0) 
    {
     	fprintf(stderr, "lbm_context_attr_str_setopt:resolver_multicast_address: %s\n", lbm_errmsg());
     	exit(1);
    }

    /* Setting the resolver port using the data-type value method */
    res_port = 12345;
    if (lbm_context_attr_setopt(cattr, "resolver_multicast_port", &res_port, sizeof(lbm_uint16_t)) != 0 )
    {
	fprintf(stderr, "lbm_context_attr_setopt:resolver_mutlicast_port: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Creating the context */
    err = lbm_context_create(&ctx, cattr, NULL, NULL);
    if (err)
    {
        printf("line %d: %s\n", __LINE__, lbm_errmsg());
        exit(1);
    }

    /* Delete the context attribute object */
    lbm_context_attr_delete(cattr);

    /* Initializing the source attribute object */
    if (lbm_src_topic_attr_create(&tattr_1) != 0)
    {
    	fprintf(stderr, "lbm_src_topic_attr_create: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Setting the transport via the source topic string method */
    if (lbm_src_topic_attr_str_setopt(tattr_1, "transport", "lbtrm") != 0 )
    {
    	fprintf(stderr, "lbm_src_topic_attr_str_setopt:transport: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Setting the lbtrm destination port via the direct value set method */
    des_port = 14001;
    if (lbm_src_topic_attr_setopt(tattr_1, "transport_lbtrm_destination_port", &des_port, sizeof(lbm_uint16_t)) != 0 )
    {
    	fprintf(stderr, "lbm_src_topic_attr_setopt:transport_lbtrm_destination_port: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Allocating the topic */
    err = lbm_src_topic_alloc(&topic_1, ctx, "test.topic.1", tattr_1);
    if (err)
    {
        printf("line %d: %s\n", __LINE__, lbm_errmsg());
        exit(1);
    }

    /* Creating the source */
    err = lbm_src_create(&src_1, ctx, topic_1, NULL, NULL, NULL);
    if (err)
    {
        printf("line %d: %s\n", __LINE__, lbm_errmsg());
        exit(1);
    }

    /* Initialized the second source attributes as a copy of the first */
    if (lbm_src_topic_attr_dup(&tattr_2, tattr_1) != 0 )
    {
        fprintf(stderr, "lbm_src_topic_attr_dup: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Now modify the destination port for this second source to put the publisher on a different transport */
    des_port = 14002;
    if (lbm_src_topic_attr_setopt(tattr_2, "transport_lbtrm_destination_port", &des_port, sizeof(lbm_uint16_t)) != 0 )
    {
        fprintf(stderr, "lbm_src_topic_attr_setopt:transport_lbtrm_destination_port: %s\n", lbm_errmsg());
        exit(1);
    }

    /* Allocating the second topic */
    err = lbm_src_topic_alloc(&topic_2, ctx, "test.topic.2", tattr_2);
    if (err)
    {
        printf("line %d: %s\n", __LINE__, lbm_errmsg());
        exit(1);
    }

    /* Creating the source */
    err = lbm_src_create(&src_2, ctx, topic_2, NULL, NULL, NULL);
    if (err)
    {
        printf("line %d: %s\n", __LINE__, lbm_errmsg());
        exit(1);
    }

    /* Delete the first and second source topic attribute objects */
    lbm_src_topic_attr_delete(tattr_1);
    lbm_src_topic_attr_delete(tattr_2);

    /* Finished with all LBM functions, delete the source and context object. */
    lbm_src_delete(src_1);
    lbm_src_delete(src_2);
    lbm_context_delete(ctx);

#if defined(_MSC_VER)
    WSACleanup();
#endif
}

