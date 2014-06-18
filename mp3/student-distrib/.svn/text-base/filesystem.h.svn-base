#ifndef _FILE_SYSTEM_H
#define _FILE_SYSTEM_H

#include "types.h"
#include "lib.h"
#include "system_calls.h"

#define BLOCK_SIZE 4096 /* size of absolute blocks in file system */
#define BB_ENTRY_SIZE 64 /* Size of each boot block entry */




/* Directory entry inside boot block */
typedef struct dir_entry
{
	uint8_t file_name[32];
	uint32_t file_type;
	uint32_t inode_id;
	uint8_t reserved[24];

} dir_entry_t;

/* the boot block - first block of file system */
typedef struct boot_block
{
	uint32_t num_dir_entries;
	uint32_t num_inodes;
	uint32_t num_data_blocks;
	uint8_t reserved[52];
	dir_entry_t dir[63];

} boot_block_t;


/* index node inside file system */
//typedef struct inode
struct inode
{
	uint32_t length;
	uint32_t data_block_index[1023];

}; //inode_t;

/* to be moved to generic system call file later */
/*
typedef struct file_desc 
{
	// RESERVED: file operations table pointer
	inode_t * inode_ptr;
	uint32_t file_position;
	uint32_t flags;
} file_desc_t;
*/

/*
typedef struct fs_dir_operations
{
	int32_t (*fs_open)(const uint8_t* filename);
	int32_t (*fs_read_dir)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fs_write)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fs_close)(int8_t* filename);
} fs_dir_fops_t;


typedef struct fs_file_operations
{
	int32_t (*fs_open)(const uint8_t* filename);
	int32_t (*fs_read_file)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fs_write)(int32_t fd, void* buf, int32_t nbytes);
	int32_t (*fs_close)(int8_t* filename);
} fs_file_fops_t;
*/



extern void load_fs_start(uint32_t addr);
extern int32_t get_fs_start_addr(void);

extern int32_t fs_open(const uint8_t* filename);
extern int32_t fs_read_dir(int32_t fd, void* buf, int32_t count);
extern int32_t fs_read_file(int32_t fd, void* buf, int32_t count);
extern int32_t fs_write(int32_t fd, void* buf, int32_t count);
extern int32_t fs_close(int32_t fd);

extern int32_t read_dentry_by_name(const uint8_t* fname, dir_entry_t* dentry);
extern int32_t read_dentry_by_index(uint32_t index, dir_entry_t* dentry);
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern void * get_file_address(uint32_t index);
extern uint32_t get_file_size(uint32_t index);





#endif
