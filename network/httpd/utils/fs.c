
#include <string.h>
#include <lwiplib.h>
#include "fs.h"
#include "fsdata.h"

//*****************************************************************************
//
// Root FS structure
//
//*****************************************************************************
extern const struct fs_data *fs_root;

//*****************************************************************************
//
// Open a file and return a handle to the file, if found.  Otherwise,
// return NULL.
//
//*****************************************************************************
struct fs_file *
fs_open(char *uri)
{
	const struct fs_data *tree = NULL;
	      struct fs_file *file = NULL;
          char   *params;

	//
	// Allocate memory for the file system structure.
	//
	file = mem_malloc(sizeof(struct fs_file));

	if(file == NULL)
	{
		return NULL;
	}

	//
	// Search params in uri
	//
	params = strchr(uri, '?');

	if(params)
	{
		*params = '\0';
		params++;
	}

	//
	// Initialize the file system tree pointer to the root of the linked list.
	//
	tree = fs_root;

	//
	// Begin processing the linked list, looking for the requested file name.
	//
	while(tree != NULL)
	{
		//
		// Compare the requested file "name" to the file name in the
		// current node.
		//
		if(strcmp(uri, tree->name) == 0)
		{
			file->name = tree->name;

			file->data = (char *)tree->data;

			if(tree->func)
			{
				file->size = tree->func((char *)tree->data, tree->size, params);
			}
			else
			{
				file->size = tree->size;
			}

			file->flgs = tree->flgs;

			//
			// Return the file system pointer.
			//
			return file;
		}

		//
		// If we get here, we did not find the file at this node of the linked
		// list.  Get the next element in the list.
		//
		tree = tree->next;
	}

	//
	// File was not found
	//
	mem_free(file);

	return NULL;
}

//*****************************************************************************
//
// Close an opened file designated by the handle.
//
//*****************************************************************************
void fs_close(struct fs_file *file)
{
	mem_free(file);
}
