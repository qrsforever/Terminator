#ifndef _TempFile_H_
#define _TempFile_H_

#ifdef __cplusplus

#include <string>

namespace IBox {

class TempFile {
public:
    TempFile(const char* pFilePath);
    ~TempFile();

    const char *filePath();

    virtual int open(char openType);

    virtual void close();

    virtual int read(void* buffer, int length);
    virtual int write(void* data, int length);

    virtual int seek(int offset);
    virtual int size();
    virtual void unlink();
private:
    int m_fd;
    std::string m_filePath;
};

} /* namespace IBox */

#endif /* __cplusplus */

#endif /* _TempFile_H_ */
