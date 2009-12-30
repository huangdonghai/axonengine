/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"
#include <zlib.h>
#include <zutil.h>
#include "gzip.h"


#define Z_BUFSIZE 4096
#define ASCII_FLAG 0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC 0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD 0x04 /* bit 2 set: extra field present */
#define ORIG_NAME 0x08 /* bit 3 set: original file name present */
#define COMMENT 0x10 /* bit 4 set: file comment present */
#define RESERVED 0xE0 /* bits 5..7: reserved */

AX_BEGIN_NAMESPACE

static const int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
static const int t_nBufferLength = 1024;
static const int t_nLevel=Z_DEFAULT_COMPRESSION;
static const int t_nStrategy=Z_DEFAULT_STRATEGY;

Gzip::Gzip(char* lpsz, int len)
	: pgzip(0)
	, Length(0)
{
	Init(lpsz,len);
}

Gzip::~Gzip() {
	if (pgzip != m_buffer)
		SafeFree(pgzip);
}

void
Gzip::Init(char *lpsz, int len) {
	if (lpsz == 0) {
		pgzip=0; 
		Length=0;
		return ;
	}

	if (len == -1) {
		len=(int)strlen(lpsz);
	}
	m_CurrentBufferSize=t_nBufferLength;
	pgzip=m_buffer;

	m_zstream.zalloc = (alloc_func)0;
	m_zstream.zfree = (free_func)0;
	m_zstream.opaque = (voidpf)0;
	m_zstream.next_in = Z_NULL;
	m_zstream.next_out = Z_NULL;
	m_zstream.avail_in = 0;
	m_zstream.avail_out = 0;
	m_z_err = Z_OK;
	m_crc = crc32(0L, Z_NULL, 0);

	int err = deflateInit2(&(m_zstream), t_nLevel, Z_DEFLATED, -MAX_WBITS, DEF_MEM_LEVEL, t_nStrategy);
	m_outbuf = (byte_t*)::malloc(Z_BUFSIZE);
	m_zstream.next_out = m_outbuf;

	if (err != Z_OK || m_outbuf == Z_NULL) {
		destroy();
		return ;
	}

	m_zstream.avail_out = Z_BUFSIZE;
	byte_t header[10]={ 0x1f, 0x8b, Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE };
	write(header, 10);

	m_zstream.next_in = (Bytef*)lpsz;
	m_zstream.avail_in = len;
	while (m_zstream.avail_in != 0) {
		if (m_zstream.avail_out == 0) {
			m_zstream.next_out = m_outbuf;
			this->write(m_outbuf, Z_BUFSIZE);
			m_zstream.avail_out = Z_BUFSIZE;
		}

		m_z_err = deflate(&m_zstream, Z_NO_FLUSH);

		if (m_z_err != Z_OK)
			break;
	}
	m_crc = crc32(m_crc, (const Bytef *)lpsz, len);
	if (finish() != Z_OK) {
		destroy();
		return;
	}
	putLong(m_crc);
	putLong (m_zstream.total_in);  
	destroy();
}

int
Gzip::write(byte_t* buf, int count) {
	if (buf == 0)
		return 0;
	if (Length+count > m_CurrentBufferSize) {
		int nTimes=(Length+count)/t_nBufferLength +1;
		byte_t* pTemp=pgzip;
		pgzip=static_cast<byte_t*>(malloc(nTimes*t_nBufferLength));
		m_CurrentBufferSize=nTimes*t_nBufferLength;
		memcpy(pgzip,pTemp,Length);
		if (pTemp!=m_buffer) free(pTemp);
	}
	memcpy(pgzip+Length,buf,count);
	Length+=count;
	return count;
}

int
Gzip::finish() {
	uInt len;
	int done = 0;
	m_zstream.avail_in = 0;

	for (; ;) {
		len = Z_BUFSIZE - m_zstream.avail_out;
		if (len != 0) {
			write(m_outbuf,len);
			m_zstream.next_out = m_outbuf;
			m_zstream.avail_out = Z_BUFSIZE;
		}
		if (done) break;
		m_z_err = deflate(&(m_zstream), Z_FINISH);
		if (len == 0 && m_z_err == Z_BUF_ERROR) m_z_err = Z_OK;

		done = (m_zstream.avail_out != 0 || m_z_err == Z_STREAM_END);
		if (m_z_err != Z_OK && m_z_err != Z_STREAM_END) break;
	}
	return  m_z_err == Z_STREAM_END ? Z_OK : m_z_err;
}

int
Gzip::destroy() {
	int err = Z_OK;
	if (m_zstream.state != NULL) {
		err = deflateEnd(&(m_zstream));
	}
	if (m_z_err < 0) err = m_z_err;
	SafeFree(m_outbuf);
	return err;
}

void
Gzip::putLong(uint_t x) {
	for (int n = 0; n < 4; n++) {
		unsigned char c=(unsigned char)(x & 0xff);
		write(&c,1);
		x >>= 8;
	}
}



Ungzip::Ungzip(byte_t* pgzip, int len)
	: psz(NULL)
	, Length(0)
	, m_pos(0)
	, m_gzip(pgzip)
	, m_gziplen(len)
{
	Init(); 
}

Ungzip::~Ungzip()
{
	if (psz!=m_buffer) SafeFree(psz);  
}

void
Ungzip::Init() {
	if (m_gzip == 0) {
		psz=0; 
		Length=0;
		return ;
	}

	m_CurrentBufferSize=t_nBufferLength;
	psz=m_buffer;
	memset(psz,0,m_CurrentBufferSize+1);

	m_zstream.zalloc = (alloc_func)0;
	m_zstream.zfree = (free_func)0;
	m_zstream.opaque = (voidpf)0;
	m_zstream.next_in = m_inbuf = Z_NULL;
	m_zstream.next_out = Z_NULL;
	m_zstream.avail_in = m_zstream.avail_out = 0;
	m_z_err = Z_OK;
	m_z_eof = 0;
	m_transparent = 0;
	m_crc = crc32(0L, Z_NULL, 0);

	m_zstream.next_in  =m_inbuf = (byte_t*)::malloc(Z_BUFSIZE);
	int  err = inflateInit2(&(m_zstream), -MAX_WBITS);
	if (err != Z_OK || m_inbuf == Z_NULL)
	{
		destroy();
		return;
	}
	m_zstream.avail_out = Z_BUFSIZE;
	check_header();
	char outbuf[Z_BUFSIZE];
	int nRead;
	while (true) {
		nRead=gzread(outbuf,Z_BUFSIZE);
		if (nRead<=0) break;
		write(outbuf,nRead);
	}
	destroy();
}

void
Ungzip::check_header() {
	int method; /* method byte */
	int flags;  /* flags byte */
	uInt len;
	int c;

	/* Check the gzip magic header */
	for (len = 0; len < 2; len++) {
		c = get_byte();
		if (c != gz_magic[len]) {
			if (len != 0)
				m_zstream.avail_in++, m_zstream.next_in--;

			if (c != EOF) {
				m_zstream.avail_in++, m_zstream.next_in--;
				m_transparent = 1;
			}
			m_z_err =m_zstream.avail_in != 0 ? Z_OK : Z_STREAM_END;
			return;
		}
	}
	method = get_byte();
	flags = get_byte();
	if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
		m_z_err = Z_DATA_ERROR;
		return;
	}
	/* Discard time, xflags and OS code: */
	for (len = 0; len < 6; len++)
		(void)get_byte();

	if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
		len  =  (uInt)get_byte();
		len += ((uInt)get_byte())<<8;
		/* len is garbage if EOF but the loop below will quit anyway */
		while (len-- != 0 && get_byte() != EOF) ;
	}
	if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
		while ((c = get_byte()) != 0 && c != EOF) ;
	}
	if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
		while ((c = get_byte()) != 0 && c != EOF) ;
	}
	if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
		for (len = 0; len < 2; len++) (void)get_byte();
	}
	m_z_err = m_z_eof ? Z_DATA_ERROR : Z_OK;
}

int
Ungzip::get_byte() {
	if (m_z_eof)
		return EOF;
	if (m_zstream.avail_in == 0) {
		errno = 0;
		m_zstream.avail_in =read(m_inbuf,Z_BUFSIZE);
		if (m_zstream.avail_in == 0) {
			m_z_eof = 1;
			return EOF;
		}
		m_zstream.next_in = m_inbuf;
	}
	m_zstream.avail_in--;
	return *(m_zstream.next_in)++;
}

int
Ungzip::read(byte_t* buf, int size) {
	int nRead=size;
	if (m_pos+size >= m_gziplen) {
		nRead=m_gziplen-m_pos;
	}
	if (nRead <= 0)
		return 0;
	memcpy(buf, m_gzip+m_pos, nRead);
	m_pos+=nRead;
	return nRead;
}

int
Ungzip::gzread(char* buf, int len) {
	Bytef *start = (Bytef*)buf; /* starting point for crc computation */
	byte_t  *next_out; /* == stream.next_out but not forced far (for MSDOS) */

	if (m_z_err == Z_DATA_ERROR || m_z_err == Z_ERRNO)
		return -1;

	if (m_z_err == Z_STREAM_END)
		return 0;  /* EOF */

	next_out = (byte_t*)buf;
	m_zstream.next_out = (Bytef*)buf;
	m_zstream.avail_out = len;
	while (m_zstream.avail_out != 0) {
		if (m_transparent) {
			/* Copy first the lookahead bytes: */
			uInt n = m_zstream.avail_in;
			if (n > m_zstream.avail_out) n = m_zstream.avail_out;
			if (n > 0)  {
				zmemcpy(m_zstream.next_out,m_zstream.next_in, n);
				next_out += n;
				m_zstream.next_out = next_out;
				m_zstream.next_in   += n;
				m_zstream.avail_out -= n;
				m_zstream.avail_in  -= n;
			}
			if (m_zstream.avail_out > 0) {
				m_zstream.avail_out -=read(next_out,m_zstream.avail_out);
			}
			len -= m_zstream.avail_out;
			m_zstream.total_in  += (uint_t)len;
			m_zstream.total_out += (uint_t)len;
			if (len == 0) m_z_eof = 1;
			return (int)len;
		}
		if (m_zstream.avail_in == 0 && !m_z_eof) {
			errno = 0;
			m_zstream.avail_in = read(m_inbuf,Z_BUFSIZE);
			if (m_zstream.avail_in == 0)
			{
				m_z_eof = 1;
			}
			m_zstream.next_in = m_inbuf;
		}
		m_z_err = inflate(&(m_zstream), Z_NO_FLUSH);
		if (m_z_err == Z_STREAM_END) {
			/* Check CRC and original size */
			m_crc = crc32(m_crc, start, (uInt)(m_zstream.next_out - start));
			start = m_zstream.next_out;
			if (getLong() != m_crc) {
				m_z_err = Z_DATA_ERROR;
			} else {
				(void)getLong();
				check_header();
				if (m_z_err == Z_OK) {
					uint_t total_in = m_zstream.total_in;
					uint_t total_out = m_zstream.total_out;
					inflateReset(&(m_zstream));
					m_zstream.total_in = total_in;
					m_zstream.total_out = total_out;
					m_crc = crc32(0L, Z_NULL, 0);
				}
			}
		}
		if (m_z_err != Z_OK || m_z_eof)
			break;
	}

	m_crc = crc32(m_crc, start, (uInt)(m_zstream.next_out - start));
	return (int)(len - m_zstream.avail_out);
}

uint_t
Ungzip::getLong() {
	uint_t x = (uint_t)get_byte();
	int c;

	x += ((uint_t)get_byte())<<8;
	x += ((uint_t)get_byte())<<16;
	c = get_byte();
	if (c == EOF) m_z_err = Z_DATA_ERROR;
	x += ((uint_t)c)<<24;
	return x;
}

int
Ungzip::write(char* buf, int count) {
	if (buf == 0)
		return 0;

	if (Length+count > m_CurrentBufferSize) {
		int nTimes=(Length+count)/t_nBufferLength +1;
		char *pTemp=psz;
		psz=static_cast<char*>(malloc(nTimes*t_nBufferLength+1));
		m_CurrentBufferSize=nTimes*t_nBufferLength;
		memset(psz,0,m_CurrentBufferSize+1);
		memcpy(psz,pTemp,Length);
		if (pTemp!=m_buffer) free(pTemp);
	}
	memcpy(psz+Length,buf,count);
	Length+=count;
	return count;
}

int
Ungzip::destroy() {
	int err = Z_OK;
	if (m_zstream.state != NULL) {
		err = inflateEnd(&(m_zstream));
	}
	if (m_z_err < 0) err = m_z_err;
	SafeFree(m_inbuf);
	return err;
}

AX_END_NAMESPACE
