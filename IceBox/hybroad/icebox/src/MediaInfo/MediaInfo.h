/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef CLI_MainH
#define CLI_MainH
//---------------------------------------------------------------------------
#include <string>
//***************************************************************************
//
//***************************************************************************
namespace IBox {

typedef struct MiAudioInfo* MiAudioInfoPtr;
struct MiAudioInfo {
    std::string mAlbum;     //专辑
    std::string mPerformer; //歌手
    std::string mDuration;  //时长
    std::string mBitRate;   //码率
};

class MediaInfo
{
public:
    MediaInfo() {}
    ~MediaInfo() {}
    int getInfo(const char *file, MiAudioInfoPtr sMi);
private:
    void mediaInfoAnalyze(std::string info, MiAudioInfoPtr sMi);
};

}

#endif

