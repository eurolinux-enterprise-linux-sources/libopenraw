/*
 * libopenraw - posix_io.c
 *
 * Copyright (C) 2005 Hubert Figuiere
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include "io_private.h"
#include "posix_io.h"


/** private data to be store in the _RawFile */
struct io_data_posix {
	/** POSIX fd returned by open() */
	int fd;
};

static IOFileRef posix_open(const char *path, int mode);
static int posix_close(IOFileRef f);
static int posix_seek(IOFileRef f, off_t offset, int whence);
static int posix_read(IOFileRef f, void *buf, size_t count);
static off_t posix_filesize(IOFileRef f);
static void *posix_mmap(IOFileRef f, size_t length, off_t offset);
static int posix_munmap(IOFileRef f, void *addr, size_t length);

/** posix io methods instance. Constant. */
struct io_methods posix_io_methods = {
	&posix_open,
	&posix_close,
	&posix_seek,
	&posix_read,
	&posix_filesize,
	&posix_mmap,
	&posix_munmap
};


/** posix implementation for open() */
static IOFileRef posix_open(const char *path, int mode)
{
	struct io_data_posix *data = 
		(struct io_data_posix *)malloc(sizeof(struct io_data_posix));
	IOFileRef f = (IOFileRef)malloc(sizeof(struct _IOFile));

	memset(f, 0, sizeof(struct _IOFile));
	memset(data, 0, sizeof(struct io_data_posix));
	
	f->methods = &posix_io_methods;
	f->_private = data;
	f->path = strdup(path);
	data->fd = open(path, mode);
	if (data->fd == -1) {
		free(data);
		free(f);
		f = NULL;
	}


	return f;
}



/** posix implementation for close() */
static int posix_close(IOFileRef f)
{
	int retval = 0;
	struct io_data_posix *data = (struct io_data_posix*)f->_private;

	retval = close(data->fd);
	free(data);
	free(f->path);
	return retval;
}


/** posix implementation for seek() */
static int posix_seek(IOFileRef f, off_t offset, int whence)
{
	int retval = 0;
	struct io_data_posix *data = (struct io_data_posix*)f->_private;

	retval = lseek(data->fd, offset, whence);
	if (retval == -1) {
		f->error = errno;
	}
	else {
		f->error = 0;
	}
	return retval;
}


/** posix implementation for read() */
static int posix_read(IOFileRef f, void *buf, size_t count)
{
	int retval = 0;
	struct io_data_posix *data = (struct io_data_posix*)f->_private;

	retval = read(data->fd, buf, count);
	if (retval == -1) {
		f->error = errno;
	}
	else {
		f->error = 0;
	}
	return retval;
}


static off_t posix_filesize(IOFileRef f)
{
	off_t size = -1;
	struct io_data_posix *data = (struct io_data_posix*)f->_private;
	struct stat sb;
    
	if(fstat(data->fd, &sb) >= 0) {
		size = sb.st_size;
	}

	return size;
}



static void *posix_mmap(IOFileRef f, size_t length, off_t offset)
{
	struct io_data_posix *data = (struct io_data_posix*)f->_private;
	
	return mmap(NULL, length, PROT_READ, MAP_SHARED, data->fd, offset);
}


static int posix_munmap(IOFileRef f, void *addr, size_t length)
{
	(void)f;
	return munmap(addr, length);
}
