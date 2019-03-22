/*
 * Copyright (C) 1996-2019 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#ifndef SQUID_ACLFILLED_CHECKLIST_H
#define SQUID_ACLFILLED_CHECKLIST_H

#include "AccessLogEntry.h"
#include "acl/Checklist.h"
#include "acl/forward.h"
#include "base/CbcPointer.h"
#include "comm/forward.h"
#include "err_type.h"
#include "ip/Address.h"
#if USE_AUTH
#include "auth/UserRequest.h"
#endif
#include "security/CertError.h"

class CachePeer;
class ConnStateData;
class HttpRequest;
class HttpReply;

/** \ingroup ACLAPI
    ACLChecklist filled with specific data, representing Squid and transaction
    state for access checks along with some data-specific checking methods
 */
class ACLFilledChecklist: public ACLChecklist
{
    CBDATA_CLASS(ACLFilledChecklist);

public:
    ACLFilledChecklist();
    ACLFilledChecklist(const acl_access *, HttpRequest *, const char *ident = nullptr);
    ~ACLFilledChecklist();

    /// Configure client request-related fields for the first time.
    /// The passed HttpRequest parameter usually contains all client-related
    /// data, including addresses and the connection manager.
    void setRequest(HttpRequest *);

    /// configure rfc931 user identity for the first time
    void setIdent(const char *userIdentity);

    /// Configures client-related fields from the passed client connection manager.
    /// Has no effect if the client connection manager field is already initialized.
    /// This method should be used in contexts where HttpRequest may be unavailable.
    void clientConnectionManager(ConnStateData *);

    /// Configures client-related fields from the passed client connection.
    /// Has no effect if the fields are already initialized.
    /// This method should be used in contexts where HttpRequest and
    /// connection manager may be unavailable.
    void clientConnection(Comm::ConnectionPointer);

    /// Configures srcAddr() to return either indirect client address or direct
    /// address depending on the argument, passed as one of *uses_indirect_client
    /// configuration options.
    void configureClientAddr(const bool useIndirect);

#if FOLLOW_X_FORWARDED_FOR
     /// Configures srcAddr() to always return available indirect client address
     /// instead of direct client address.
     void forceIndirectAddr() { forceIndirectAddr_ = true; }
#endif /* FOLLOW_X_FORWARDED_FOR */

    /// the associated client connection manager or nil
    ConnStateData *clientConnectionManager() const;

    /// remote address, direct or indirect
    const Ip::Address &srcAddr() const;

    /// local address
    const Ip::Address &myAddr() const { return my_addr; }

    /// The client side fd. It uses conn() if available
    int fd() const;

    // TODO: remove as unused?
    /// set the client side FD
    void fd(int aDescriptor);

    //int authenticated();

    bool destinationDomainChecked() const;
    void markDestinationDomainChecked();
    bool sourceDomainChecked() const;
    void markSourceDomainChecked();

    // ACLChecklist API
    virtual bool hasRequest() const { return request != NULL; }
    virtual bool hasReply() const { return reply != NULL; }
    virtual bool hasAle() const { return al != NULL; }
    virtual void syncAle(HttpRequest *adaptedRequest, const char *logUri) const;
    virtual void verifyAle() const;

public:
    Ip::Address dst_addr;
    SBuf dst_peer_name;
    char *dst_rdns;

    HttpRequest *request;
    HttpReply *reply;

    char rfc931[USER_IDENT_SZ];
#if USE_AUTH
    Auth::UserRequest::Pointer auth_user_request;
#endif
#if SQUID_SNMP
    /// configure with SNMP specific parameters
    void snmpDetails(char *community, const Ip::Address &fromAddr, const Ip::Address &localAddr);

    char *snmp_community;
#endif

    /// SSL [certificate validation] errors, in undefined order
    const Security::CertErrors *sslErrors;
    /// The peer certificate
    Security::CertPointer serverCert;

    AccessLogEntry::Pointer al; ///< info for the future access.log, and external ACL

    ExternalACLEntryPointer extacl_entry;

    err_type requestErrorType;

private:
    void setClientConnectionManager(ConnStateData *);
    void setClientConnection(Comm::ConnectionPointer);

    /// a client connection manager, if any
    ConnStateData *connectionManager_;
    /// a client connection, if any
    Comm::ConnectionPointer clientConnection_;
    // TODO: remove as unused?
    int fd_;                        /**< may be available when conn_ is not */
    bool destinationDomainChecked_;
    bool sourceDomainChecked_;
    Ip::Address src_addr;
    Ip::Address my_addr;
#if FOLLOW_X_FORWARDED_FOR
    /// whether we will use indirect client address instead of direct address
    bool forceIndirectAddr_;
#endif /* FOLLOW_X_FORWARDED_FOR */
    /// not implemented; will cause link failures if used
    ACLFilledChecklist(const ACLFilledChecklist &);
    /// not implemented; will cause link failures if used
    ACLFilledChecklist &operator=(const ACLFilledChecklist &);
};

/// convenience and safety wrapper for dynamic_cast<ACLFilledChecklist*>
inline
ACLFilledChecklist *Filled(ACLChecklist *checklist)
{
    // this should always be safe because ACLChecklist is an abstract class
    // and ACLFilledChecklist is its only [concrete] child
    return dynamic_cast<ACLFilledChecklist*>(checklist);
}

#endif /* SQUID_ACLFILLED_CHECKLIST_H */

