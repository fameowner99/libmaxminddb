#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "MMDB.h"
#include "MMDB_Helper.h"
#include "getopt.h"
#include <assert.h>

void usage(char *prg)
{
    fprintf(stderr, "Usage: %s -f database addr\n", prg);
    exit(1);
}

int main(int argc, char *const argv[])
{
    int verbose = 0;
    int character;
    char *fname = NULL;

    while ((character = getopt(argc, argv, "vf:")) != -1) {
        switch (character) {
        case 'v':
            verbose = 1;
            break;
        case 'f':
            fname = strdup(optarg);
            break;
        default:
        case '?':
            usage(argv[0]);
        }
    }
    argc -= optind;
    argv += optind;

    if (!fname) {
        fname = strdup("/usr/local/share/GeoIP2/reference-database.dat");
    }

    assert(fname != NULL);

    MMDB_s *mmdb = MMDB_open(fname, MMDB_MODE_MEMORY_CACHE);

    if (!mmdb) {
        fprintf(stderr, "Can't open %s\n", fname);
        exit(1);
    }

    free(fname);

    char *ipstr = argv[0];
    struct in_addr ip;
    if (ipstr == NULL || 1 != addr_to_num(ipstr, &ip)) {
        fprintf(stderr, "Invalid IP\n");
        exit(1);
    }

    if (verbose) {
        MMDB_decode_all_s *decode_all = calloc(1, sizeof(MMDB_decode_all_s));
        int err = MMDB_get_tree(&mmdb->meta, &decode_all);
        assert(err == MMDB_SUCCESS);

        if (decode_all != NULL)
            MMDB_dump(decode_all, 0);
        free(decode_all);
    }

    MMDB_root_entry_s root = {.entry.mmdb = mmdb };
    uint32_t ipnum = htonl(ip.s_addr);
    int err = MMDB_lookup_by_ipnum(ipnum, &root);
    if (err == MMDB_SUCCESS) {
        double dlat, dlon;
        char *city, *country, *region_name, code2[3];
        if (root.entry.offset > 0) {
            MMDB_return_s res_location;
            MMDB_decode_all_s *decode_all =
                calloc(1, sizeof(MMDB_decode_all_s));
            int err = MMDB_get_tree(&root.entry, &decode_all);
            if (decode_all != NULL)
                MMDB_dump(decode_all, 0);
            free(decode_all);

        } else {
            puts("Sorry, nothing found");       // not found
        }
    }

    return (0);
}