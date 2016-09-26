#include "filereader.h"

FileReader::FileReader(const QString &path)
    : mPath(path)
    , mOpened(false)
    , mFile(path)
{

}

FileReader::~FileReader()
{
    if (mpDataStream != Q_NULLPTR) {
        delete mpDataStream;
        mpDataStream = Q_NULLPTR;
    }

    if (mOpened) {
        mFile.close();
    }
}

bool FileReader::initReader(bool bLittleEnd)
{
    if (!mFile.exists()) {
        return false;
    }

    if (!mFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    mOpened = true;

    mpDataStream = new QDataStream(&mFile);
    mpDataStream->setByteOrder(
                bLittleEnd ? QDataStream::LittleEndian : QDataStream::BigEndian);

    return true;

//    bool result = false;
//    do {
//        QDataStream qds(&qfile);
//        qds.setByteOrder(QDataStream::LittleEndian);

//        ResChunk_header header;
//        qds.readRawData((char*)&header, sizeof(ResChunk_header));

//        if (header.size == qfile.size()) {
//            result = true;
//        } else {
//            result = false;
//        }

//    } while(0);

    //    qfile.close();
}

qint64 FileReader::fileSize() {
	return mFile.size();
}

int FileReader::readByte(uchar& buf)
{
    return mpDataStream->readRawData((char*)&buf, 1);
}

int FileReader::readChar(char& buf) {
	return mpDataStream->readRawData(&buf, 1);
}

int FileReader::readShort(short& buf) {
	*mpDataStream >> buf;
	return 0;
}

int FileReader::readUShort(ushort& buf)
{
	*mpDataStream >> buf;
	return 0;
}

int FileReader::readInt(int& buf)
{
	*mpDataStream >> buf;
	return 0;
}

int FileReader::readUInt(uint& buf) {
	*mpDataStream >> buf;
	return 0;
}

int FileReader::readBytes(void *buf, int len) {
	return mpDataStream->readRawData((char*)buf, len);
}

qint64 FileReader::filePos() const
{
    return mFile.pos();
}

bool FileReader::fileSeek(qint64 pos)
{
    return mFile.seek(pos);
}

int FileReader::fileSkip(int len)
{
    return mpDataStream->skipRawData(len);
}


