#pragma once

#include "CoreTypes.h"

class FileIO
{
	const Uint32 HeaderMAGIC	= 'MML4';
	const Uint32 HeaderVERSION	= 0x00000001;

	struct FileIO_Header
	{
		Uint32	Magic;
		Uint32	Version;
		Uint32	Compressed_Size;
		Uint32	Uncompressed_Size;
		bool	IsCompressed;
	};

public:
	 bool LoadBinaryBlobFromFile(const char *filename, void **Data, Uint32 &DataSize);
	 bool SaveBinaryBlobToFile(  const char *filename, void *Data, Uint32 DataSize);
};

extern FileIO gGlobalFileIO;