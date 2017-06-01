#include "TempFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace IBox {

TempFile::TempFile(const char* pFilePath)
	: m_fd(-1)
	, m_filePath(pFilePath)
{
}

TempFile::~TempFile()
{
  //  close();
 //   unlink(m_filePath.c_str());
}

const char* 
TempFile::filePath()
{
    return m_filePath.c_str();
}

int 
TempFile::open(char openType)
{
    // Check that m_fd is -1, if that's not the case it means that an open file
    // has not been closed, so close it before continuing.
    if (m_fd != -1)
        close();
    if (openType == 'w')
        m_fd = ::open(m_filePath.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
    else if (openType == 'r')
        m_fd = ::open(m_filePath.c_str(), O_RDONLY);

    return m_fd;
}

void 
TempFile::close()
{
    if (m_fd >= 0) {
        ::fsync(m_fd);
        ::close(m_fd);
    }
    m_fd = -1;
}

int 
TempFile::read(void* buffer, int length)
{
    return ::read(m_fd, buffer, length);
}

int 
TempFile::write(void* data, int length)
{
    return ::write(m_fd, data, length);
}

int 
TempFile::seek(int offset)
{
    if (lseek(m_fd, offset, SEEK_SET) == -1)
        return -1;
    return 0;
}

int 
TempFile::size()
{
    struct stat fileStat;

    if (m_fd == -1)
        return -1;

    if (fstat(m_fd, &fileStat) < 0) 
        return -1;

    return fileStat.st_size;
}

void 
TempFile::unlink()
{
     ::unlink(m_filePath.c_str());
}

} /* namespace IBox */
