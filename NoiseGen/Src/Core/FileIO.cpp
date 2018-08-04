#include <stdio.h>

#include "FileIO.h"

#include "LZ4\LZ4.h"


FileIO gGlobalFileIO;


bool FileIO::LoadBinaryBlobFromFile(const char *filename, void **DataOut, Uint32 &DataSizeOut)
{
	// Open the file and get the header. This will determine if the file is a compressed blob or not
	FILE *fp;
	fopen_s(&fp,filename, "rb");

	// Load Binary Blob - This could be compressed so read header first
	FileIO_Header header;
	fread(&header , sizeof(FileIO_Header), 1, fp);

	assert(header.Magic		== HeaderMAGIC);
	assert(header.Version	== HeaderVERSION);

	Uint8 *pDataSrc		= new Uint8[header.Compressed_Size];
	fread( pDataSrc , header.Compressed_Size, 1, fp);
	fclose(fp);

	if(header.IsCompressed)
	{
		// Decompress to the destination 
		Uint8 *pDataDest	= new Uint8[header.Uncompressed_Size];
		int LZ4Ret =  LZ4_decompress_safe((const char *) pDataSrc, (char *) pDataDest, header.Compressed_Size, header.Uncompressed_Size);
		delete [] pDataSrc;
		*DataOut	= pDataDest;
		DataSizeOut = header.Uncompressed_Size;
	}
	else
	{
		*DataOut	= pDataSrc;
		DataSizeOut = header.Uncompressed_Size;
	}
	return true;
}

bool FileIO::SaveBinaryBlobToFile(  const char *filename, void *Data, Uint32 DataSize)
{
	FILE *fp;
	fopen_s(&fp,filename, "wb");

	FileIO_Header header;
	header.Magic			= HeaderMAGIC;
	header.Version			= HeaderVERSION;
	header.Uncompressed_Size= DataSize;

#define USE_COMPRESSION

#ifdef USE_COMPRESSION
	// Compress the data so we know the compressed size
	Uint32 OutDataSize = DataSize;
	Uint8 *pDataDest	= new Uint8[OutDataSize];

	int CompressSize = LZ4_compress_default((const char *) Data, (char *) pDataDest, DataSize, OutDataSize);
	
	while(CompressSize == 0)
	{
		delete [] pDataDest;
		OutDataSize		*=2;
		pDataDest		= new Uint8[OutDataSize];
		CompressSize	= LZ4_compress_default((const char *) Data, (char *) pDataDest, DataSize, OutDataSize);
	}

	header.Compressed_Size	= CompressSize;
	header.IsCompressed		= true;

	fwrite(&header , sizeof(FileIO_Header), 1, fp);
    fwrite( pDataDest , CompressSize, 1, fp);
	delete [] pDataDest;
#else
	header.Compressed_Size	= DataSize;
	header.IsCompressed		= false;

	fwrite(&header , sizeof(FileIO_Header), 1, fp);
    fwrite( Data , DataSize, 1, fp);

#endif

	fclose(fp);
	return true;
}

