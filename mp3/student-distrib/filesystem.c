#include "filesystem.h"

static uint32_t fs_start_addr; // 0x0040E000

static boot_block_t * bb;
// static file_desc_t file_desc; // moved back from sys calls /* file descriptor of current file */
//static dir_entry_t dentry;

//pcb_t * pcb;
// file_desc_t file_desc;


#define FILE_NAME_SIZE 32 /* size of file name inside file system directory entry */
#define TYPE_RTC 0
#define TYPE_DIR 1
#define TYPE_FILE 2




/*
 * load_fs_start
 *   DESCRIPTION: Sets the file system start address.
 *   INPUTS: addr -- file system start address
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *	 SIDE EFFECTS: none
 */ 
void load_fs_start(uint32_t addr)
{
	fs_start_addr = addr;
}

/*
 * get_fs_start_addr
 *   DESCRIPTION: Returns the filesystem starting address
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: the filesystem starting address
 *	 SIDE EFFECTS: none
 */ 
int32_t get_fs_start_addr(void)
{
	return fs_start_addr;
}

/*
 * fs_open
 *   DESCRIPTION: "Opens" file system by returning file system file descriptor.
 *   INPUTS: filename -- file name of file to read data from
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail (invalid filename or pointers)
 *	 SIDE EFFECTS: none
 */ 
int32_t fs_open(const uint8_t* filename)
{
	dir_entry_t dentry;

	bb = (boot_block_t *)(fs_start_addr); /* boot block points to start of file system */

	/* return fail if read dentry fails */
 	if( -1 == read_dentry_by_name( filename, &dentry) )
 		return -1;

/*

 	// inode pointer should be null if the file is a directory or RTC 
 	if(dentry.file_type == TYPE_RTC || dentry.file_type == TYPE_DIR)
 	{
 		file_desc.inode_ptr = NULL;
 	}
 	// points to an inode if it's a file 
 	else
 	{
 		file_desc.inode_ptr = (inode_t *)(fs_start_addr + (dentry.inode_id+1)*BLOCK_SIZE);
 	}
*/
	// Find a spot in the unnamed/fd array to put our new file
/* 	while( pcb->file[fd].file_ops != NULL || fd != 8)
 	{
 		fd++;
 	}


 	// continue if we have a valid file descriptor
 //	if( fd < 8)
 	{
		
		// if it's not a fs descriptor, we want to open it
		// infinite fs_open if we don't check this
		if( dentry.file_type != TYPE_DIR || dentry.file_type != TYPE_FILE )
		{
 			pcb->file[fd].file_ops->open(filename);
 		}
 	}
 
 	if( fd >= 8)
 	{
 		fd = -1;
 	}
 	else
 	{
 		 // set our pcb file desc entry to what we just filled
		pcb->file[fd] = file_desc;
 	}

*/
	return 0;
}



/*
 * fs_read_dir
 *   DESCRIPTION: Reads from file system directory data and inserts into buffer parameter.
 *   INPUTS: buf -- file name of file to read data from
 *			 count -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: buffer filled with read data
 *	 SIDE EFFECTS: buf gets destroyed.
 */ 
int32_t fs_read_dir(int32_t fd, void* buf, int32_t count)
{
	uint32_t bytes_read;
	bytes_read = 0;
	buf = (uint8_t *)buf;

	
	if(active_terminal->top_pcb->file[fd].flags == FILE_OPEN)
	{

		/* ensure it is a directory */
		if( active_terminal->top_pcb->file[fd].inode_index >= 0) // if(dentry.file_type == TYPE_DIR)
		{
			/* makes sure we don't read into the nether regions */
			if(active_terminal->top_pcb->file[fd].file_position < bb->num_dir_entries)
			{
				/* buffer is filled with next directory entry name */
				strncpy( (int8_t *)buf, (int8_t *)bb->dir[active_terminal->top_pcb->file[fd].file_position].file_name, FILE_NAME_SIZE);
				active_terminal->top_pcb->file[fd].file_position++;
				bytes_read = strlen( (int8_t*)buf);
			}
			else
			{
				/* do nothing? we read 0 bytes */
				bytes_read = 0;
			}
		}
	}


	return bytes_read;
}

/*
 * fs_read_file
 *   DESCRIPTION: Reads from file system data and inserts into buffer parameter.
 *   INPUTS: fd -- file descriptor
 *			 buf -- file name of file to read data from
 *			 count -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: buffer filled with read data
 *	 SIDE EFFECTS: buf gets destroyed.
 */ 
int32_t fs_read_file(int32_t fd, void* buf, int32_t count)
{
	uint32_t bytes_read;
	bytes_read = 0;
	buf = (uint8_t *)buf;

	//cli();

	if( active_terminal->top_pcb->file[fd].flags == FILE_OPEN )
	{
		/* ensure it is a file */
		if( active_terminal->top_pcb->file[fd].inode_index >= 0) // if(dentry.file_type == TYPE_DIR)
		{
			/* buffer is filled with data from file */
			bytes_read = read_data( active_terminal->top_pcb->file[fd].inode_index, active_terminal->top_pcb->file[fd].file_position, buf, count);

			if( bytes_read == -1)
			{
				//sti();
				return bytes_read;
			}
			if( bytes_read == 0)
			{

				// pcb->file[fd].file_position = 0;
				active_terminal->top_pcb->file[fd].file_position = 0;
				
				//sti();
				return bytes_read;
			}
			// update the file position
			active_terminal->top_pcb->file[fd].file_position += bytes_read;

		}
	}

	//sti();
	return bytes_read;
}



/*
 * fs_write
 *   DESCRIPTION: Returns -1 on fail. It will always fail
 *                because we're using a read-only file system.
 *   INPUTS: fd -- not used
 *			 buf -- not used
 *			 count -- not used
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on fail (always)
 *	 SIDE EFFECTS: none
 */ 
int32_t fs_write(int32_t fd, void* buf, int32_t count)
{
	return -1;
}

/*
 * fs_close
 *   DESCRIPTION: Returns 0 on success. It will always succeed
 *                because we don't really close anything.
 *   INPUTS: fd -- not used
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success (always)
 *	 SIDE EFFECTS: none
 */ 
int32_t fs_close(int32_t fd)
{
	return 0;
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: Fills the directory entry given we know the name of the directory.
 *   INPUTS: fname -- name of desired directory entry
 *			 dentry -- directory entry struct to fill
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail (name not found or fname invalid)
 *	 SIDE EFFECTS: dentry is altered.
 */ 
int32_t read_dentry_by_name(const uint8_t* fname, dir_entry_t* dentry)
{
	bb = (boot_block_t *)(fs_start_addr); /* boot block points to start of file system */

	if(fname == NULL || dentry == NULL || bb == NULL)
	{
		return -1;
	}

	uint32_t count = 0;


	//printf("inside read_dentry_by_name 1\n");

	/* Compare the current string with the parameter string to
	 * see if we found the correct dir entry.
	 *
	 * Also make sure we are within the bounds of the boot block.
	*/
	while( (count < bb->num_dir_entries) && ( strncmp( (int8_t *)bb->dir[count].file_name, (int8_t *)fname, FILE_NAME_SIZE) != 0) )
	{
		//printf("bb->dir[count].file_name=%s\n",bb->dir[count].file_name);
		count++;
	}
	if(count >= bb->num_dir_entries)
	{
		//printf("read dentry by name not found =(\n");
		return -1; /* name not found */
	}

	//printf("inside read_dentry_by_name 2\n");

	/* fill dir entry with file name, file type, and node id */
	strncpy( (int8_t *)dentry->file_name, (int8_t *)bb->dir[count].file_name, FILE_NAME_SIZE);
	dentry->file_type = bb->dir[count].file_type;
	dentry->inode_id = bb->dir[count].inode_id;

//	printf("dentry->file_name=%s\n",dentry->file_name);

	//printf("inside read_dentry_by_name 3\n");

	return 0;

}

/*
 * read_dentry_by_index
 *   DESCRIPTION: Fills the directory entry given we know the index of the directory.
 *   INPUTS: index -- index of desired directory entry
 *			 dentry -- directory entry struct to fill
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on fail (index not found)
 *	 SIDE EFFECTS: dentry is altered.
 */ 
int32_t read_dentry_by_index(uint32_t index, dir_entry_t* dentry)
{
	bb = (boot_block_t *)(fs_start_addr); /* boot block points to start of file system */
	uint32_t count = 0;

	/* Compare the current inode with the parameter inode to
	 * see if we found the correct dir entry.
	 *
	 * Also make sure we are within the bounds of the boot block.
	*/
	while( ( bb->dir[count].inode_id != index) && (count < bb->num_dir_entries) )
	{
		count++;
	}
	if(count >= bb->num_dir_entries)
	{
		return -1; /* node index not found */
	}
	
	/* fill dir entry with file name, file type, and node id */
	strncpy( (int8_t *)dentry->file_name, (int8_t *)bb->dir[count].file_name, FILE_NAME_SIZE);
	dentry->file_type = bb->dir[count].file_type;
	dentry->inode_id = bb->dir[count].inode_id;

	return 0;

}

/*
 * read_data
 *   DESCRIPTION: Fills the buffer with data read from file with node index given.
 *   INPUTS: inode -- index of node that contains the data block information
 *			 offset -- start byte of read in file
 *			 buf -- buffer that we fill with read data
 *			 length -- how many bytes to copy
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes successfully read
 *				   -1 on fail (bad data block/pointer)
 *	 SIDE EFFECTS: buf is altered.
 */ 
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	inode_t *inode_ptr;
	uint8_t *d_block_ptr;
	uint8_t d_block_index;
	uint32_t num_read;
	

	/* set the boot block */
	bb = (boot_block_t *)(fs_start_addr);
	/* set the inode */
	inode_ptr = (inode_t *)(fs_start_addr + (inode + 1) * BLOCK_SIZE);
	
	d_block_index = offset / BLOCK_SIZE;
	d_block_ptr = (uint8_t *)(fs_start_addr + (bb->num_inodes + 1 + inode_ptr->data_block_index[d_block_index]) * BLOCK_SIZE);
	
	for(num_read = 0; num_read < length; num_read++) {
		/* Reached the end of the file */
		if (offset + num_read >= inode_ptr->length)
			return num_read;
		/* Reached the end of the block, get the next one */
		if ((offset + num_read) % BLOCK_SIZE == 0) {
			d_block_index = (offset + num_read) / BLOCK_SIZE;
			d_block_ptr = (uint8_t *)(fs_start_addr + (bb->num_inodes + 1 + inode_ptr->data_block_index[d_block_index]) * BLOCK_SIZE);		

		}
		/* bad data block */
		if (d_block_ptr == NULL)
			return -1;
		
		/* copy the data */
		buf[num_read] = d_block_ptr[(offset+num_read) % BLOCK_SIZE];
	}
	
	return num_read;
}

/*
 * read_data
 *   DESCRIPTION: Fills the buffer with data read from file with node index given.
 *   INPUTS: inode -- index of node that contains the data block information
 *			 offset -- start byte of read in file
 *			 buf -- buffer that we fill with read data
 *			 length -- how many bytes to copy
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes successfully read
 *				   -1 on fail (bad data block/pointer)
 *	 SIDE EFFECTS: buf is altered.
 */ 
//int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
//{
//	inode_t * inode_ptr;
//	uint8_t * d_block_ptr;
//	uint32_t i, j; /* indices used for filling buffer */
//	uint32_t num_read = 0; /* number of bytes successfully read and put in buffer */
//	uint32_t block_index = 0; /* keeps track of which block we're reading data from */
//
//	/* get the desired inode */
////	inode_ptr = (inode_t *)(FS_START_ADDR + (inode+1)*BLOCK_SIZE); // replaced by file_desc.inode_ptr
////	printf("read_data\n");
//
//	/* gets the desired data block */
//	/* we might need +1 on data block index offset */
//
//	bb = (boot_block_t *)(fs_start_addr); /* boot block points to start of file system */
//	
//	inode_ptr = (inode_t *)( (uint32_t)bb + (inode+1)*BLOCK_SIZE);
//
//
//	// find the correct first data block index
//	block_index = offset / BLOCK_SIZE;
//
//
////	d_block_ptr = (uint8_t *)(fs_start_addr + (bb->num_inodes+1)*BLOCK_SIZE + (file_desc.inode_ptr->data_block_index[block_index]
////		)*BLOCK_SIZE);
//	d_block_ptr = (uint8_t *)(fs_start_addr + (bb->num_inodes+1)*BLOCK_SIZE + (inode_ptr->data_block_index[block_index]
//		)*BLOCK_SIZE);
//
//	block_index++;
//
//	/* bad data block */
//	if( d_block_ptr == NULL)
//		return -1;
//
////	printf("offset=%d, length=%d\n",offset,length);
//
//
//
//	for(i=offset, j=0; j < length; i++, j++)
//	{
//		if( inode_ptr->length != 0 && i >= inode_ptr->length)
//			break;
//
//		if(i != offset && ((num_read+offset) % BLOCK_SIZE) == 0)
//		{
//			/* data block pointer points to next data block in file system */
//			d_block_ptr = (uint8_t *)(fs_start_addr + (bb->num_inodes+1)*BLOCK_SIZE + (inode_ptr->data_block_index[block_index]
//				)*BLOCK_SIZE);
//			block_index++;
//			/* bad data block */
//			if( d_block_ptr == NULL)
//				return -1;
//		}
//		
//		buf[j] = d_block_ptr[i];
//		num_read++;
//
//		//printf("read_data: num_read=%d\n",num_read);
//	}
//
//	return num_read;
//}


/*
 * get_file_address
 *   DESCRIPTION: Returns the file system address of the first data block given an inode index
 *   INPUTS: index -- the index of inode of interest
 *   OUTPUTS: none
 *   RETURN VALUE: address of the first data block of an inode
 *	 SIDE EFFECTS: none
 */ 
void * get_file_address(uint32_t index)
{
	inode_t * inode_ptr;
	inode_ptr = (inode_t *)((uint32_t)bb + (index+1)*BLOCK_SIZE);
	return (uint8_t *)(fs_start_addr + (bb->num_inodes+1)*BLOCK_SIZE + (inode_ptr->data_block_index[0])*BLOCK_SIZE);
}


/*
 * get_file_size
 *   DESCRIPTION: Returns the number of bytes that an inode's data blocks consist of
 *   INPUTS: index -- the index of inode of interest
 *   OUTPUTS: none
 *   RETURN VALUE: number of data block bytes
 *	 SIDE EFFECTS: none
 */  
uint32_t get_file_size(uint32_t index)
{
	inode_t * inode_ptr;
	inode_ptr = (inode_t *)((uint32_t)bb + (index+1)*BLOCK_SIZE);
	return inode_ptr->length;
}

