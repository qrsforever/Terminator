#include "FileExplorerAssertions.h"

#include "File.h"
#include "sha2.h"
#include "HWEncryptor.h"
#include "HWDecryptor.h"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include <map>

namespace IBox {

#define DIR_IS_DOT(x) ((x)[0] == '.' && (x)[1] == '\0')
#define DIR_IS_DOTDOT(x) ((x)[0] == '.' && (x)[1] == '.' && (x)[2] == '\0')

static pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;

static std::map<std::string, File*> gBusyFiles;

File::File(std::string path) 
{	
	pthread_mutex_lock(&gMutex);
    gBusyFiles.insert(std::make_pair(path, this));
    pthread_mutex_unlock(&gMutex);
	
    mPath = path;
    init();
}

File::~File()
{
	pthread_mutex_lock(&gMutex);
    std::map<std::string, File*>::iterator it;
    it = gBusyFiles.find(mPath);
    if (it != gBusyFiles.end())
    	gBusyFiles.erase(it);
    pthread_mutex_unlock(&gMutex);
}

void File::init() {
    mName = mPath.substr(mPath.find_last_of("/") + 1);
    lstat(mPath.c_str(), &mStat);
}

void File::listFiles(FileFilter* filter) {
    DIR* dir;
    struct dirent* ptr;

    if (isFile()) {
        if (filter)
            filter->accept(mPath);
        return ;
    }

    if ((dir = opendir(mPath.c_str())) == NULL) {
        FILEEXPLORER_LOG_ERROR("opendir {%s} failed, No such directory.\n", mPath.c_str());
        return ;
    }

    while ((ptr = readdir(dir)) != NULL) {
		//skip . and ..
        if (DIR_IS_DOT(ptr->d_name) || DIR_IS_DOTDOT(ptr->d_name))
            continue;
        if (filter) {
            filter->accept(mPath + "/" + ptr->d_name);
        }
    }

    closedir(dir);
}

bool File::copyTo(std::string destPath) {
    int src;
    int dest;

    File destFile(destPath);
    destFile.mkParentDir();

    src = open(mPath.c_str(), O_RDONLY);
    if (src == -1) {
        perror("open");
        return false;
    }

    dest = open(destPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mStat.st_mode);
    if (dest == -1) {
        perror("open");
        close(src);
        return false;
    }

    size_t size = 0;

    while (size < mStat.st_size) {
        int rd = 0;
        char buf[2048] = {0};

        rd = read(src, buf, sizeof(buf));
        if (rd == -1 && errno != EINTR)
            break;

        if (rd > 0) {
            int wr = 0;
            char* pbuf = buf;

            while ((wr = write(dest, buf, rd)) != 0) {
                if (wr == -1 && errno != EINTR)
                    break;
                else if (wr > 0) {
                    size += wr;
                    if (wr == rd)
                        break;
                    pbuf += wr;
                    rd -= wr;
                }
            }

            if (wr == -1)
                break;
        }

    }

    close(src);
    close(dest);
    return (size == mStat.st_size);
}

bool File::renameTo(std::string newPath) {
    File newFile(newPath);
    newFile.mkParentDir();

    if (rename(mPath.c_str(), newPath.c_str()) == -1) {
        perror("rename");
        return false;
    }

    mPath = newPath;
    init();
    return true;
}

bool File::remove() {
    return (unlink(mPath.c_str()) == 0);
}

bool File::encrypt(std::string key)
{
	std::string suffix = mName.substr(mName.find_last_of(".") + 1);
	if (suffix.compare("hyencrypt") == 0)
		return true;
	
	int src;
	int dest;
	std::string enPath = mPath + ".hyencrypt";
	File destFile(enPath);
	destFile.mkParentDir();

	src = open(mPath.c_str(), O_RDONLY);
	if (src == -1) {
		perror("open");
		return false;
	}

	dest = open(enPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mStat.st_mode);
	if (dest == -1) {
		perror("open");
		close(src);
		return false;
	}
	
	size_t size = 0;
	const int bufSize = 1024 * 1024;
	HWEncryptor encryptor((unsigned char *)key.c_str(), key.size(), bufSize);
	unsigned char *pInBuf = encryptor.getInputBuffer();
	if (pInBuf != NULL) {
		while (size < mStat.st_size) {
			int rd = 0;
			memset(pInBuf, 0, bufSize);
			rd = read(src, pInBuf, bufSize);
			if (rd == -1 && errno != EINTR)
				break;
			
			if (rd > 0) {
				unsigned char* pbuf = encryptor.encrypt(pInBuf, rd); 
				int wr = 0;
				
				while ((wr = write(dest, pbuf, rd)) != 0) {
					if (wr == -1 && errno != EINTR)
						break;
					else if (wr > 0) {
						size += wr;
						if (wr == rd)
							break;
						pbuf += wr;
						rd -= wr;
					}
				}

				if (wr == -1)
					break;
			}

		}
	}
	close(src);
	close(dest);

	if (size == mStat.st_size && unlink(mPath.c_str()) == 0) {
		mPath = enPath;
		init();
    	return true;
	}
	unlink(enPath.c_str());
	return false;
}

bool File::decrypt(std::string key)
{
	std::string suffix = mName.substr(mName.find_last_of(".") + 1);
	if (suffix.compare("hyencrypt"))
		return false;
	
	int src;
	int dest;
	std::string dePath = mPath.substr(0, mPath.find_last_of("."));
	File destFile(dePath);
	destFile.mkParentDir();

	src = open(mPath.c_str(), O_RDONLY);
	if (src == -1) {
		perror("open");
		return false;
	}

	dest = open(dePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, mStat.st_mode);
	if (dest == -1) {
		perror("open");
		close(src);
		return false;
	}

	size_t size = 0;
	const int bufSize = 1024 * 1024;
	HWDecryptor decryptor((unsigned char *)key.c_str(), key.size(), bufSize);
	unsigned char *pInBuf = decryptor.getInputBuffer();
	if (pInBuf != NULL) {
		while (size < mStat.st_size) {
			int rd = 0;
			memset(pInBuf, 0, bufSize);
			rd = read(src, pInBuf, bufSize);
			if (rd == -1 && errno != EINTR)
				break;
			if (rd > 0) {
				unsigned char* pbuf = decryptor.decrypt(pInBuf, rd); 
				int wr = 0;
				while ((wr = write(dest, pbuf, rd)) != 0) {
					if (wr == -1 && errno != EINTR)
						break;
					else if (wr > 0) {
						size += wr;
						if (wr == rd)
							break;
						pbuf += wr;
						rd -= wr;
					}
				}

				if (wr == -1)
					break;
			}

		}
	}
	close(src);
	close(dest);

	if (size == mStat.st_size
		&& unlink(mPath.c_str()) == 0) {
		mPath = dePath;
		init();
    	return true;
	}
	unlink(dePath.c_str());
	return false;
}

int File::type() {
	std::string suffix = mName.substr(mName.find_last_of(".") + 1);
	if (suffix.length() == 0)
		return FileType_Unknown;
	else if (Type::isAudioFile(suffix))
		return FileType_Audio;
	else if (Type::isVideoFile(suffix))
		return FileType_Video;
	else if (Type::isImageFile(suffix))
		return FileType_Image;
	else if (Type::isDocumentFile(suffix))
		return FileType_Document;
	else 
		return FileType_Unknown;
}

/*-------------------------------------------------------------------------
    Creates the directory named by the trailing filename of this file.
-------------------------------------------------------------------------*/
bool File::mkDir() {
    if (access(mPath.c_str(), F_OK) == 0)
        return true;

    if (mkdir(mPath.c_str(), 0777) == 0)
        return true;
    else
        perror("mkdir");

    return false;
}

/*-------------------------------------------------------------------------
    Creates the directory named by the trailing filename of this file,
    including the complete directory path required to create this directory.
-------------------------------------------------------------------------*/
bool File::mkDirs() {
    std::string parentDir = getParent();
    if (access(parentDir.c_str(), F_OK) != 0) {
        File parent(parentDir);
        parent.mkDirs();
    }

    return mkDir();
}

bool File::mkParentDir() {
    File parentDir(getParent());
    return parentDir.mkDirs();
}

std::string File::getParent() {
    return mPath.substr(0, mPath.find_last_of("/"));
}


/*-------------------------------------------------------------------------
    Returns a boolean indicating whether this file can be found on the underlying
    file system.
-------------------------------------------------------------------------*/
bool File::exists() {
    return (access(mPath.c_str(), F_OK) == 0);
}

std::string File::hashCode() {
    std::string hashStr = "";

    if (isFile()) {
        sha256_ctx ctx;

        sha256_init(&ctx);
        FILE* fp = fopen(mPath.c_str(), "rb");
        if (fp) {
            unsigned char digest[32] = {0};
            while (!feof(fp)) {
                unsigned char buf[1024 * 1024] = {0};
                int len = fread(buf, 1, sizeof(buf), fp);
                sha256_update(&ctx, buf, len);
            }
            sha256_final(&ctx, digest);

            for (int i = 0; i < sizeof(digest); i++) {
                char buf[3] = {0};
                snprintf(buf, sizeof(buf), "%02X", digest[i]);
                hashStr += buf;
            }
			fclose(fp);
        }
    }

    return hashStr;
}



bool 
File::lock(std::string path)
{
	bool found = false;
	pthread_mutex_lock(&gMutex);
    std::map<std::string, File*>::iterator it;
    it = gBusyFiles.find(path);
	found = it != gBusyFiles.end() ? true : false;
	pthread_mutex_unlock(&gMutex);
	if (!found) 
		new File(path);
	return !found;
}

void 
File::unlock(std::string path)
{
	File* file = NULL;
	pthread_mutex_lock(&gMutex);
    std::map<std::string, File*>::iterator it;
    it = gBusyFiles.find(path);
	if (it != gBusyFiles.end())
		file = it->second;
	pthread_mutex_unlock(&gMutex);
	if (file)
		delete file;
}

bool
File::isLocked(std::string path)
{
	bool found = false;
	pthread_mutex_lock(&gMutex);
    std::map<std::string, File*>::iterator it;
    it = gBusyFiles.find(path);
	found = it != gBusyFiles.end() ? true : false;
	pthread_mutex_unlock(&gMutex);
	return found;
}

} // namespace IBox

