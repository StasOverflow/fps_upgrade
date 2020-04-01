#ifndef __FS_H__
#define __FS_H__


struct fs_file
{
	const char *name;
	void *data;
	int  size;
	char flgs;
};


struct fs_file *fs_open(char *uri);
void            fs_close(struct fs_file *file);

#endif /* __FS_H__ */
