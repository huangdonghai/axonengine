/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef AX_GZIP_H
#define AX_GZIP_H

#include <zlib.h>
//#include <zutil.h>

AX_BEGIN_NAMESPACE

class Gzip {
public:
	byte_t *pgzip;
	int Length;

public:
	Gzip(char *lpsz,int len=-1);
	~Gzip();

	void Init(char *lpsz,int len=-1);

private:
	int write(byte_t *buf,int count);
	int finish();
	int destroy();
	void putLong (uint_t x);

private:
	byte_t m_buffer[1024];
	int m_CurrentBufferSize;
	z_stream m_zstream;
	int m_z_err;	/* error code for last stream operation */
	byte_t *m_outbuf;	/* output buffer */
	uint_t m_crc;		/* crc32 of uncompressed data */
};


class Ungzip {
public:
	char *psz;
	int  Length;

	Ungzip(byte_t *pgzip,int len);
	~Ungzip();
	void Init();

private:
	void check_header();
	int get_byte();
	int read(byte_t *buf,int size);

	int gzread(char *buf,int len);
	uint_t getLong();
	int write(char *buf,int count);
	int destroy();

private:
	char m_buffer[1024+1];
	int m_CurrentBufferSize;
	z_stream m_zstream;
	int m_z_err;	/* error code for last stream operation */
	byte_t *m_inbuf;	/* output buffer */
	uint_t m_crc;		/* crc32 of uncompressed data */
	int m_z_eof;
	int m_transparent;

	int m_pos;
	byte_t *m_gzip;
	int m_gziplen;

};

AX_END_NAMESPACE

#endif // AX_GZIP_H
