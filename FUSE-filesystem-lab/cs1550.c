#define FUSE_USE_VERSION 26

#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>

#include "cs1550.h"

// Declare helper functions (see end of file)
static struct cs1550_directory_entry * find_dir_entry(char dir_name[]);
static struct cs1550_file_entry * find_file(struct cs1550_directory_entry *, char file_name[], char extension[]);
static int check_path(const char *path);
static int get_start_block(char dir_name[]);

// Dat root block
struct cs1550_root_directory *root;
// .disk file
FILE *f;

/**
 * Called whenever the system wants to know the file attributes, including
 * simply whether the file exists or not.
 *
 * `man 2 stat` will show the fields of a `struct stat` structure.
 */
static int cs1550_getattr(const char *path, struct stat *statbuf)
{	
	// Clear out `statbuf` first -- this function initializes it.
	memset(statbuf, 0, sizeof(struct stat));

	//Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	// Check if the path is the root directory.
	if (strcmp(path, "/") == 0) 
	{
		statbuf->st_mode = S_IFDIR | 0755;
		statbuf->st_nlink = 2;

		return 0;
	}

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);
	int size;

	// Check if the path is a file.
	if (res == 2 || res == 3) 
	{
		// Regular file
		statbuf->st_mode = S_IFREG | 0666;
	
		// Only one hard link to this file
		statbuf->st_nlink = 1;

		// Attempt to find match in root block
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}
		else
		{
			// If match attempt to find a matching file
			struct cs1550_file_entry *matching_file = find_file(matching_directory, filename, extension);
			if(!matching_file)
			{
				free(matching_directory);
				return -ENOENT;
			}
			else
			{
				size = matching_file->fsize;
			}
		}

		// Determine file size
		statbuf->st_size = size;
		free(matching_directory);
		return 0;
	}

	// Check if path is a subdir.
	if (res == 1) 
	{
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}
		else
		{
			statbuf->st_mode = S_IFDIR | 0755;
			statbuf->st_nlink = 2;
			free(matching_directory);
			return 0;
		}
	}

	// Otherwise, the path doesn't exist.
	return -ENOENT;
}

/**
 * Called whenever the contents of a directory are desired. Could be from `ls`,
 * or could even be when a user presses TAB to perform autocompletion.
 */
static int cs1550_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			  off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	// Parsing data
	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);

	// Check path to find directory
	if (strcmp(path, "/") == 0)
	{
		// Add the current and parent directories
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);

		// If root list subdirs
		for (size_t i = 0; i < root->num_directories; i++) 
		{
			filler(buf, root->directories[i].dname, NULL, 0);
		}
		return 0;
	}
	else if(res == 1)
	{
		// Add the current and parent directories
		filler(buf, ".", NULL, 0);
		filler(buf, "..", NULL, 0);

		// If res = 1, then we are in a subdir
		// Attempt to find matching dir in root block
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
		
			return -ENOENT;
		}
		// If we find a match list all files
		else
		{
			// Make an array for the filename + extension. Set the size to max filename + 1 char for . + max extension + 1 char for \0
			char file[MAX_FILENAME + MAX_EXTENSION + 2];
			for (size_t i = 0; i < matching_directory->num_files; i++) 
			{
				// Copy the filename to array
				strncpy(file, matching_directory->files[i].fname, (MAX_FILENAME + 1));
				// Check if file extension exists and add extension
				if(strcmp(matching_directory->files[i].fext, "") != 0)
				{
					strncat(file, ".", 2);
					strncat(file, matching_directory->files[i].fext, (MAX_EXTENSION + 1));
				}
				
				// Write changes
				filler(buf, file, NULL, 0);
			}
			free(matching_directory);
			return 0;
			
		}
	}
	else
	{
		//Return -ENOTDIR if the path is not to a directory
		return -ENOTDIR;	
	}
}

/**
 * Creates a directory. Ignore `mode` since we're not dealing with permissions.
 */
static int cs1550_mkdir(const char *path, mode_t mode)
{
	(void) mode;

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];
	
	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);

	// Make a directory in the root
	if (res == 1) 
	{
		// Loop through dirs and check if any of their names match the directory name. If so, return -EEXIST
		for (size_t i = 0; i < root->num_directories; i++)
		{
			if (strcmp(directory, root->directories[i].dname) == 0)
			{
				return -EEXIST;
			}
		}

		// Check to make sure there is space for new dir
		if (root->num_directories >= MAX_DIRS_IN_ROOT)
		{
			return -ENOSPC;
		}
		else
		{
			// Copy the new directory name into the next index if dir does NOT exist and there is space.
			strncpy(root->directories[root->num_directories].dname, directory, (MAX_FILENAME + 1));
			
			// Set the starting block of the new directory to the last allocated block
			// Increment num of dirs and last block allocated
			// Find start of file then write changes
			root->directories[root->num_directories].n_start_block = root->last_allocated_block + 1;
			root->num_directories++;
			root->last_allocated_block++;
			fseek(f, 0, SEEK_SET);
			fwrite(root, BLOCK_SIZE, 1, f);
			return 0;
		}
	}

	return -EPERM;
}

/**
 * Removes a directory.
 */
static int cs1550_rmdir(const char *path)
{
	(void) path;
	return 0;
}

/**
 * Does the actual creation of a file. `mode` and `dev` can be ignored.
 */
static int cs1550_mknod(const char *path, mode_t mode, dev_t dev)
{
	(void) mode;
	(void) dev;

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];

	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);
	
	// Make sure there are 2-3 args.
	if (res == 2 || res == 3)
	{
		// Try to find matching dir in root block
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}

		else
		{
			// If we find a matching dir attempt to find a matching file. If file does NOT exist
			// attempt to create new one. Check to make sure there's enough space tho.
			struct cs1550_file_entry *matching_file = find_file(matching_directory, filename, extension);

			if(!matching_file)
			{
				if(matching_directory->num_files < MAX_FILES_IN_DIR)
				{
					int start_block = get_start_block(directory);

					//Copy file data into the next free file and add extension if needed
					strncpy(matching_directory->files[matching_directory->num_files].fname, filename, (MAX_FILENAME + 1));
					
					if(res == 3)
					{
						strncpy(matching_directory->files[matching_directory->num_files].fext, extension, (MAX_EXTENSION + 1));

					}
					matching_directory->files[matching_directory->num_files].fsize = 0;
					matching_directory->files[matching_directory->num_files].n_index_block = root->last_allocated_block + 1;

					//Seek to and read the index block into a variable of type struct cs1550_index_block. Write the value of the last_allocated_block
					// as the first entry in the index block array. Increment last_allocated_block for the first data block of the file
					struct cs1550_index_block *index = malloc(sizeof(struct cs1550_index_block));
					memset(index, 0, sizeof(struct cs1550_index_block));
					fseek(f, ((root->last_allocated_block + 1) * BLOCK_SIZE), SEEK_SET);
					fread(index, BLOCK_SIZE, 1, f);
					root->last_allocated_block++;
					index->entries[0] = root->last_allocated_block + 1;
					root->last_allocated_block++;
					matching_directory->num_files++;

					//Write changes to dir and root back to disk
					fseek(f, start_block * BLOCK_SIZE, SEEK_SET);
					fwrite(matching_directory, BLOCK_SIZE, 1, f);
					fseek(f, 0, SEEK_SET);
					fwrite(root, BLOCK_SIZE, 1, f);
					fseek(f, ((root->last_allocated_block - 1) * BLOCK_SIZE), SEEK_SET);
					fwrite(index, BLOCK_SIZE, 1, f);

					free(matching_directory);
					free(matching_file);
					free(index);
					return 0;
				}

				// Not enough space
				else
				{
					free(matching_directory);
					return -ENOSPC;
				}
			}

			// File already exists
			else
			{
				free(matching_directory);
				return -EEXIST;
			}
		}
	}
	else
	{
		return -EPERM;
	}
}

/**
 * Deletes a file.
 */
static int cs1550_unlink(const char *path)
{
	(void) path;
	return 0;
}

/**
 * Read `size` bytes from file into `buf`, starting from `offset`.
 */
static int cs1550_read(const char *path, char *buf, size_t size, off_t offset,
		       struct fuse_file_info *fi)
{
	(void) fi;

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];

	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);
	
	// Check for path/filename
	if(res == 2 || res == 3)
	{
		// Look for matching dir in root block
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}

		else
		{
			// Matching dir?
			struct cs1550_file_entry *matching_file = find_file(matching_directory, filename, extension);
			if(!matching_file)
			{
				free(matching_directory);
				return -ENOENT;
			}
			
			else
			{
				//Read the index and data blocks
				struct cs1550_index_block *index = malloc(sizeof(struct cs1550_index_block));
				fseek(f, (matching_file->n_index_block * BLOCK_SIZE), SEEK_SET);
				fread(index, BLOCK_SIZE, 1, f);

				struct cs1550_data_block *data = malloc(sizeof(struct cs1550_data_block));

				size_t temp_size = 0;
				while(temp_size != size)
				{
					
					// Determine the index inside the array of data blocks inside the index block (offset / block size)
					// Then calc curr offset and num of bytes to read
					int curr_index = (offset + temp_size) / BLOCK_SIZE;
					int curr_offset = (offset + temp_size) % BLOCK_SIZE;
					int curr_size = BLOCK_SIZE;

					if((size - temp_size) < BLOCK_SIZE)
					{
						if((size - temp_size) + curr_offset > BLOCK_SIZE)
						{
							curr_size = BLOCK_SIZE - curr_offset;
						}
						
						else
						{
							curr_size = size - temp_size;

						}
					}

					// If the index empty skip reading data
					if(index->entries[curr_index] == 0)
					{
						temp_size += curr_size;
						continue;
					}

					// Seek to curr offset, read block in, copy data, increment bytes copied
					fseek(f, (index->entries[curr_index] * BLOCK_SIZE), SEEK_SET);
					fread(data, BLOCK_SIZE, 1, f);
					memcpy(buf + temp_size, ((char*)data) + curr_offset, curr_size);
					temp_size += curr_size;

				}
				
				// If you need to read the next data blocks, retrieve the block numbers
				free(matching_directory);
				free(index);
				free(data);
				return size;
			}
		}
	}

	else
	{
		return -EISDIR;
	}
}

/**
 * Write `size` bytes from `buf` into file, starting from `offset`.
 */
static int cs1550_write(const char *path, const char *buf, size_t size,
			off_t offset, struct fuse_file_info *fi)
{
	(void) fi;

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];

	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);
	// Check for path/filename
	if(res == 2 || res == 3)
	{
		// Look for matching dir in the root block
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}
		else
		{
			// Look for matching file if matching dir and return an error if file does NOT exist
			struct cs1550_file_entry *matching_file = find_file(matching_directory, filename, extension);
			if(!matching_file)
			{
				free(matching_directory);
				return -ENOENT;
			}

			else
			{
				//Read the index/data blocks
				struct cs1550_index_block *index = malloc(sizeof(struct cs1550_index_block));
				fseek(f, (matching_file->n_index_block * BLOCK_SIZE), SEEK_SET);
				fread(index, BLOCK_SIZE, 1, f);

				struct cs1550_data_block *data = malloc(sizeof(struct cs1550_data_block));

				size_t temp_size = 0;
				while(temp_size != size)
				{
					// Use the offset to determine the index
					// Calc offset for curr data block and amount of bytes to read
					int curr_index = (offset + temp_size) / BLOCK_SIZE;
					int curr_offset = (offset + temp_size) % BLOCK_SIZE;
					int curr_size = BLOCK_SIZE;
					if((size - temp_size) < BLOCK_SIZE)
					{
						if((size - temp_size) + curr_offset > BLOCK_SIZE)
						{
							curr_size = BLOCK_SIZE - curr_offset;
						}
						else
						{
							curr_size = size - temp_size;

						}
					}

					// If the index entry is empty, allocate a new block if there's space
					// Write changes back to disk
					if(index->entries[curr_index] == 0)
					{
						index->entries[curr_index] = root->last_allocated_block + 1;
						root->last_allocated_block++;
						fseek(f, 0, SEEK_SET);
						fwrite(root, BLOCK_SIZE, 1, f);
						fseek(f, (matching_file->n_index_block * BLOCK_SIZE), SEEK_SET);
						fwrite(index, BLOCK_SIZE, 1, f);
					}

					// Read in the current data block and copy contents. Write change back
					fseek(f, (index->entries[curr_index] * BLOCK_SIZE), SEEK_SET);
					fread(data, BLOCK_SIZE, 1, f);
					memcpy(((char*)data) + curr_offset, buf + temp_size, curr_size);
					fseek(f, (index->entries[curr_index] * BLOCK_SIZE), SEEK_SET);
					fwrite(data, BLOCK_SIZE, 1, f);
					temp_size += curr_size;

				}
				
				free(index);
				free(data);
				
				// Increment file size and write changes
				if(offset == 0)
					matching_file->fsize = size;

				else
					matching_file->fsize += size;

				fseek(f, (get_start_block(directory) * BLOCK_SIZE), SEEK_SET);
				fwrite(matching_directory, BLOCK_SIZE, 1, f);
				free(matching_directory);
				return size;
			}
		}
	}
	
	else
	{
		return -EISDIR;
	}
}

/**
 * Called when a new file is created (with a 0 size) or when an existing file
 * is made shorter. We're not handling deleting files or truncating existing
 * ones, so all we need to do here is to initialize the appropriate directory
 * entry.
 */
static int cs1550_truncate(const char *path, off_t size)
{
	(void) path;
	(void) size;
	return 0;
}

/**
 * Called when we open a file.
 */
static int cs1550_open(const char *path, struct fuse_file_info *fi)
{
    (void) fi;

	char directory[MAX_FILENAME + 1];
	char filename[MAX_FILENAME + 1];
	char extension[MAX_EXTENSION + 1];

	// Check if the path is valid
	if(check_path(path) == 0)
	{
		return -ENAMETOOLONG;
	}

	int res;
	res = sscanf(path, "/%[^/]/%[^.].%s", directory, filename, extension);

	if(res == 1)
	{
		// Attempt to find matching directory in the root block. Return error if NOT found, return 0 if found.
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);

		if(!matching_directory)
		{
			return -ENOENT;
		}

		else
		{
			return 0;
		}
	}

	// Attempt to find matching dir in root block then matching file.
	else if(res == 2 || res == 3)
	{
		struct cs1550_directory_entry *matching_directory = find_dir_entry(directory);
		if(!matching_directory)
		{
			return -ENOENT;
		}
		else
		{
			struct cs1550_file_entry *matching_file = find_file(matching_directory, filename, extension);

			if(!matching_file)
			{
				return -ENOENT;
			}

			else
			{
				return 0;
			}
		}
	}

	// Else path is invalid	
	else
	{
		return -ENOENT;
	}
}

/**
 * Called when close is called on a file descriptor, but because it might
 * have been dup'ed, this isn't a guarantee we won't ever need the file
 * again. For us, return success simply to avoid the unimplemented error
 * in the debug log.
 */
static int cs1550_flush(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	(void) fi;
	return 0;
}

/**
 * This function should be used to open and/or initialize your `.disk` file.
 */
static void *cs1550_init(struct fuse_conn_info *fi)
{
	(void) fi;

	//Read in first disk block(root)
	root = malloc(BLOCK_SIZE);
	f = fopen(".disk", "rb+");
	if (f != NULL)
	{
		fread(root, BLOCK_SIZE, 1, f);
	}
	return NULL;
}

/**
 * This function should be used to close the `.disk` file.
 */
static void cs1550_destroy(void *args)
{
	(void) args;

	// Free teh node! ...And close the .disk file
	free(root);
	fclose(f);
}

/*
 * Register our new functions as the implementations of the syscalls.
 */
static struct fuse_operations cs1550_oper = {
	.getattr	= cs1550_getattr,
	.readdir	= cs1550_readdir,
	.mkdir		= cs1550_mkdir,
	.rmdir		= cs1550_rmdir,
	.read		= cs1550_read,
	.write		= cs1550_write,
	.mknod		= cs1550_mknod,
	.unlink		= cs1550_unlink,
	.truncate	= cs1550_truncate,
	.flush		= cs1550_flush,
	.open		= cs1550_open,
	.init		= cs1550_init,
	.destroy	= cs1550_destroy,
};

/*
 * Don't change this.
 */
int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &cs1550_oper, NULL);
}

/**************
*             *
*	HELPERZ   *
*             *
**************/


// Loop root block and return starting block of a dir
static int get_start_block(char dir_name[])
{
	for (size_t i = 0; i < root->num_directories; i++)
	{
		if (strcmp(dir_name, root->directories[i].dname) == 0)
		{
			return root->directories[i].n_start_block;
		}
	}
	return 0;
}

// Loop root block and return dir entry match
static struct cs1550_directory_entry * find_dir_entry(char dir_name[])
{
	for (size_t i = 0; i < root->num_directories; i++)
	{
		if (strcmp(dir_name, root->directories[i].dname) == 0)
		{
			int start = root->directories[i].n_start_block;
			
			struct cs1550_directory_entry *dir = malloc(sizeof(struct cs1550_directory_entry));
			
			fseek(f, start * BLOCK_SIZE, SEEK_SET);
			fread(dir, BLOCK_SIZE, 1, f);
			return dir;
		}
	}
	
	return NULL;
}


// Loop and return a the matching file if exists
static struct cs1550_file_entry * find_file(struct cs1550_directory_entry *dir, char file_name[], char extension[])
{
	for (size_t i = 0; i < dir->num_files; i++)
	{
		if((strcmp(file_name, dir->files[i].fname) == 0) && (strcmp(extension, dir->files[i].fext) == 0))
		{
			return &(dir->files[i]);
		}
	}
	
	return NULL;
}

// Loop through path to check length of argz
static int check_path(const char *path)
{
	int valid_dir = 0;
	int delimiter_index = 0;
	int valid_file = 0;
	
	for(int i = 1; i < (MAX_FILENAME + 2); i++)
	{
		if(path[i] == '/')
		{
			valid_dir = 1;
			delimiter_index = i;
			break;
		}
		
		else if(path[i] == '\0')
		{
			return 1;
		}		
	}

	// Check validity of dir before continuing...
	if(valid_dir == 0)
	{
		return 0;
	}

	for(int i = delimiter_index + 1; i < (delimiter_index + MAX_FILENAME + 2); i++)
	{
		if(path[i] == '.')
		{
			valid_file = 1;
			delimiter_index = i;
			break;
		}
		
		else if(path[i] == '\0')
		{
			return 1;
		}
	}

	if(valid_file == 0)
	{
		return 0;
	}

	for(int i = delimiter_index + 1; i < (delimiter_index + MAX_EXTENSION + 2); i++)
	{
		if(path[i] == '\0')
		{
			return 1;
		}
	}

	return 0;

}