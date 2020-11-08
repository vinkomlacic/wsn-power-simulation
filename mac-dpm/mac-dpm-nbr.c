#include "net/mac/mac-dpm/mac-dpm-nbr.h"
#include "net/mac/mac-dpm/mac-dpm-conf.h"

#include "net/nbr-table.h"
#include "net/linkaddr.h"
#include "net/link-stats.h"


#include "sys/log.h"
#define LOG_MODULE "MAC DPM NBR"
#define LOG_LEVEL LOG_LEVEL_MAC_DPM_NBR

#if LOG_LEVEL == LOG_LEVEL_DBG
#define MAC_DPM_NBR_DEBUG
#endif


/* Neighbor table. */
NBR_TABLE_GLOBAL(mac_dpm_neighbor_t, mac_dpm_neighbors);


#ifdef MAC_DPM_NBR_DEBUG
void
print_nbr_table(void) {
    LOG_INFO("NBR TABLE\n");
    nbr_table_item_t * nbr;
    mac_dpm_neighbor_t * mac_nbr;

    for (nbr = nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = nbr_table_next(mac_dpm_neighbors, nbr)) {
            mac_nbr = (mac_dpm_neighbor_t *) nbr;

            LOG_INFO_LLADDR(&mac_nbr->linkaddr);
            LOG_INFO_(" %d %u\n", mac_nbr->link_stats->rssi, mac_nbr->link_stats->etx);
    }
}
#else
void
print_nbr_table(void) {}
#endif /* MAC_DPM_NBR_DEBUG */


void
init_mac_dpm_nbr(void) {
    nbr_table_register(mac_dpm_neighbors, (nbr_table_callback *) remove_nbr_from_nbr_table);
}


int
count_neighbors(void) {
    nbr_table_item_t * nbr;
    int count = 0;

    for (nbr = nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = nbr_table_next(mac_dpm_neighbors, nbr)) {
            count++;
    }

    return count;
}


int
count_verified_neighbors(void) {
    mac_dpm_neighbor_t * nbr;
    int count = 0;

    for (nbr = (mac_dpm_neighbor_t *) nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = (mac_dpm_neighbor_t *) nbr_table_next(mac_dpm_neighbors, nbr)) {
            if (nbr->verified) {
                count++;
            }
    }

    return count;
}


void
add_linkaddr_to_nbr_table(linkaddr_t const * const neighbor_linkaddr) {
    if (linkaddr_cmp(neighbor_linkaddr, &linkaddr_null)) {
        LOG_WARN("Broadcast address. Not adding to the neighbor table.\n");
        return;
    }

    mac_dpm_neighbor_t * nbr = nbr_table_add_lladdr(mac_dpm_neighbors, neighbor_linkaddr, NBR_TABLE_REASON_LINK_STATS, NULL);
    nbr->linkaddr = *neighbor_linkaddr;
    nbr->link_stats = link_stats_from_lladdr(neighbor_linkaddr);
    nbr->verified = false;
    LOG_INFO("Neighbor with lladdr ");
    LOG_INFO_LLADDR(&nbr->linkaddr);
    LOG_INFO_(" added to the neighbor table.\n");
}


void
add_nbr_to_nbr_table(mac_dpm_neighbor_t const neighbor) {
    mac_dpm_neighbor_t * nbr = nbr_table_add_lladdr(mac_dpm_neighbors, &neighbor.linkaddr, NBR_TABLE_REASON_LINK_STATS, NULL);
    nbr->linkaddr = neighbor.linkaddr;
    nbr->link_stats = neighbor.link_stats;
    nbr->verified = false;
    LOG_INFO("Neighbor with lladdr ");
    LOG_INFO_LLADDR(&nbr->linkaddr);
    LOG_INFO_(" added to the neighbor table.\n");
}


void
remove_nbr_from_nbr_table(mac_dpm_neighbor_t const neighbor) {
    nbr_table_item_t * const nbr = nbr_table_get_from_lladdr(mac_dpm_neighbors, &neighbor.linkaddr);
    nbr_table_remove(mac_dpm_neighbors, nbr);
}


void
verify_nbr_with_linkaddr(linkaddr_t const * const nbr_linkaddr) {
    mac_dpm_neighbor_t * const nbr = (mac_dpm_neighbor_t *) nbr_table_get_from_lladdr(mac_dpm_neighbors, nbr_linkaddr);
    nbr->verified = true;
}


void
revoke_nbr_verification(void) {
    mac_dpm_neighbor_t * nbr;

    for (nbr = (mac_dpm_neighbor_t *) nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = (mac_dpm_neighbor_t *) nbr_table_next(mac_dpm_neighbors, nbr)) {
            nbr->verified = false;
    }
}


bool
all_nbrs_are_verified(void) {
    mac_dpm_neighbor_t * nbr;
    bool all_are_verified = true;

    for (nbr = (mac_dpm_neighbor_t *) nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = (mac_dpm_neighbor_t *) nbr_table_next(mac_dpm_neighbors, nbr)) {
            all_are_verified &= nbr->verified;
    }

    return all_are_verified;
}


mac_dpm_neighbor_t *
min_link_stats_neighbor(void) {
    mac_dpm_neighbor_t * current_neighbor = (mac_dpm_neighbor_t *) nbr_table_head(mac_dpm_neighbors);
    mac_dpm_neighbor_t * worst_neighbor = current_neighbor;
    int32_t current_neighbor_score = current_neighbor->link_stats->rssi + current_neighbor->link_stats->etx;
    int32_t worst_neighbor_score = current_neighbor_score;

    for (current_neighbor = (mac_dpm_neighbor_t *) nbr_table_head(mac_dpm_neighbors);
        current_neighbor != NULL;
        current_neighbor = (mac_dpm_neighbor_t *) nbr_table_next(mac_dpm_neighbors, current_neighbor)) {
            current_neighbor_score = current_neighbor->link_stats->rssi + current_neighbor->link_stats->etx;
            if (current_neighbor_score < worst_neighbor_score) {
                worst_neighbor = current_neighbor;
                worst_neighbor_score = current_neighbor_score;
            }
    }

    return worst_neighbor;
}



void clear_nbr_table(void) {
    nbr_table_item_t * nbr;

    for (nbr = nbr_table_head(mac_dpm_neighbors);
        nbr != NULL;
        nbr = nbr_table_next(mac_dpm_neighbors, nbr)) {
            nbr_table_remove(mac_dpm_neighbors, nbr);
    }
    LOG_DBG("Neighbor table cleared.");
}

