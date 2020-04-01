/*
 * string_utils.c
 *
 *  Created on: 07.07.2014
 *      Author: V.Dudnik
 */

#include <string.h>

void string_space_align_center( char *str, long new_strlen )
{
	long   spaces_cnt;
	long   src_strlen;

	src_strlen = strlen(str);
	spaces_cnt = new_strlen - src_strlen;

	if( spaces_cnt > 0 )
	{
		memset(str + src_strlen, ' ', spaces_cnt);

		str[new_strlen] = '\0';

		spaces_cnt /= 2;

		if( spaces_cnt > 0 )
		{
			while( --src_strlen >= 0 )
			{
				str[src_strlen + spaces_cnt] = str[src_strlen];
				str[src_strlen] = ' ';
			}
		}
	}
}


void string_space_align_left( char *str, long new_strlen )
{
	long   spaces_cnt;
	long   src_strlen;

	src_strlen = strlen(str);
	spaces_cnt = new_strlen - src_strlen;

	if( spaces_cnt > 0 )
	{
		memset(str + src_strlen, ' ', spaces_cnt);

		str[new_strlen] = '\0';
	}
}

void string_insert( char *src_str, char *sub_str, long pos )
{
	long src_str_len = strlen(src_str);
	long sub_str_len = strlen(sub_str);

	if( pos <= src_str_len )
	{
		while( src_str_len >= 0 )
		{
			src_str[src_str_len + sub_str_len] = src_str[src_str_len];

			if( src_str_len == pos )
			{
				for( long i = 0; i < sub_str_len; i++ )
				{
					src_str[pos + i] = sub_str[i];
				}

				break;
			}

			src_str_len--;
		}
	}
}
