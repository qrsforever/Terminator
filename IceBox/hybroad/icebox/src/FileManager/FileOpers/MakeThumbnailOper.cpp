#include "MakeThumbnailOper.h"
#include "Thumbnail.h"
#include "Message.h"
#include "File.h"
#include "StringData.h"
#include "JsonObject.h"

namespace IBox {

bool ThumbnailMaker::makeThumbnail(std::string in, std::string out, int quality, int max) 
{
    Thumbnail thumb;
    return (thumb.create(in.c_str(), out.c_str(), quality, max) == Thumbnail::R_SUCCESS);
}

bool ThumbnailMaker::makeThumbnailBySize(std::string in, std::string out, int w, int h) 
{
    Thumbnail thumb;
    return (thumb.createBySize(in.c_str(), out.c_str(), w, h) == Thumbnail::R_SUCCESS);
}

bool ThumbnailMaker::makeThumbnail(std::string in, std::string out, float scale) 
{
    Thumbnail thumb;
    return (thumb.createByScale(in.c_str(), out.c_str(), scale) == Thumbnail::R_SUCCESS);
}

bool ThumbnailMaker::makeIcon(std::string path) 
{
    File icon(path + ".hyicon");
    return icon.exists() ? true : makeThumbnailBySize(path, icon.path(), 150, 150);
}	

MakeThumbnailOper::MakeThumbnailOper(int code, std::string request) 
	: FileOper(code, request)	
{
	
}	

Result MakeThumbnailOper::execute() 
{
	JsonObject requestObj(mOperRequest);
    File thumb(requestObj.getString("Path"));
	std::string filePath = thumb.path().substr(0, thumb.path().find_last_of("."));
    bool result = thumb.exists() ?
    	true : ThumbnailMaker::makeThumbnail(filePath, thumb.path(), 80, 1280);
	return Result::OK;
}

} // namespace IBox