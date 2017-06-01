#include "FileExplorerAssertions.h"
#include "ExifReader.h"

#include <utility>      // std::pair
#include <vector>
#include <stdio.h>

namespace IBox {

static void readExifEntry(ExifEntry* entry, void* data) {
    std::vector< std::pair<std::string, std::string> >* exifData =
        (std::vector< std::pair<std::string, std::string> >*)data;

    char buf[2048] = {0};
    exif_entry_get_value(entry, buf, sizeof(buf));
    exifData->push_back(std::make_pair(exif_tag_get_name(entry->tag), buf));
}

static void readExifContent(ExifContent* content, void* data) {
    exif_content_foreach_entry(content, readExifEntry, data);
}

bool getExifDataFromFile(std::string path, void* data) {
    ExifData* d = NULL;
    d = exif_data_new_from_file(path.c_str());
    if (d == NULL) {
        FILEEXPLORER_LOG_WARNING("No exif data or an error occur.\n");
        return false;
    }
    exif_data_foreach_content(d, readExifContent, data);
    exif_data_unref(d);
    return true;
}


} // namespace IBox