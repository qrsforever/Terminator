#include "XmppAssertions.h"
#include "XmppLocalPlayer.h"

#include "PlatformAPI.h"
#include "MessageHandler.h"

namespace IBox {
    
static uint32_t gMagicCount = 0;
 
XmppLocalPlayer::XmppLocalPlayer(int idx, std::string jid, MessageHandler* handler)
    : mAppJid(jid), mPlayerIndex(idx)
    , mMagicNumber(gMagicCount++)
    , mTotalTime(0)
    , mCurrentTime(0)
    , mCurrentAudioChannel(0)
    , mCurrentAudioTrack(0)
    , mHandler(handler)
{
    XMPP_LOG("\n");
    yhw_aout_getVolume(&mCurrentVolume);
    yhw_aout_getMute(&mMuteStatus);
}

XmppLocalPlayer::~XmppLocalPlayer()
{
    XMPP_LOG("\n");
    stop();
}

int
XmppLocalPlayer::play(std::string url)
{
    XMPP_LOG("url = %s\n", url.c_str());
    mUrl = url;
    return ymm_stream_playerStart(mPlayerIndex, (char*)mUrl.c_str(), 1);
}

int
XmppLocalPlayer::pause()
{
    XMPP_LOG("\n");
    return ymm_stream_playerSetTrickMode(mPlayerIndex, YX_PAUSE, 0);
}

int
XmppLocalPlayer::resume()
{
    XMPP_LOG("\n");
    return ymm_stream_playerSetTrickMode(mPlayerIndex, YX_NORMAL_PLAY, 0);
}

int
XmppLocalPlayer::seekTo(int pTime)
{
    XMPP_LOG("time=%d\n", pTime);
    return ymm_stream_playerSetTrickMode(mPlayerIndex, YX_SEEK, pTime);
}

int
XmppLocalPlayer::fastForward(int pSpeed)
{
    XMPP_LOG("speed: %d\n", pSpeed);
    return ymm_stream_playerSetTrickMode(mPlayerIndex, YX_FAST_FORWARD, pSpeed);
}

int
XmppLocalPlayer::fastRewind(int pSpeed)
{
    XMPP_LOG("speed: %d\n", pSpeed);
    return ymm_stream_playerSetTrickMode(mPlayerIndex, YX_FAST_REW, pSpeed);
}

int
XmppLocalPlayer::stop()
{
    return ymm_stream_playerStop(mPlayerIndex);
}

unsigned int
XmppLocalPlayer::getTotalTime()
{
    ymm_stream_playerGetTotalTime(mPlayerIndex, &mTotalTime);
    XMPP_LOG("totaltime: %u\n", mTotalTime);
    return mTotalTime;
}

unsigned int
XmppLocalPlayer::getCurrentTime()
{
    ymm_stream_playerGetPlaytime(mPlayerIndex, &mCurrentTime);
    XMPP_LOG("currenttime: %u\n", mCurrentTime);
    return mCurrentTime;
}

int
XmppLocalPlayer::setVolume(int pVolume)
{
    return yhw_aout_setVolume(pVolume);
}

int
XmppLocalPlayer::setMute(int pMute)
{
    return yhw_aout_setMute(pMute);
}

int 
XmppLocalPlayer::convert(std::string& str)
{
    if (!str.compare("Pause"))    // high-frenquency
        return LOCALPLAYER_PAUSE;
    if (!str.compare("Resume"))   // high-frenquency
        return LOCALPLAYER_RESUME;
    if (!str.compare("SetVolume"))
        return LOCALPLAYER_SETVOLUME;
    if (!str.compare("Seek"))
        return LOCALPLAYER_SEEK;
    if (!str.compare("SetMute"))
        return LOCALPLAYER_SETMUTE;
    if (!str.compare("GetTotalTime"))
        return LOCALPLAYER_GETTOTALTIME;
    if (!str.compare("GetCurrentTime"))
        return LOCALPLAYER_GETCURRENTTIME; 
    if (!str.compare("Create"))
        return LOCALPLAYER_CREATE;
    if (!str.compare("Play"))
        return LOCALPLAYER_PLAY;
    if (!str.compare("Close"))
        return LOCALPLAYER_CLOSE;
    if (!str.compare("FastForward"))
        return LOCALPLAYER_FASTFORWARD;
    if (!str.compare("FastRewind"))
        return LOCALPLAYER_FASTREWIND;
    if (!str.compare("SwitchChannel"))
        return LOCALPLAYER_SWITCHAUDIOCHANNEL;
    if (!str.compare("SwitchTrack"))
        return LOCALPLAYER_SWITCHAUDIOTRACK;
    if (!str.compare("SwitchSubtitle"))
        return LOCALPLAYER_SWITCHSUBTITLE;
    return -1;
}

#if 0
int
XmppLocalPlayer::switchAudioChannel(int)
{
    ymm_decoder_getAudioChannel(&mCurrentAudioChannel);
    mCurrentAudioChannel++;
    if(mCurrentAudioChannel > YX_AUDIO_CHANNEL_OUTPUT_SWAP)
        mCurrentAudioChannel = YX_AUDIO_CHANNEL_OUTPUT_STEREO;
    return ymm_decoder_setAudioChannel(mCurrentAudioChannel);
}

int
XmppLocalPlayer::switchAudioTrack(char *rAudioLanguage)
{
    int tAudioNum = 0;

    ymm_decoder_getNumOfAudio(&tAudioNum); 
    if(tAudioNum > 1) {
        mCurrentAudioTrack++;
        if(mCurrentAudioTrack >= tAudioNum)
            mCurrentAudioTrack = 0;
        ymm_decoder_setAudioPIDIndex(mCurrentAudioTrack);
        ymm_decoder_getAudioLanguageFromIndex(mCurrentAudioTrack, &rAudioLanguage);
        return 0;
    } else {
        return -1;
    }
}

int
XmppLocalPlayer::switchSubtitle(char *rSubtitleLanguage)
{
    int tSubtitleNum = 0;

    ymm_stream_subPlayerGetLanguageNum(0, &tSubtitleNum);
    if(tSubtitleNum == 0) {
        return -1;
    } else {
        mCurrentSubtitle++;
        if(mCurrentSubtitle >= tSubtitleNum)
            mCurrentSubtitle = 0;
        ymm_decoder_setSubtitlePIDIndex(mCurrentSubtitle);
        ymm_decoder_showSubtitle();
        ymm_decoder_getSubtitleLangFromIndex(mCurrentSubtitle, &rSubtitleLanguage);
        return 0;
    }
}
#endif

}

