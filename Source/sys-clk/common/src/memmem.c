#include "memmem.h"

void *memmem_impl(void *haystack, size_t haystacklen, const void *needle, size_t needlelen)
{
	const unsigned char *cmpp;
	const unsigned char *p;
	const unsigned char *endp;
	const unsigned char *q;
	const unsigned char *endq;
	unsigned char found;

	if(haystack == NULL)
	{
		return NULL;
	}
	if(needle == NULL)
	{
		return haystack;
	}
	if(haystacklen == 0)
	{
		return NULL;
	}
	if(needlelen == 0)
	{
		return haystack;
	}

	if(needlelen > haystacklen)
	{
		return NULL;
	}

	endp = haystack + haystacklen - needlelen;
	endq = needle + needlelen;
	for(p = haystack; p <= endp; p++)
	{
		found = 1;
		cmpp = p;
		for(q = needle; q < endq; q++)
		{
			if(*cmpp != *q)
			{
				found = 0;
				break;
			}
			else
			{
				cmpp++;
			}
		}
		if(found)
		{
			return (void*)p;
		}
	}

  return NULL;
}
