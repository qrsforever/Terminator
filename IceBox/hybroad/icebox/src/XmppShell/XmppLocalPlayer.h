#ifndef _XmppLocalPlayer_H_
#define _XmppLocalPlayer_H_

#include <string>

#define LOCALPLAYER_CREATE               0
#define LOCALPLAYER_CLOSE                1
#define LOCALPLAYER_PLAY                 2
#define LOCALPLAYER_PAUSE                3
#define LOCALPLAYER_RESUME               4
#define LOCALPLAYER_FASTFORWARD          5
#define LOCALPLAYER_FASTREWIND           6
#define LOCALPLAYER_SEEK                 7
#define LOCALPLAYER_GETTOTALTIME         8
#define LOCALPLAYER_GETCURRENTTIME       9
#define LOCALPLAYER_SETVOLUME            10
#define LOCALPLAYER_SETMUTE              11
#define LOCALPLAYER_SWITCHAUDIOCHANNEL   12
#define LOCALPLAYER_SWITCHAUDIOTRACK     13
#define LOCALPLAYER_SWITCHSUBTITLE       14

namespace IBox {

class MessageHandler;
class XmppLocalPlayer {
public:
    XmppLocalPlayer(int, std::string, MessageHandler* handler = 0);
    ~XmppLocalPlayer();

    static int convert(std::string& str);
  
    // Player Control
    int play(std::string url);
    int pause();
    int resume();
    int seekTo(int);
    int fastForward(int);
    int fastRewind(int);
    int stop();
    unsigned int getTotalTime();
    unsigned int getCurrentTime();

    /* Audio Control */
    int setVolume(int);
    int getVolume(void) { return mCurrentVolume; }
    int setMute(int);
    int getMute(void) { return mMuteStatus; }

    std::string switchAudioChannel() { return "not finished"; }
    std::string switchAudioTrack() { return "not finished"; }
    std::string switchSubtitle() { return "not finished"; }

#if 0
    int switchAudioChannel(int);
    int getAudioChannel(int pInstanceId) { return mAudioChannel; }
    int switchAudioTrack(char *rAudioLanguage);
    int getAudioTrack(int pInstanceId) { return mAudioTrack; }

    // Subtitle Control
    void switchSubtitle(char *rSubtitleLanguage);
    int getCurrentSubtitleInfo(std::string& strValue);
    int getCurrentPlayUrl(std::string& strValue);
#endif

    std::string& getAppJid() { return mAppJid; }
    int getPlayerID() { return mMagicNumber; }

private :
    unsigned int mTotalTime;
    unsigned int mCurrentTime;
    int mPlayerIndex;
    int mMagicNumber;
    int mMuteStatus;
    int mCurrentVolume;
    int mCurrentAudioChannel;
    int mCurrentAudioTrack;
    std::string mUrl;
    std::string mAppJid;
    MessageHandler* mHandler;
};

} // namespace IBox

#endif // _XmppLocalPlayer_H_
