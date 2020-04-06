/*
 * Copyright (C) 1996-2020 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#ifndef SQUID_RESOLVEDPEERS_H
#define SQUID_RESOLVEDPEERS_H

#include "base/RefCount.h"
#include "comm/forward.h"

#include <iosfwd>

class ResolvedPeerPath
{
public:
    explicit ResolvedPeerPath(const Comm::ConnectionPointer &conn) : connection(conn), available(true) {}

    Comm::ConnectionPointer connection; ///< a path(address) candidate
    bool available; ///< whether this address may be used (i.e., has not been tried already)
};

typedef std::vector<ResolvedPeerPath> ConnectionList;

/// cache_peer and origin server addresses (a.k.a. paths)
/// selected and resolved by the peering code
class ResolvedPeers: public RefCountable
{
    MEMPROXY_CLASS(ResolvedPeers);

public:
    typedef RefCount<ResolvedPeers> Pointer;

    ResolvedPeers();

    /// whether we lack any known candidate paths
    bool empty() const;

    /// add a candidate path to try after all the existing paths
    void addPath(const Comm::ConnectionPointer &);

    /// re-inserts the previously extracted address into the same position
    void retryPath(const Comm::ConnectionPointer &);

    /// extracts and returns the first queued address
    Comm::ConnectionPointer extractFront();

    /// extracts and returns the first same-peer same-family address
    /// \returns nil if it cannot find the requested address
    Comm::ConnectionPointer extractPrime(const Comm::Connection &currentPeer);

    /// extracts and returns the first same-peer different-family address
    /// \returns nil if it cannot find the requested address
    Comm::ConnectionPointer extractSpare(const Comm::Connection &currentPeer);

    /// whether extractSpare() would return a non-nil path right now
    bool haveSpare(const Comm::Connection &currentPeer);

    /// whether extractPrime() returns and will continue to return nil
    bool doneWithPrimes(const Comm::Connection &currentPeer);

    /// whether extractSpare() returns and will continue to return nil
    bool doneWithSpares(const Comm::Connection &currentPeer);

    /// whether doneWithPrimes() and doneWithSpares() are true for currentPeer
    bool doneWithPeer(const Comm::Connection &currentPeer);

    /// the current number of candidate paths
    ConnectionList::size_type size() const;

    /// whether all of the available candidate paths received from DNS
    bool destinationsFinalized = false;

    /// whether HappyConnOpener::noteCandidatesChange() is scheduled to fire
    bool notificationPending = false;

private:
    using size_type = ConnectionList::size_type;

    /// The protocol family of the given path, AF_INET or AF_INET6
    static int ConnectionFamily(const Comm::Connection &conn);

    ConnectionList::iterator start();
    ConnectionList::iterator findSpare(const Comm::Connection &currentPeer, bool *hasNext = nullptr);
    ConnectionList::iterator findPrime(const Comm::Connection &currentPeer, bool *hasNext = nullptr);
    ConnectionList::iterator findPeer(const Comm::Connection &currentPeer, bool *hasNext = nullptr);
    Comm::ConnectionPointer extractFound(const char *description, const ConnectionList::iterator &found);

    typedef ConnectionList::iterator (ResolvedPeers::*findSmthFun)(const Comm::Connection &, bool *);
    bool doneWith(const Comm::Connection &currentPeer, findSmthFun);

    ConnectionList paths_; ///< resolved addresses in (peer, family) order
    /// the number of leading paths_ elements that are all currently unavailable
    /// i.e. the size of the front paths_ segment comprised of unavailable items
    /// i.e. the position of the first available candidate (or paths_.size())
    size_type candidatesToSkip = 0;
};

/// summarized ResolvedPeers (for debugging)
std::ostream &operator <<(std::ostream &, const ResolvedPeers &);

#endif /* SQUID_RESOLVEDPEERS_H */

