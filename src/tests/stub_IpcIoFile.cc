/*
 * Copyright (C) 1996-2020 The Squid Software Foundation and contributors
 *
 * Squid software is distributed under GPLv2+ license and includes
 * contributions from numerous individuals and organizations.
 * Please see the COPYING and CONTRIBUTORS files for details.
 */

#include "squid.h"

#if USE_DISKIO
#include "DiskIO/IpcIo/IpcIoFile.h"

#define STUB_API "DiskIO/IocIo/IpcIoFile.cc"

#include "tests/STUB.h"

void IpcIoFile::StatQueue(std::ostream &) STUB

#endif /* USE_DISKIO */

