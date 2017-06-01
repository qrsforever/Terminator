#include "Type.h"

namespace IBox {

static const char* gImageSuffix[] = { "bmp", "gif", "png", "jpg", "jpeg", "jpe", "tiff", "tif", "pcd", "qti", "qtf", "qtif", "ico", "pnm", "ppm" };
static const char* gVideoSuffix[] = { "avi", "wmv", "mp4", "rmvb", "kkv", "3gp", "ts", "mpeg", "mpg", "mkv", "m3u8", "mov", "m2ts", "flv", "m2t", "mts", "vob", "dat", "m4v", "asf", "f4v", "3g2", "m1v", "m2v", "tp", "trp", "m2p", "rm", "avc", "dv", "divx", "mjpg", "mjpeg", "mpe", "mp2p", "mp2t", "mpg2", "mpeg2", "m4p", "mp4ps", "ogm", "hdmov", "qt", "iso" };
static const char* gAudioSuffix[] = { "mp3", "wma", "amr", "aac", "wav", "wave", "ogg", "mka", "ac3", "m4a", "ra", "flac", "ape", "mpa", "aif", "aiff", "at3p", "au", "snd", "dts", "rmi", "mid", "mp1", "mp2", "pcm", "lpcm", "l16", "ram" };
static const char* gDocumentSuffix[] = { "doc", "docx", "txt", "xls", "ppt", "pdf" };

bool Type::isAudioFile(std::string suffix) {
	for (int i = 0; i < sizeof(gAudioSuffix)/sizeof(gAudioSuffix[0]); i++) {
		if (suffix.compare(gAudioSuffix[i]) == 0)
			return true;
	}
	return false;
}

bool Type::isVideoFile(std::string suffix){
	for (int i = 0; i < sizeof(gVideoSuffix)/sizeof(gVideoSuffix[0]); i++) {
		if (suffix.compare(gVideoSuffix[i]) == 0)
			return true;
	}
	return false;
}

bool Type::isImageFile(std::string suffix){
	for (int i = 0; i < sizeof(gImageSuffix)/sizeof(gImageSuffix[0]); i++) {
		if (suffix.compare(gImageSuffix[i]) == 0)
			return true;
	}
	return false;
}

bool Type::isDocumentFile(std::string suffix){
	for (int i = 0; i < sizeof(gDocumentSuffix)/sizeof(gDocumentSuffix[0]); i++) {
		if (suffix.compare(gDocumentSuffix[i]) == 0)
			return true;
	}
	return false;
}


} // namespace IBox


