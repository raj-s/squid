/*
 * Copyright (C) 1996-2018 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#ifndef ICMP_NET_DB_H
#define ICMP_NET_DB_H

#include "hash.h"
#include "ip/forward.h"
#include "mem/forward.h"

class CachePeer;
class HttpRequest;
class netdbEntry;
class ps_state;
class StoreEntry;
class URL;

class net_db_name
{
    MEMPROXY_CLASS(net_db_name);

public:
    net_db_name(const char *name, netdbEntry *);
    ~net_db_name() {xfree(hash.key);}

    hash_link hash;     /* must be first */
    net_db_name *next;
    netdbEntry *net_db_entry;
};

// POD
class net_db_peer
{
public:
    const char *peername;
    double hops;
    double rtt;
    time_t expires;
};

class netdbEntry
{
    MEMPROXY_CLASS(netdbEntry);

public:
    netdbEntry() { *network = 0; }

    hash_link hash;     /* must be first */
    char network[MAX_IPSTRLEN];
    int pings_sent = 0;
    int pings_recv = 0;
    double hops = 0;
    double rtt = 1.0;
    time_t next_ping_time = 0;
    time_t last_use_time = 0;
    int link_count = 0;
    net_db_name *hosts = nullptr;
    net_db_peer *peers = nullptr;
    int n_peers_alloc = 0;
    int n_peers = 0;
};

void netdbInit(void);

void netdbHandlePingReply(const Ip::Address &from, int hops, int rtt);
void netdbPingSite(const char *hostname);
void netdbDump(StoreEntry *);

void netdbFreeMemory(void);
int netdbHostHops(const char *host);
int netdbHostRtt(const char *host);
void netdbUpdatePeer(const URL &, CachePeer * e, int rtt, int hops);

void netdbDeleteAddrNetwork(Ip::Address &addr);
void netdbBinaryExchange(StoreEntry *);
void netdbExchangeStart(void *);

void netdbExchangeUpdatePeer(Ip::Address &, CachePeer *, double, double);
CachePeer *netdbClosestParent(ps_state *);
void netdbHostData(const char *host, int *samp, int *rtt, int *hops);

#endif /* ICMP_NET_DB_H */

