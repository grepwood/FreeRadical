#include <stdint.h>

uint32_t DAT2DataSize(FILE * DAT2)
{
	uint32_t result;
	fseeko(DAT2,-4,SEEK_END);
	if(!BigEndian())
	{
		fread(&result,4,1,DAT2);
	}
	else
	{
		result = ReadAlienEndian(DAT2);
	}
	return result;
}

uint32_t DAT2TreeSize(FILE * DAT2)
{
	uint32_t result;
	fseeko(DAT2,-8,SEEK_END);
	if(!BigEndian())
	{
		fread(&result,4,1,DAT2);
	}
	else
	{
		result = ReadAlienEndian(DAT2);
	}
	return result;
}

/* Call DAT2FilesTotal with Offset equal to DataSize - TreeSize - 8
 * like
 * DAT2FilesTotal(DAT2File,DataSize-TreeSize-8); */

uint32_t DAT2FilesTotal(FILE * DAT2, uint32_t Offset)
{
	uint32_t result;
	fseeko(DAT2,Offset,SEEK_SET);
	if(!BigEndian())
	{
		fread(&result,4,1,DAT2);
	}
	else
	{
		result = ReadAlienEndian(DAT2);
	}
	return result;
}

char ItIsDAT2(FILE * DATX)
{
	char result = 0;
	uint32_t ActualSize = 0;
	uint32_t Current = ftello(DATX);
	uint32_t DataSize = DAT2DataSize(DATX);
	fseeko(DATX,0L,SEEK_END);
	ActualSize = ftello(DATX);
	fseeko(DATX,Current,SEEK_SET); /* We gracefully restore position */
	if(ActualSize == DataSize)
	{
		result = 1; /* Or an unlikely coincidence */
	}
	return result;
}
