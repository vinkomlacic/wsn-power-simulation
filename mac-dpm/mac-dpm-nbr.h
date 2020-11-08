/**
 * Builds a neighbor table by sending a beacon periodically.
 */
#ifndef MAC_DPM_NBR_
#define MAC_DPM_NBR_

#include "net/linkaddr.h"
#include "net/link-stats.h"
#include "net/nbr-table.h"

#include <stdbool.h>

/* Type definitions */
typedef struct mac_dpm_neighbor {
    linkaddr_t linkaddr;
    bool verified;
    const struct link_stats * link_stats;
} mac_dpm_neighbor_t;


/* The neighbor table. */
NBR_TABLE_DECLARE(mac_dpm_neighbors);


/* Initializes the neighbor table. */
void init_mac_dpm_nbr(void);


void print_nbr_table(void);


/* Counts the number of neighbors in the table. */
int count_neighbors(void);


/* Returns the number of verified neighbors in the table. */
int count_verified_neighbors(void);


/* Finds the neighbor link stats and adds it to the neighbor table. */
void add_linkaddr_to_nbr_table(linkaddr_t const *);


/* Store the neighbor to the neighbor table. */
void add_nbr_to_nbr_table(mac_dpm_neighbor_t);


/* Removes the neighbor from the neighbor table. */
void remove_nbr_from_nbr_table(mac_dpm_neighbor_t);


/* Tags the neighbor as verified. It means that the table record is fresh. */
void verify_nbr_with_linkaddr(linkaddr_t const *);


/* Removes verified attribute so that the neighbors are ready for another verification. */
void revoke_nbr_verification(void);


/* Returns true if all neighbors are verified. */
bool all_nbrs_are_verified(void);


/* Clears the neighbor table. */
void clear_nbr_table(void);


/* Return the neighbor with the worst link stats. */
mac_dpm_neighbor_t * min_link_stats_neighbor(void);


#endif /* MAC_DPM_NBR_ */
