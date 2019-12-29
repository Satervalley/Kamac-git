/*
    Itzam/C (version 7.0) is an embedded database engine written in Standard C.
    
    It is part of the Drakontos Library of Interesting and Esoteric Oddities

    Copyright 2016 Scott Robert Ladd. All rights reserved.

    Itzam/C is user-supported open source software. It's continued development is dependent on
    financial support from the community. You can provide funding by visiting the Itzam/C
    website at:

        http://www.drakontos.com

    You license Itzam/C under the Simplified BSD License (FreeBSD License), the text of which 
    is available at the website above. 
*/

#if !defined(ITZAM_ERRORS_H)
#define ITZAM_ERRORS_H

static const char * ERROR_STRINGS [] =
{
    "invalid datafile signature",
    "invalid version",
    "can not open 64-bit datafile on 32-bit operating system",
    "write failed",
    "open failed",
    "read failed",
    "close failed",
    "seek failed",
    "tell failed",
    "duplicate remove",
    "flush failed",
    "rewrite record too small",
    "page not found",
    "lost key",
    "key not written",
    "key seek failed",
    "unable to remove key record",
    "record seek failed",
    "unable to remove data record",
    "list of deleted records could not be read",
    "list of deleted records could not be written",
    "iterator record count differs from database internal count",
    "rewrite over deleted record",
    "invalid column index",
    "invalid row index",
    "invalid hash value",
    "memory allocation failed",
    "attempt reading deleted record",
    "invalid record signature found",
    "invalid file locking mode",
    "unable to lock datafile",
    "unable to unlock datafile",
    "size mismatch when reading record",
    "attempt to start new transaction while one is already active",
    "no transaction active",
    "attempt to free a B-tree cursor when cursors were active",
    "invalid datafile",
    "size_t is incompatible with Itzam",
    "could not create datafile",
    "global shared memory requires Administrator or user with SeCreateGlobalPrivilege",
    "cannot create global shared memory",
    "another process or thread has already created shared objects for this datafile",
    "invalid operation for read only file"
};

static const char * STATE_MESSAGES [] =
{
    "okay",
    "operation failed",
    "version mismatch in files",
    "iterator at end",
    "iterator at beginning",
    "key not found",
    "duplicate key",
    "exceeded maximum file size on 32-bit system",
    "unable to write data record for index",
    "sizeof(size_t) smaller than required for file references; possibly 64-bit DB on 32-bit platform",
    "invalid operation for read only file"
};

#endif
