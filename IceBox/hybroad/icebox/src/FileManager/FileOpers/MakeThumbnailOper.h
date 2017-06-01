#ifndef __MakeThumbnailOper__H_
#define __MakeThumbnailOper__H_

#include "Result.h"
#include "FileOper.h"

namespace IBox {

class ThumbnailMaker {
public:
	static bool makeIcon(std::string path);
	static bool makeThumbnail(std::string in, std::string out, int w, int h);
	static bool makeThumbnailBySize(std::string in, std::string out, int w, int h);
	static bool makeThumbnail(std::string in, std::string out, float scale);	
};

class MakeThumbnailOper : public FileOper {
public:
    MakeThumbnailOper(int code, std::string request);
    Result execute();
};

} // namespace IBox

#endif

