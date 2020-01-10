/*
 * libopenraw - memstream.h
 *
 * Copyright (C) 2007 Hubert Figuière
 *
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef __IO_MEMSTREAM_H__
#define __IO_MEMSTREAM_H__


#include "io/stream.h"

namespace OpenRaw {
	namespace IO {
		
		class MemStream
			: public Stream
		{
		public:
			MemStream(void *ptr, size_t s);

			virtual ~MemStream()
				{}

			virtual or_error open();
			virtual int close();
			virtual int seek(off_t offset, int whence);
			virtual int read(void *buf, size_t count);
			virtual off_t filesize();


		private:
			void * m_ptr;
			size_t m_size;
			unsigned char * m_current;

			/** private copy constructor to make sure it is not called */
			MemStream(const MemStream& f);
			/** private = operator to make sure it is never called */
			MemStream & operator=(const MemStream&);
};

	}
}

#endif

