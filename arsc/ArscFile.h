#ifndef ARSCFILE_H
#define ARSCFILE_H

#include "filereader.h"
#include "resourcetypes.h"

#include <QString>
#include <QList>

class ArscFile : public FileReader
{
public:
    ArscFile(const QString& path);
    ~ArscFile();

    bool parseFile();

private:
    struct StringLen {
        bool isUtf8;
        // origin data for utf8

        unsigned char* data;
        // origin data for utf16
        char16_t* data16;
        // data_len
        int dataLen;

        // count out str length.
        quint32 u8Len;
        // utf16 len. isUtf8 has this.
        quint32 u16Len;

        int stringLen() {
          return isUtf8 ? u8Len : u16Len;
        }
    };

    struct MyResStringPool {
        ResStringPool_header* mHeader;
        quint32* mStringEntries;
        char** mStrings;
        StringLen** mStringLens;
        quint64 mStringSize;

        quint32* mStyleEntries;
        ResStringPool_span** mStyles;
    };

	struct MyResTableTypeType_entry {
		bool isSimple;
		ResTable_entry* mEntry;
		Res_value* mResVaule;

		ResTable_map_entry* pMapEntry;
		QList<ResTable_map>* pMapList;
		quint32 mAllSize;
	};

	struct MyResTableTypeType {
		ResTable_type* mType;

		MyResTableTypeType_entry** pEntries;
		quint32 mEntriesSize;
	};

	struct MyResTableTypeSepc {
		ResTable_typeSpec* mTypeSpec;

		quint32* mpFlags;
		QList<MyResTableTypeType*> pTypeTypes;
	};

	struct MyResTableLib {
		ResTable_lib_header mHeader;
		ResTable_lib_entry** pEntries;
	};

    QString mArsc;

    ResChunk_header *mHeader;
    quint32 mPackageCount;
    MyResStringPool *mResStringPool; // R.strings.xxx
    ResTable_package *mPackages;
    MyResStringPool *mTypeStringPool; // attr drawable id string xxx ==
    MyResStringPool *mKeyStringPool;//attr name exp.==
	QList<MyResTableTypeSepc> mTypeSpecList;
	MyResTableLib* mMyResTableLib;

    ResChunk_header *readResChunkHeader();
    MyResStringPool *readResStringPool();
    StringLen *readStringLen(bool isUtf8);
	uint readUtf16StringLen(ushort* data, int& dataLen);
    uint readUtf8StringLen(unsigned char* data, int& dataLen);
    ResStringPool_span* readResStringPoolSpan();
    ResTable_package* readPackage();
	void readResTableTypeSepc();
	MyResTableTypeType_entry* readResTableTypeTypeEntry();
};

#endif // ARSCFILE_H
