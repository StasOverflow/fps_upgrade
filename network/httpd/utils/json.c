
#include "json.h"


char *
json_node_add(char *p, const char *name, const char *value)
{
	*p++ = '\n';
	*p++ = '\t';
	*p++ = '"';

	while(*name)
	{
		*p++ = *name++;
	}

	*p++ = '"';
	*p++ = ':';
	*p++ = ' ';
	*p++ = '"';

	while(*value)
	{
		*p++ = *value++;
	}

	*p++ = '"';
	*p++ = ',';

	return p;
}


char *
json_end(char *p)
{
	if(*(p - 1) == ',')
	{
		p--;
	}

	*p++ = '\n';
	*p++ = '}';
	*p   = '\0';

	return p;
}


char *
json_begin(char *p)
{
	*p++ = '{';

	return p;
}
