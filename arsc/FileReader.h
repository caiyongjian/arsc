#ifndef FILEREADER_H
#define FILEREADER_H

#include <QString>
#include <QFile>
#include <QDataStream>

class FileReader
{
public:
    FileReader(const QString &path);
    ~FileReader();

    bool initReader(bool bLittleEnd);

	qint64 fileSize();

    int readByte(uchar& buf);
	//int readUchar(uchar* buf);
	int readChar(char& buf);

	int readShort(short& buf);
	int readUShort(ushort& buf);

	int readInt(int& buf);
	int readUInt(uint& buf);

    //int readBytes(uchar* buf, int len);

	int readBytes(void* buf, int len);

    qint64 filePos() const;

    bool fileSeek(qint64 pos);

    int fileSkip(int len);

private:
    QString mPath;

    bool mOpened;

    QFile mFile;

    QDataStream* mpDataStream;
};

#endif // FILEREADER_H
