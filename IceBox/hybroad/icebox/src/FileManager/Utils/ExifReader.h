#ifndef __ExifReader__H_
#define __ExifReader__H_

#include "libexif/exif-data.h" 
#include "libexif/exif-system.h" 

#include <string>
#include <iostream>

namespace IBox {

bool getExifDataFromFile(std::string path, void *data);

} // namespace IBox

#endif
