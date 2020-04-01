#ifndef __FSDATA_H__
#define __FSDATA_H__


#define FS_FILE_COMPRESSED          ( 0x01 )
#define FS_FILE_HAS_HTTP_HDR        ( 0x02 )


struct fs_data
{
	const struct fs_data *next;
	const char *name;
	int (*func)(char *out, int size, char *uri_params);
	const void *data;
	int  size;
	char flgs;
};


#endif /* __FSDATA_H__ */
