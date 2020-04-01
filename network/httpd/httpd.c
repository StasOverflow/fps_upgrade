
#include <string.h>
#include <lwiplib.h>
#include <lwip/init.h>

#include "utils/fs.h"
#include "fsdata.h"
#include "httpd.h"

//*****************************************************************************
//
// Generate the HTTP status header
//
//*****************************************************************************
static const char *
httpd_status_header_get(int status)
{
	struct status_hdr
	{
	    int status;
	    const char *hdr;
	};

	static const struct status_hdr status_hdrs[] =
	{
		{ 200, "HTTP/1.1 200 OK\r\n"          },
		{ 400, "HTTP/1.1 400 Bad Request\r\n" },
		{ 403, "HTTP/1.1 403 Forbidden\r\n"   },
		{ 404, "HTTP/1.1 404 Not Found\r\n"   },
	};

	const int hdr_num = (sizeof(status_hdrs) /
			             sizeof(status_hdrs[0]));

    //
    // Make HTTP status header
    //
    for(int i = 0; i < hdr_num; i++)
    {
    	if(status_hdrs[i].status == status)
    	{
    		return status_hdrs[i].hdr;
    	}
    }

    return NULL;
}

//*****************************************************************************
//
// Generate the HTTP Content-type header
//
//*****************************************************************************
static const char *
httpd_content_type_header_get(const char *name)
{
	char  *tmp;
	char  *ext;

	struct content_hdr
	{
	    const char *ext;
	    const char *hdr;
	};

	static const struct content_hdr content_hdrs[] =
	{
		{ "*",    "Content-type: text/plain\r\n"               },
		{ "html", "Content-type: text/html\r\n"                },
		{ "xml",  "Content-type: text/xml\r\n"                 },
		{ "css",  "Content-type: text/css\r\n"                 },
		{ "gif",  "Content-type: image/gif\r\n"                },
		{ "png",  "Content-type: image/png\r\n"                },
		{ "bmp",  "Content-type: image/bmp\r\n"                },
		{ "jpg",  "Content-type: image/jpeg\r\n"               },
		{ "ico",  "Content-type: image/x-icon\r\n"             },
		{ "json", "Content-type: application/json\r\n"         },
		{ "js",   "Content-type: application/x-javascript\r\n" },
		{ "bin",  "Content-type: application/octet-stream\r\n" },
	};

	const int hdr_num = (sizeof(content_hdrs) /
			             sizeof(content_hdrs[0]));

	//
	// Get a pointer to the file extension.  We find this by looking for the
	// last occurrence of "." in the filename passed.
	//
	ext = NULL;
	tmp = strchr(name, '.');

	while(tmp)
	{
		ext = tmp + 1;
		tmp = strchr(ext, '.');
	}

	//
	// Now determine the content type and add the relevant header for that.
	//
	for(int i = 0; (i < hdr_num) && ext; i++)
	{
		//
		// Have we found a matching extension?
		//
		if( !strcmp(content_hdrs[i].ext, ext) )
		{
			return content_hdrs[i].hdr;
		}
	}

	//
	// Not found. Return a default header
	//
	return content_hdrs[0].hdr;
}

//*****************************************************************************
//
// Serve one HTTP connection accepted in the http thread
//
//*****************************************************************************
static void
httpd_serve(struct netconn *conn)
{
	struct fs_file *file = NULL;
	struct netbuf  *inbuf;
	const char     *hdrs[10];
	u16_t          hdr_count = 0;
	char           *buf;
	int            http_status;
	u16_t          buflen;
	int            i;

	//
	// Read the data from the port, blocking if nothing yet there.
	// We assume the request (the part we care about) is in one netbuf
	//
	if(netconn_recv(conn, &inbuf) == ERR_OK)
	{
		netbuf_data(inbuf, (void**)&buf, &buflen);

		//
		// Validate request
		//
	    if(buflen >= 10  &&
	       buf[0] == 'G' &&
	       buf[1] == 'E' &&
	       buf[2] == 'T' &&
	       buf[3] == ' ')
	    {
			//
			// We have a GET request. Find the end of the URI by looking for the
			// HTTP marker. We can't just use strstr to find this since the request
			// came from an outside source and we can't be sure that it is
			// correctly formed. We need to make sure that our search is bounded
			// by the packet length so we do it manually.
			//
			for(i = 5; i <= (buflen - 5); i++)
			{
				if( (buf[i + 0] == ' ') &&
					(buf[i + 1] == 'H') &&
					(buf[i + 2] == 'T') &&
					(buf[i + 3] == 'T') &&
					(buf[i + 4] == 'P') )
				{
					buf[i] = '\0';
					break;
				}
			}

			if(i <= (buflen - 5))
			{
				//
				// Is root file asked?
				//
				if((buf[4] == '/') &&
				   (buf[5] == '\0'))
				{
					char uri[] = "/index.html";
					file = fs_open(uri);
				}
				else
				{
					file = fs_open(&buf[4]);
				}

				if(file != NULL)
				{
					http_status = 200;
				}
				else
				{
					http_status = 404;
				}
			}
			else
			{
				http_status = 400;
			}
	    }
	    else
	    {
	    	http_status = 400;
	    }

	    //
	    // Try to open error page
	    //
	    if(http_status >= 400)
	    {
	    	char uri[20] = "/error.cgi?code=";

	    	switch(http_status)
	    	{
	    	case 400:
	    		strcat(uri, "400");
	    		break;

	    	case 404:
	    		strcat(uri, "404");
	    		break;

	    	default:
	    		strcat(uri, "500");
	    		break;
	    	}

	    	file = fs_open(uri);
	    }

		//
		// Get HTTP status header
		//
		hdrs[hdr_count++] = httpd_status_header_get(http_status);

		//
		// Set Server header
		//
		hdrs[hdr_count++] = "Server: lwIP/" LWIP_VERSION_STRING "\r\n";

		if(file != NULL)
		{
			//
			// Check if http headers are already included to file
			//
			if(!(file->flgs & FS_FILE_HAS_HTTP_HDR))
			{
				//
				// Get Content-Type header
				//
				hdrs[hdr_count++] = httpd_content_type_header_get(file->name);

				//
				// Set Content-Encoding header if file is compressed
				//
				if(file->flgs & FS_FILE_COMPRESSED)
				{
					hdrs[hdr_count++] = "Content-Encoding: deflate\r\n";
//					hdrs[hdr_count++] = "Content-Encoding: gzip\r\n";
				}

				//
				// Set Connection header
				//
				hdrs[hdr_count++] = "Connection: close\r\n\r\n";
			}
		}
		else
		{
			//
			// Set Connection header
			//
			hdrs[hdr_count++] = "Connection: close\r\n\r\n";
		}

		//
		// Send headers
		//
		for(i = 0; i < hdr_count; i++)
		{
			netconn_write(conn, hdrs[i], strlen(hdrs[i]), NETCONN_COPY);
		}

		//
		// Send file if it's available
		//
		if(file != NULL)
		{
			netconn_write(conn, file->data, file->size, NETCONN_COPY);

			fs_close(file);
		}
	}

	//
	// Close the connection
	//
	netconn_close(conn);

	//
	// Delete the buffer (netconn_recv gives us ownership,
	// so we have to make sure to deallocate the buffer)
	//
	netbuf_delete(inbuf);
}

//*****************************************************************************
//
// The main function, never returns!
//
//*****************************************************************************
void
httpd_thread(void *arg)
{
	struct netconn *conn, *newconn;

	//
	// Create a new TCP connection handle
	//
	conn = netconn_new(NETCONN_TCP);

	//
	// Bind to port 80 (HTTP) with default IP address
	//
	netconn_bind(conn, NULL, 80);

	//
	// Put the connection into LISTEN state
	//
	netconn_listen(conn);

	for(;;)
	{
		if(netconn_accept(conn, &newconn) == ERR_OK)
		{
			httpd_serve(newconn);
			netconn_delete(newconn);
		}
	}
}
