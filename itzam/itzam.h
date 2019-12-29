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

#if !defined(LIBITZAM_ITZAM_H)
#define LIBITZAM_ITZAM_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
//#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

//-- Operating system identification and settings
 
#define ITZAM_MAX_FILE_SIZE 9223372036854775808

#if defined(_WIN64) || defined (_MSC_VER)
    #define ITZAM_WINDOWS
    #include <io.h>
    #include <fcntl.h>
    #include <share.h>
    #include <windows.h>
    #include <process.h>
    #define ITZAM_SHMEM_TYPE HANDLE
    #define ITZAM_FILE_TYPE  HANDLE
    #define ITZAM_GOOD_FILE(file) (file != INVALID_HANDLE_VALUE)
    #define ITZAM_SEEK_BEGIN   FILE_BEGIN
    #define ITZAM_SEEK_END     FILE_END
    #define ITZAM_SEEK_CURRENT FILE_CURRENT
    #define DLL_EXPORT
    #pragma warning (disable : 4786 4244 4267 4101 4996 4761 4127)
#elif defined(__linux__) || defined(__linux)
    #define ITZAM_LINUX
    #include <unistd.h>
    #include <pthread.h>
    #include <fcntl.h>
    #define ITZAM_SHMEM_TYPE   int64_t
    #define ITZAM_FILE_TYPE    int
    #define ITZAM_SEEK_BEGIN   SEEK_SET
    #define ITZAM_SEEK_END     SEEK_END
    #define ITZAM_SEEK_CURRENT SEEK_CUR
    #define ITZAM_GOOD_FILE(file) (file != -1)
#endif

//-- simple types and values

#pragma pack(push)
#pragma pack(8)

typedef uint8_t itzam_byte;
typedef int64_t itzam_ref;
typedef int64_t itzam_int;

static const itzam_ref ITZAM_NULL_REF = -1;
static const itzam_ref ITZAM_NULL_KEY = -1;

typedef enum {
    ITZAM_ERROR_SIGNATURE,              // 00
    ITZAM_ERROR_VERSION,
    ITZAM_ERROR_64BIT_DB,
    ITZAM_ERROR_WRITE_FAILED,
    ITZAM_ERROR_OPEN_FAILED,
    ITZAM_ERROR_READ_FAILED,
    ITZAM_ERROR_CLOSE_FAILED,
    ITZAM_ERROR_SEEK_FAILED,
    ITZAM_ERROR_TELL_FAILED,
    ITZAM_ERROR_DUPE_REMOVE,
    ITZAM_ERROR_FLUSH_FAILED,           // 10
    ITZAM_ERROR_TOO_SMALL,
    ITZAM_ERROR_PAGE_NOT_FOUND,
    ITZAM_ERROR_LOST_KEY,
    ITZAM_ERROR_KEY_NOT_WRITTEN,
    ITZAM_ERROR_KEY_SEEK_FAILED,
    ITZAM_ERROR_KEY_REMOVE_FAILED,
    ITZAM_ERROR_REC_SEEK_FAILED,
    ITZAM_ERROR_REC_REMOVE_FAILED,
    ITZAM_ERROR_DELLIST_NOT_READ,
    ITZAM_ERROR_DELLIST_NOT_WRITTEN,    // 20
    ITZAM_ERROR_ITERATOR_COUNT,
    ITZAM_ERROR_REWRITE_DELETED,
    ITZAM_ERROR_INVALID_COL,
    ITZAM_ERROR_INVALID_ROW,
    ITZAM_ERROR_INVALID_HASH,
    ITZAM_ERROR_MALLOC,
    ITZAM_ERROR_READ_DELETED,
    ITZAM_ERROR_INVALID_RECORD,
    ITZAM_ERROR_INVALID_FILE_LOCK_MODE,
    ITZAM_ERROR_FILE_LOCK_FAILED,       // 30
    ITZAM_ERROR_UNLOCK_FAILED,
    ITZAM_ERROR_REC_SIZE,
    ITZAM_ERROR_TRANSACTION_OVERLAP,
    ITZAM_ERROR_NO_TRANSACTION,
    ITZAM_ERROR_CURSOR_COUNT,
    ITZAM_ERROR_INVALID_DATAFILE_OBJECT,
    ITZAM_ERROR_SIZE_T,
    ITZAM_ERROR_FILE_CREATE,
    ITZAM_ERROR_SHMEM_PRIVILEGE,
    ITZAM_ERROR_SHMEM,                  // 40
    ITZAM_ERROR_ALREADY_CREATED,
    ITZAM_ERROR_READ_ONLY,
    ITZAM_ERROR_TOO_LONG
} itzam_error;

typedef enum {
    ITZAM_OKAY,
    ITZAM_FAILED,
    ITZAM_VERSION_ERROR,
    ITZAM_AT_END,
    ITZAM_AT_BEGIN,
    ITZAM_NOT_FOUND,
    ITZAM_DUPLICATE,
    ITZAM_32BIT_OVERFLOW,
    ITZAM_DATA_WRITE_FAILED,
    ITZAM_REF_SIZE_MISMATCH,
    ITZAM_READ_ONLY,
    ITZAM_OVERWRITE_TOO_LONG
} itzam_state;

//-- shared memory functions

ITZAM_SHMEM_TYPE itzam_shmem_obtain(const char * name, size_t len, bool * creator);
void itzam_shmem_close(ITZAM_SHMEM_TYPE shmem, const char * name);
void * itzam_shmem_getptr(ITZAM_SHMEM_TYPE shmem, size_t len);
bool itzam_shmem_commit(void * shmem_ptr);
bool itzam_shmem_freeptr(void * shmem_ptr, size_t len);

//-- generalized file operations

ITZAM_FILE_TYPE itzam_file_create(const char * filename);
ITZAM_FILE_TYPE itzam_file_open(const char * filename);
bool itzam_file_close(ITZAM_FILE_TYPE datafile);
bool itzam_file_read(ITZAM_FILE_TYPE datafile, void * data, size_t len);
bool itzam_file_write(ITZAM_FILE_TYPE datafile, const void * data, size_t len);
itzam_ref itzam_file_seek(ITZAM_FILE_TYPE datafile, itzam_ref pos, int mode);
itzam_ref itzam_file_tell(ITZAM_FILE_TYPE datafile);
bool itzam_file_commit(ITZAM_FILE_TYPE datafile);
bool itzam_file_remove(const char * filename);
bool itzam_file_lock(ITZAM_FILE_TYPE datafile);
bool itzam_file_unlock(ITZAM_FILE_TYPE datafile);

//-- general function types

typedef void itzam_error_handler(const char * function_name, itzam_error error);
extern itzam_error_handler * default_error_handler;
void itzam_set_default_error_handler(itzam_error_handler * handler);

//-- utility function for normalizing system objects' names (mutex, shm, ...)

void itzam_build_normalized_name(char * buffer, const char * format, const char * basename);

//-- functions of this type must return:
//      < 0    key1 is before key2
//        0    keys are equal
//      > 0    key1 is after key2
 
typedef int itzam_key_comparator(const void * key1, const void * key2);

//-- function to select keys to be included in a subset
//      itzam_true    include key
//      itzam_false   don't include this key
 
typedef bool itzam_key_selector(const void * key);

//-- built-in key comparisons
 
int itzam_comparator_int32(const void * key1, const void * key2);
int itzam_comparator_uint32(const void * key1, const void * key2);
int itzam_comparator_int64(const void * key1, const void * key2);
int itzam_comparator_uint64(const void * key1, const void * key2);
int itzam_comparator_string(const void * key1, const void * key2);

//-- a callback function used to retrieve whatever data is associated with a reference
 
typedef bool itzam_export_callback(itzam_ref ref, void ** record, itzam_int * rec_len);

//-- datafile structures
 
static const char * ITZAM_DISPLAY_VERSION = "7.0.0"; 

//-- data file header, prefix for entire data file
 
static const uint32_t  ITZAM_DATAFILE_SIGNATURE   = 0x4D5A5449; // ITZM 
static const uint32_t  ITZAM_RECORD_SIGNATURE     = 0x525A5449; // ITZR 
static const uint32_t  ITZAM_DATAFILE_VERSION     = 0x40070000; // 64.MM.MM.MM 
static const itzam_int ITZAM_DELLIST_BLOCK_SIZE   = 256;

//-- flags and masks for record identification
 
static const int32_t ITZAM_RECORD_FLAGS_GENERAL   = 0x000000ff;
static const int32_t ITZAM_RECORD_IN_USE          = 0x00000001;
static const int32_t ITZAM_RECORD_DELLIST         = 0x00000002;
static const int32_t ITZAM_RECORD_SCHEMA          = 0x00000004;
static const int32_t ITZAM_RECORD_TRAN_HEADER     = 0x00000010;
static const int32_t ITZAM_RECORD_TRAN_RECORD     = 0x00000020;

static const int32_t ITZAM_RECORD_FLAGS_BTREE     = 0x00000f00;
static const int32_t ITZAM_RECORD_BTREE_HEADER    = 0x00000100;
static const int32_t ITZAM_RECORD_BTREE_PAGE      = 0x00000200;
static const int32_t ITZAM_RECORD_BTREE_KEY       = 0x00000400;

static const int32_t ITZAM_RECORD_FLAGS_MATRIX    = 0x0000f000;
static const int32_t ITZAM_RECORD_MATRIX_HEADER   = 0x00001000;
static const int32_t ITZAM_RECORD_MATRIX_TABLE    = 0x00002000;

static const int32_t ITZAM_RECORD_FLAGS_HASH      = 0x000f0000;
static const int32_t ITZAM_RECORD_HASH_HEADER     = 0x00010000;
static const int32_t ITZAM_RECORD_HASH_TABLE      = 0x00020000;
static const int32_t ITZAM_RECORD_HASH_KEY        = 0x00040000;
static const int32_t ITZAM_RECORD_HASH_LIST_ENTRY = 0x00080000;

//-- record header, prefix for every record in a datafile
 
typedef struct t_itzam_record_header {
    uint32_t  m_signature; // four bytes that identify this as an itzam record 
    uint32_t  m_flags;     // a set of ITZAM_RECORD_* bit settings 
    itzam_int m_length;    // record length 
    itzam_int m_rec_len;   // number of bytes in use by actual data 
} itzam_record_header;

//-- structures for list of deleted records
 
typedef struct t_itzam_dellist_header {
    itzam_int   m_table_size; // number of entries in the table 
} itzam_dellist_header;

typedef struct t_itzam_dellist_entry {
    itzam_ref   m_where;    // reference for this deleted record 
    itzam_int   m_length;   // length of the deleted record 
} itzam_dellist_entry;

//-- transaction definitions
 
typedef enum {
    ITZAM_TRAN_OP_WRITE,
    ITZAM_TRAN_OP_REMOVE,
    ITZAM_TRAN_OP_OVERWRITE
}
itzam_op_type;

//-- data file header
 
typedef struct t_itzam_datafile_header {
    uint32_t  m_signature;        // file type signature 
    uint32_t  m_version;          // version of this file type 
    itzam_ref m_dellist_ref;      // position of deleted list in file 
    itzam_ref m_schema_ref;       // position of XML schema that describes this file 
    itzam_ref m_index_list_ref;   // position of index reference list 
    itzam_ref m_transaction_tail; // last item in the current transaction 
} itzam_datafile_header;

//-- transaction operation header
 
typedef struct t_itzam_op_header {
    itzam_op_type         m_type;
    itzam_ref             m_where;
    itzam_ref             m_prev_tran;
    itzam_record_header   m_record_header;
    itzam_ref             m_record_where;
} itzam_op_header;

//-- definition of shared memory used by all instances of a datafile
 
typedef struct t_itzam_datafile_shared {
    int                       m_count;             // header information 
    itzam_datafile_header     m_header;            // header information 
#if defined(ITZAM_LINUX)
    pthread_mutex_t           m_mutex;             // shared mutex 
#endif
} itzam_datafile_shared;

//-- working storage for a loaded datafile
 
typedef struct t_itzam_datafile {
    // OS file information 
    ITZAM_FILE_TYPE           m_file;              // file associated with this datafile 
    char *                    m_filename;          // filename for this datafile 

    // list of deleted records 
    itzam_dellist_header      m_dellist_header;    // header for current deleted record list 
    itzam_dellist_entry *     m_dellist;           // pointer to list of deleted records 

    // transaction tracking file information 
    char *                    m_tran_file_name;    // transaction filename 
    struct t_itzam_datafile * m_tran_file;         // transaction file 

#if defined(ITZAM_WINDOWS)
    // shared (named) mutex 
    HANDLE                    m_mutex;             // mutex information 
#endif

    // shared memory, used to communicate between threads and processes 
    ITZAM_SHMEM_TYPE          m_shmem;             // system link to shared memory 
    char *                    m_shmem_name;        // name of shared memory block 
    itzam_datafile_shared *   m_shared;            // items shared among all instances 

    // error handling 
    itzam_error_handler *     m_error_handler;     // function to handle errors that occur 

    // flags 
    bool                m_is_open;           // is the file currently open? 
    bool                m_read_only;         // is the file open read-only? 
    bool                m_file_locked;       // is the file currently locked? 
    bool                m_in_transaction;    // are we journalizing a transaction? 
    bool                m_tran_replacing;    // set when a write replaces a record during a write 

    // file locking 
#if defined(ITZAM_LINUX)
    struct flock              m_file_lock;         // fcntl lock 
#endif
} itzam_datafile;

//-- prototypes for variable-length reference data file

bool itzam_datafile_exists(const char * filename);
itzam_state itzam_datafile_create(itzam_datafile * datafile, const char * filename);
itzam_state itzam_datafile_open(itzam_datafile * datafile, const char * filename, bool recover, bool read_only);
itzam_state itzam_datafile_close(itzam_datafile * datafile);
void itzam_datafile_mutex_lock(itzam_datafile * datafile);
void itzam_datafile_mutex_unlock(itzam_datafile * datafile);
bool itzam_datafile_file_lock(itzam_datafile * datafile);
bool itzam_datafile_file_unlock(itzam_datafile * datafile);
bool itzam_datafile_is_open(itzam_datafile * datafile);
float itzam_datafile_get_version(itzam_datafile * datafile);
void itzam_datafile_set_error_handler(itzam_datafile * datafile, itzam_error_handler * error_handler);
itzam_ref itzam_datafile_tell(itzam_datafile * datafile);
itzam_state itzam_datafile_seek(itzam_datafile * datafile, itzam_ref pos);
itzam_state itzam_datafile_rewind(itzam_datafile * datafile);

// INTERNAL FUNCTIONS -- DO NOT USE EXPLICITLY 
itzam_ref itzam_datafile_get_next_open(itzam_datafile * datafile, itzam_int length);
itzam_ref itzam_datafile_write_flags(itzam_datafile * datafile, const void * data, itzam_int length, itzam_ref where, int32_t flags);
itzam_ref itzam_datafile_write(itzam_datafile * datafile, const void * data, itzam_int length, itzam_ref where);
itzam_state itzam_datafile_overwrite(itzam_datafile * datafile, const void * data, itzam_int length, itzam_ref where, itzam_int offset);
itzam_state itzam_datafile_read(itzam_datafile * datafile, void * data, itzam_int max_length);
itzam_state itzam_datafile_read_alloc(itzam_datafile * datafile, void ** data, itzam_int * length);
itzam_state itzam_datafile_remove(itzam_datafile * datafile);
itzam_state itzam_datafile_transaction_start(itzam_datafile * datafile);
itzam_state itzam_datafile_transaction_commit(itzam_datafile * datafile);
itzam_state itzam_datafile_transaction_rollback(itzam_datafile * datafile);

//-- B-tree types data structures

// page header, prefix for pages
typedef struct t_itzam_btree_page_header {
    itzam_ref  m_where;     // location in page file 
    itzam_ref  m_parent;    // parent in page file 
    uint16_t   m_key_count; // actual  # of keys 
} itzam_btree_page_header;

// a page in the B-tree
typedef struct t_itzam_btree_page {
    itzam_byte * m_data; // the actual data for this reference
    itzam_btree_page_header * m_header; // header information

    // these elements are pointers into m_data
    itzam_byte * m_keys;  // array of key/data objects 
    itzam_ref  * m_links; // links to other pages 
} itzam_btree_page;

// B-tree constants
static const uint32_t ITZAM_BTREE_VERSION        = 0x00050000; // vvvv.vv.vv
static const uint16_t ITZAM_BTREE_ORDER_MINIMUM  =  3;
static const uint16_t ITZAM_BTREE_ORDER_DEFAULT  = 25;

// B-tree header
typedef struct t_itzam_btree_header {
    uint32_t   m_version;      // version of this file structure 
    uint32_t   m_sizeof_page;  // sizeof(itzam_btree_page) used in creating this btreefile 
    uint32_t   m_sizeof_key;   // size of data being stored in pages 
    uint16_t   m_order;        // number of keys per page 
    uint64_t   m_count;        // counts number of active records 
    uint64_t   m_ticker;       // counts the total number of new records added over the life of this btree datafile
    itzam_ref  m_where;        // pointer to location of this header 
    itzam_ref  m_root_where;   // pointer to location of root reference in file 
    itzam_ref  m_schema_ref;   // links to a copy of the XML schema for this table (future use) 
} itzam_btree_header;

// working storage for a loaded B-tree
typedef struct t_itzam_btree {
    itzam_datafile *         m_datafile;          // file associated with this btree file 
    ITZAM_SHMEM_TYPE         m_shmem_header;      // memory map for the header 
    char *                   m_shmem_header_name; // name associated with memory map 
    itzam_btree_header *     m_header;            // header information 
    ITZAM_SHMEM_TYPE         m_shmem_root;        // memory map for the header 
    char *                   m_shmem_root_name;   // name associated with memory map 
    itzam_byte *             m_root_data;         // actual root data 
    itzam_btree_page         m_root;              // root structure 
    bool                     m_free_datafile;     // free the datafile object when closed 
    uint16_t                 m_links_size;        // number of links per page; order + 1; calculated at creation time 
    uint16_t                 m_min_keys;          // minimum # of keys; order / 2; calculated at creation time 
    uint16_t                 m_cursor_count;      // Number of active cursors 
    itzam_key_comparator *   m_key_comparator;    // function to compare keys 
    itzam_ref                m_saved_header;      // temporary header saved  during transaction, for use in a rollback 
} itzam_btree;

//-- prototypes for B-tree file
 
itzam_state itzam_btree_create(itzam_btree * btree, const char * filename, uint16_t order, itzam_int key_size, itzam_key_comparator * key_comparator, itzam_error_handler * error_handler);
itzam_state itzam_btree_open(itzam_btree * btree, const char * filename, itzam_key_comparator * key_comparator, itzam_error_handler * error_handler,  bool recover, bool read_only);
itzam_state itzam_btree_close(itzam_btree * btree);
uint64_t itzam_btree_count(itzam_btree * btree);
uint64_t itzam_btree_ticker(itzam_btree * btree);
void itzam_btree_mutex_lock(itzam_btree * btree);
void itzam_btree_mutex_unlock(itzam_btree * btree);
bool itzam_btree_file_lock(itzam_btree * btree);
bool itzam_btree_file_unlock(itzam_btree * btree);
bool itzam_btree_is_open(itzam_btree * btree);
void itzam_btree_set_error_handler(itzam_btree * btree, itzam_error_handler * error_handler);
bool itzam_btree_find(itzam_btree * btree, const void * search_key, void * result);
bool itzam_btree_find_before(itzam_btree * btree, const void * search_key, void * result);
bool itzam_btree_find_after(itzam_btree * btree, const void * search_key, void * result);
itzam_state itzam_btree_insert(itzam_btree * btree, const void * key);
itzam_state itzam_btree_remove(itzam_btree * btree, const void * key);
uint16_t itzam_btree_cursor_count(itzam_btree * btree);
itzam_state itzam_btree_transaction_start(itzam_btree * btree);
itzam_state itzam_btree_transaction_commit(itzam_btree * btree);
itzam_state itzam_btree_transaction_rollback(itzam_btree * btree);

//-- B-tree analysis (mostly for development purposes)
 
void itzam_btree_dump_page(itzam_btree * btree, itzam_btree_page * page, itzam_ref parent_ref);
void itzam_btree_dump_btree(itzam_btree * btree);
void itzam_btree_analyze(itzam_btree * btree, FILE * output);

//-- B-tree cursors

typedef struct t_itzam_btree_cursor {
    itzam_btree      * m_btree;
    itzam_btree_page * m_page;
    size_t             m_index;
} itzam_btree_cursor;

itzam_state itzam_btree_cursor_create(itzam_btree_cursor * cursor, itzam_btree * btree);
itzam_state itzam_btree_cursor_create_at(itzam_btree_cursor * cursor, itzam_btree * btree, const void * start_key, bool before);
bool itzam_btree_cursor_valid(itzam_btree_cursor * cursor);
itzam_state itzam_btree_cursor_free(itzam_btree_cursor * cursor);
bool itzam_btree_cursor_prev(itzam_btree_cursor * cursor);
bool itzam_btree_cursor_next(itzam_btree_cursor * cursor);
bool itzam_btree_cursor_reset(itzam_btree_cursor * cursor);
itzam_state itzam_btree_cursor_read(itzam_btree_cursor * cursor, void * returned_key);

#pragma pack(pop)

#if defined(__cplusplus)
}
#endif

#endif
