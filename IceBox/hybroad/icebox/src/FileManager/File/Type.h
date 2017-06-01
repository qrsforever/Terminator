#ifndef __Type__H_
#define __Type__H_

#include <string>

namespace IBox {

typedef enum {
    FileType_Unknown,
    FileType_Audio,
    FileType_Video,
    FileType_Image,
    FileType_Document
} FileType;

class Type {
public:
	static bool isAudioFile(std::string suffix);
	static bool isVideoFile(std::string suffix);
	static bool isImageFile(std::string suffix);
	static bool isDocumentFile(std::string suffix);	
};

} // namespace IBox

#endif

