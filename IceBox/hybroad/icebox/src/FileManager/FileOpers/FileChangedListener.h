#ifndef __FileChangedListener__H_
#define __FileChangedListener__H_

#include <string>

namespace IBox {

class FileChangedListener {
public:	
	virtual void onFileCreated(std::string user, std::string path) = 0;
	virtual void onDirCreated(std::string user, std::string path) = 0;
	virtual void onFileDeleted(std::string user, std::string path) = 0;
	virtual void onDirDeleted(std::string user, std::string path) = 0;
	virtual void onFileRealDeleted(std::string user, std::string path) = 0;
	virtual void onDirRealDeleted(std::string user, std::string path) = 0;
	virtual void onFileRenamed(std::string oldPath, std::string newPath) = 0;
	virtual void onDirRenamed(std::string oldPath, std::string newPath) = 0;
	virtual void onSizeChanged(std::string path) = 0;
	virtual void onHashCodeChanged(std::string path, std::string hash) = 0;
	virtual void onOwnerChanged(std::string user, std::string toUser, std::string path) = 0;
	virtual void onFileRecovered(std::string user, std::string path) = 0;
	virtual void onDirRecovered(std::string user, std::string path) = 0;
	virtual void onFileBackuped(std::string src, std::string dest) = 0;
	virtual void onDirBackuped(std::string src, std::string dest) = 0;	
	virtual void onShareChanged(std::string user, std::string toUser, std::string path, bool shared) = 0;
	virtual void onFileEncrypted(std::string src, std::string dest) = 0;
	virtual void onFileDecrypted(std::string oldPath, std::string newPath) = 0;
};

} // namespace IBox

#endif

