#include "ArscFile.h"

#define IS_NULL(a) Q_NULLPTR == a
#define NOT_NULL(a) Q_NULLPTR != a

#define RELEASE_PTR(a) if (Q_NULLPTR!=a) {delete a;}



ArscFile::ArscFile(const QString &path)
	: FileReader(path)
	, mArsc(path)
{

}

ArscFile::~ArscFile()
{
}

bool ArscFile::parseFile()
{
	if (!initReader(true)) {
		return false;
	}

	mHeader = readResChunkHeader();
	if (IS_NULL(mHeader)) {
		return false;
	}
	readUInt(mPackageCount);

	mResStringPool = readResStringPool();

	mPackages = readPackage();

	mTypeStringPool = readResStringPool();

	mKeyStringPool = readResStringPool();

	readResTableTypeSepc();
	return true;
}

ResChunk_header* ArscFile::readResChunkHeader() {
	ResChunk_header* pHeader = new ResChunk_header();
	const int len = sizeof(ResChunk_header);
	if (len == readBytes((uchar*)pHeader, len)) {
		if (pHeader->type == RES_TABLE_TYPE) {
			return pHeader;
		}
	}
	delete pHeader;
	return Q_NULLPTR;
}

ArscFile::MyResStringPool *ArscFile::readResStringPool()
{
	bool result = false;
	ResStringPool_header* pHeader = Q_NULLPTR;
	quint32* pEntries = Q_NULLPTR;
	quint32* pEntryStyles = Q_NULLPTR;
	StringLen** pStringLens = Q_NULLPTR;
	char** pStrings = Q_NULLPTR;
	int stringPadding = 0;
	ResStringPool_span** pStyles = Q_NULLPTR;
	quint64 iStringsSize = 0;
	qint64 pos = filePos();

	do {
		pHeader = new ResStringPool_header();
		const int len = sizeof(ResStringPool_header);
		if (len != readBytes((uchar*)pHeader, len) ||
			pHeader->header.type != RES_STRING_POOL_TYPE) {
			break;
		}
		bool isUtf8 = 0 != (pHeader->flags & ResStringPool_header::UTF8_FLAG);

		quint32* pEntries = new quint32[pHeader->stringCount];
		const int entry_len = sizeof(quint32) * pHeader->stringCount;
		if (entry_len != readBytes((char*)&pEntries[0], entry_len)) {
			break;
		}

		quint32* pEntryStyles = new quint32[pHeader->styleCount];
		const int entry_styles_len = sizeof(quint32) * pHeader->styleCount;
		if (entry_styles_len != readBytes((char*)&pEntryStyles[0], entry_styles_len)) {
			break;
		}

		qint64 cur_pos = filePos();
		pStringLens = new StringLen*[pHeader->stringCount];
		memset(&pStringLens[0], 0, sizeof(StringLen*) * pHeader->stringCount);
		pStrings = new char*[pHeader->stringCount];
		memset(&pStrings[0], 0, sizeof(char*) * pHeader->stringCount);

		qint64 stringsStart = pHeader->stringsStart + pos;
		for (quint32 i = 0; i < pHeader->stringCount; i++) {
			quint64 seek_pos = stringsStart + pEntries[i];
			fileSeek(seek_pos);
			pStringLens[i] = readStringLen(isUtf8);
			char* str = new char[pStringLens[i]->stringLen()];
			readBytes(str, pStringLens[i]->stringLen());
			pStrings[i] = str;
			iStringsSize += pStringLens[i]->stringLen() + pStringLens[i]->dataLen + 1;
			fileSkip(1);
		}

		quint64 endPos = pos + pHeader->header.size;
		cur_pos = filePos();
		bool nostyle = pHeader->stylesStart == 0;
		if (nostyle) {
			stringPadding = endPos - cur_pos;
		}
		else {
			stringsStart = pos + pHeader->stylesStart;
			stringPadding = stringsStart - cur_pos;
		}

		if (stringPadding != 0)
		{
			fileSkip(stringPadding);
		}

		if (nostyle) {
			result = true;
			break;
		}

		pStyles = new ResStringPool_span*[pHeader->styleCount];
		memset(&pStyles[0], 0, sizeof(ResStringPool_span*)*pHeader->styleCount);
		for (int i = 0; i < pHeader->styleCount; i++) {
			quint64 seek_pos = stringsStart + pEntryStyles[i];
			fileSeek(seek_pos);
			pStyles[i] = readResStringPoolSpan();
		}

		quint32 stylesEnds = 0;
		readUInt(stylesEnds);
		if (stylesEnds != ResStringPool_span::END) {
			break;
		}

		stylesEnds = 0;
		readUInt(stylesEnds);
		if (stylesEnds != ResStringPool_span::END) {
			break;
		}

		result = true;
	} while (0);

	if (!result) {
		const quint32 STRING_COUNT_N = IS_NULL(pHeader) ? 0 : pHeader->stringCount;
		const quint32 STYLE_COUNT_N = IS_NULL(pHeader) ? 0 : pHeader->styleCount;

		RELEASE_PTR(pHeader);
		RELEASE_PTR(pEntries);
		RELEASE_PTR(pEntryStyles);

		if (NOT_NULL(pStringLens)) {
			for (quint32 i = 0; i < STRING_COUNT_N; i++) {
				RELEASE_PTR(pStringLens[i]);
			}
			RELEASE_PTR(pStringLens);
		}


		if (NOT_NULL(pStrings)) {
			for (quint32 i = 0; i < STRING_COUNT_N; i++) {
				RELEASE_PTR(pStrings[i]);
			}
			RELEASE_PTR(pStrings);
		}

		if (NOT_NULL(pStyles)) {
			for (quint32 i = 0; i < STYLE_COUNT_N; i++) {
				RELEASE_PTR(pStyles[i]);
			}
			RELEASE_PTR(pStyles);
		}

		return Q_NULLPTR;
	}

	if (result) {
		MyResStringPool* pool = new MyResStringPool();
		pool->mHeader = pHeader;
		pool->mStringEntries = pEntries;
		pool->mStringLens = pStringLens;
		pool->mStrings = pStrings;
		pool->mStringSize = iStringsSize;
		pool->mStyleEntries = pEntryStyles;
		pool->mStyles = pStyles;
		return pool;
	}
	return Q_NULLPTR;
}

ArscFile::StringLen* ArscFile::readStringLen(bool isUtf8) {
	quint32 result = 0;
	if (isUtf8) {
		//char length1 = 0;
		//char length2 = 0;
		//bool hasLength2 = false;
		//readByte(&length1);
		//result = length1;
		//if (0 != (length1 & 0x80)) {
		//    readByte(&length2);
		//    result = ((result & 0x7F) << 8) | length2;
		//    hasLength2 = true;
		//}
		//StringLen* pLen = new StringLen();
		//pLen->value = result;
		//pLen->isUtf8 = true;
		//const int N = hasLength2 ? 2 : 1;
		//pLen->data = new char[N];
		//pLen->data[0] = length1;
		//if (hasLength2) pLen->data[1] = length2;
		//pLen->len = sizeof(char) * N;
		//return pLen;
		unsigned char* data = new unsigned char[4];
		int u16DataLen = 0;
		int u16len = readUtf8StringLen(data, u16DataLen);

		int u8DataLen = 0;
		int u8len = readUtf8StringLen(data + u16DataLen, u8DataLen);

		StringLen* pLen = new StringLen();
		pLen->isUtf8 = true;
		pLen->data = data;
		pLen->u16Len = u16len;
		pLen->u8Len = u8len;
		pLen->dataLen = u16DataLen + u8DataLen;
		return pLen;
	}
	else {
		ushort* data = new ushort[2];
		int u16DataLen = 0;
		int u16len = readUtf16StringLen(data, u16DataLen);

		StringLen* pLen = new StringLen();
		pLen->isUtf8 = false;
		pLen->data16 = data;
		pLen->u16Len = u16len;
		pLen->u8Len = 0;
		pLen->dataLen = u16DataLen;
		return pLen;
	}
}

uint ArscFile::readUtf16StringLen(ushort* data, int& dataLen) {
	memset(&data[0], 0, sizeof(ushort) * 2);
	dataLen = 0;

	uint result = 0;
	readUShort(data[0]);
	result = data[0];
	dataLen = 2;
	if (0 != (data[0] & 0x8000)) {
		readUShort(data[1]);
		result = ((result & 0x7FFF) << 16) | data[1];
		dataLen = 4;
	}
	return result;
}

uint ArscFile::readUtf8StringLen(unsigned char* data, int& dataLen) {
	memset(&data[0], 0, 2);
	dataLen = 0;

	uint result = 0;

	readByte(data[0]);
	result = data[0];
	dataLen = 1;
	if (0 != (data[0] & 0x80)) {
		readByte(data[1]);
		result = ((result & 0x7F) << 8) | data[1];
		dataLen = 2;
	}
	return result;
}

ResStringPool_span* ArscFile::readResStringPoolSpan() {
	ResStringPool_span* span = new ResStringPool_span;
	readUInt(span->name.index);
	readUInt(span->firstChar);
	readUInt(span->lastChar);
	fileSkip(4);//END 0xFFFFFFFF
	return span;
}

ResTable_package* ArscFile::readPackage()
{
	ResChunk_header header;
	readBytes(&header, sizeof(ResChunk_header));

	ResTable_package* pPackage = new ResTable_package();
	memset(pPackage, 0, sizeof(ResTable_package));
	memcpy(pPackage, &header, sizeof(ResChunk_header));
	readBytes(&(pPackage->id),
		header.headerSize - sizeof(ResChunk_header));
	if (pPackage->header.type != RES_TABLE_PACKAGE_TYPE) {
		delete pPackage;
		return Q_NULLPTR;
	}
	return pPackage;
}

void ArscFile::readResTableTypeSepc() {
	quint64 fileOffset = filePos();

	const qint64 fileLen = fileSize();

	MyResTableTypeSepc* pCurrentTypeSpec = Q_NULLPTR;

	while (fileOffset < fileLen) {
		ResChunk_header *header = new ResChunk_header();
		readBytes(header, sizeof(ResChunk_header));
		switch (header->type)
		{
		case RES_TABLE_TYPE_SPEC_TYPE:
		{
			ResTable_typeSpec *pTypeSpec = new ResTable_typeSpec();
			pTypeSpec->header.headerSize = header->headerSize;
			pTypeSpec->header.type = header->type;
			pTypeSpec->header.size = header->size;
			readBytes(&(pTypeSpec->id),
				header->headerSize - sizeof(ResChunk_header));

			quint32* pFlags = Q_NULLPTR;
			const quint32 count = pTypeSpec->entryCount;
			if (count != 0) {
				pFlags = new quint32[count];
				readBytes(pFlags, sizeof(quint32)*count);
			}

			MyResTableTypeSepc myTypeSpec;
			myTypeSpec.mTypeSpec = pTypeSpec;
			myTypeSpec.mpFlags = pFlags;
			pCurrentTypeSpec = &myTypeSpec;
			mTypeSpecList.append(myTypeSpec);
		}
		break;
		case RES_TABLE_TYPE_TYPE:
		{
			ResTable_type *pType = new ResTable_type();
			pType->header.headerSize = header->headerSize;
			pType->header.size = header->size;
			pType->header.type = header->type;
			readBytes(&(pType->id),
				header->headerSize - sizeof(ResChunk_header));

			int* pEntryOffsets = new int[pType->entryCount];
			for (int i = 0; i < pType->entryCount; i++) {
				readInt(pEntryOffsets[i]);
			}
			//readBytes(&pEntryOffsets[0],
			//	sizeof(quint32) * pType->entryCount);

			quint64 start = fileOffset + pType->entriesStart;
			quint32 entriesSize = 0;
			MyResTableTypeType_entry** pEntries = new MyResTableTypeType_entry*[pType->entryCount];
			for (int i = 0; i < pType->entryCount; i++) {
				int pos = pEntryOffsets[i];
				if (pos == -1) {
					pEntries[i] = Q_NULLPTR;
					continue;
				}
				quint64 seek_pos = start + pos;
				fileSeek(seek_pos);
				pEntries[i] = readResTableTypeTypeEntry();
				pEntries[i]->mAllSize = filePos() - seek_pos;
				entriesSize += pEntries[i]->mAllSize;
			}
			MyResTableTypeType* pTypeType = new MyResTableTypeType();
			pTypeType->mType = pType;
			pTypeType->pEntries = pEntries;
			pTypeType->mEntriesSize = entriesSize;

			pCurrentTypeSpec->pTypeTypes.append(pTypeType);
		}
		break;
		case RES_TABLE_LIBRARY_TYPE:
		{
			if (mMyResTableLib == NULL)
			{
				mMyResTableLib = new MyResTableLib();
			}
			mMyResTableLib->mHeader.header = *header;
			readUInt(mMyResTableLib->mHeader.count);

			const int libCount = mMyResTableLib->mHeader.count;
			ResTable_lib_entry** pEntries = Q_NULLPTR;
			if (libCount > 0)
			{
				pEntries = new ResTable_lib_entry*[libCount];
			}
			for (size_t i = 0; i < libCount; i++)
			{
				ResTable_lib_entry* pEntry = new ResTable_lib_entry();
				readBytes(pEntry, sizeof(ResTable_lib_entry));
				pEntries[i] = pEntry;
			}
			mMyResTableLib->pEntries = pEntries;
		}
		break;
		default:
			break;
		}
		fileOffset += header->size;
		fileSeek(fileOffset);
	}
}

ArscFile::MyResTableTypeType_entry* ArscFile::readResTableTypeTypeEntry() {
	MyResTableTypeType_entry* pResult = new MyResTableTypeType_entry();

	ResTable_entry entry;
	readBytes(&entry, sizeof(ResTable_entry));
	if (0 != (entry.flags & ResTable_entry::FLAG_COMPLEX)) {
		ResTable_map_entry* pEntry = new ResTable_map_entry();
		memcpy(pEntry, &entry, sizeof(ResTable_entry));
		readUInt(pEntry->parent.ident);
		readUInt(pEntry->count);
		pResult->isSimple = false;
		pResult->pMapEntry = pEntry;
		pResult->pMapList = new QList<ResTable_map>();
		pResult->pMapList->reserve(pEntry->count);

		for (size_t i = 0; i < pEntry->count; i++)
		{
			ResTable_map map;
			readBytes(&map, sizeof(ResTable_map));
			pResult->pMapList->append(map);
		}
	}
	else {
		Res_value* pValue = new Res_value();
		readBytes(pValue, sizeof(Res_value));
		pResult->isSimple = true;
		pResult->mEntry = new ResTable_entry(entry);
		pResult->mResVaule = pValue;
	}
	return pResult;
}

//quint32 ArscFile::decodeLength(bool isUtf8) {
//    quint32 result = 0;
//    if (isUtf8) {
//        char length1 = 0;
//        readByte(&length1);
//        result = length1;
//        if (0 != (length1 & 0x80)) {
//            char length2 = 0;
//            readByte(&length2);
//            result = ((result & 0x7F) << 8) | length2;
//        }
//        return result;
//    } else {
//        quint16 len1 = 0;
//        readShort(&len1);
//        result = len1;
//        if (0 != (len1 & 0x8000)) {
//            quint16 len2 = 0;
//            readShort(&len2);
//            result = ((result & 0x7FFF) << 16) | len2;
//        }
//        return result;
//    }
//}
