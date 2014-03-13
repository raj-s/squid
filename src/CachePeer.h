#ifndef SQUID_CACHEPEER_H_
#define SQUID_CACHEPEER_H_
/*
 * SQUID Web Proxy Cache          http://www.squid-cache.org/
 * ----------------------------------------------------------
 *
 *  Squid is the result of efforts by numerous individuals from
 *  the Internet community; see the CONTRIBUTORS file for full
 *  details.   Many organizations have provided support for Squid's
 *  development; see the SPONSORS file for full details.  Squid is
 *  Copyrighted (C) 2001 by the Regents of the University of
 *  California; see the COPYRIGHT file for full details.  Squid
 *  incorporates software developed and/or copyrighted by other
 *  sources; see the CREDITS file for full details.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 */

#include "acl/forward.h"
#include "base/CbcPointer.h"
#include "enums.h"
#include "icp_opcode.h"
#include "ip/Address.h"

//TODO: remove, it is unconditionally defined and always used.
#define PEER_MULTICAST_SIBLINGS 1

#if USE_SSL
#include <openssl/ssl.h>
#endif

class CachePeerDomainList;
class NeighborTypeDomainList;
class PconnPool;
class PeerDigest;
class PeerPoolMgr;

// currently a POD
class CachePeer
{
public:
    u_int index;
    char *name;
    char *host;
    peer_t type;

    Ip::Address in_addr;

    struct {
        int pings_sent;
        int pings_acked;
        int fetches;
        int rtt;
        int ignored_replies;
        int n_keepalives_sent;
        int n_keepalives_recv;
        time_t probe_start;
        time_t last_query;
        time_t last_reply;
        time_t last_connect_failure;
        time_t last_connect_probe;
        int logged_state;   /* so we can print dead/revived msgs */
        int conn_open;      /* current opened connections */
    } stats;

    struct {
        int version;
        int counts[ICP_END+1];
        unsigned short port;
    } icp;

#if USE_HTCP
    struct {
        double version;
        int counts[2];
        unsigned short port;
    } htcp;
#endif

    unsigned short http_port;
    CachePeerDomainList *peer_domain;
    NeighborTypeDomainList *typelist;
    acl_access *access;

    struct {
        bool proxy_only;
        bool no_query;
        bool background_ping;
        bool no_digest;
        bool default_parent;
        bool roundrobin;
        bool weighted_roundrobin;
        bool mcast_responder;
        bool closest_only;
#if USE_HTCP
        bool htcp;
        bool htcp_oldsquid;
        bool htcp_no_clr;
        bool htcp_no_purge_clr;
        bool htcp_only_clr;
        bool htcp_forward_clr;
#endif
        bool no_netdb_exchange;
#if USE_DELAY_POOLS
        bool no_delay;
#endif
        bool allow_miss;
        bool carp;
        struct {
            bool set; //If false, whole url is to be used. Overrides others
            bool scheme;
            bool host;
            bool port;
            bool path;
            bool params;
        } carp_key;
#if USE_AUTH
        bool userhash;
#endif
        bool sourcehash;
        bool originserver;
        bool no_tproxy;
#if PEER_MULTICAST_SIBLINGS
        bool mcast_siblings;
#endif
    } options;

    int weight;
    int basetime;

    struct {
        double avg_n_members;
        int n_times_counted;
        int n_replies_expected;
        int ttl;
        int id;

        struct {
            bool count_event_pending;
            bool counting;
        } flags;
    } mcast;
#if USE_CACHE_DIGESTS

    PeerDigest *digest;
    char *digest_url;
#endif

    int tcp_up;         /* 0 if a connect() fails */

    Ip::Address addresses[10];
    int n_addresses;
    int rr_count;
    CachePeer *next;
    int testing_now;

    struct {
        unsigned int hash;
        double load_multiplier;
        double load_factor; /* normalized weight value */
    } carp;
#if USE_AUTH
    struct {
        unsigned int hash;
        double load_multiplier;
        double load_factor; /* normalized weight value */
    } userhash;
#endif
    struct {
        unsigned int hash;
        double load_multiplier;
        double load_factor; /* normalized weight value */
    } sourcehash;

    char *login;        /* Proxy authorization */
    time_t connect_timeout;
    int connect_fail_limit;
    int max_conn;
    struct {
        PconnPool *pool; ///< idle connection pool for this peer
        CbcPointer<PeerPoolMgr> mgr; ///< pool manager
        int limit; ///< the limit itself
        bool waitingForClose; ///< a conn must close before we open a standby conn
    } standby; ///< optional "cache_peer standby=limit" feature
    char *domain;       /* Forced domain */
#if USE_SSL

    int use_ssl;
    char *sslcert;
    char *sslkey;
    int sslversion;
    char *ssloptions;
    char *sslcipher;
    char *sslcafile;
    char *sslcapath;
    char *sslcrlfile;
    char *sslflags;
    char *ssldomain;
    SSL_CTX *sslContext;
    SSL_SESSION *sslSession;
#endif

    int front_end_https;
    int connection_auth;
};

#endif /* SQUID_CACHEPEER_H_ */
