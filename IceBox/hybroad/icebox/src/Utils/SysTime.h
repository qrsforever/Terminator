#ifndef _SysTime_H_
#define _SysTime_H_

#include <stdint.h>

#ifdef __cplusplus

namespace IBox {

/** Platform-implemented utilities to return time of day, and millisecond counter.
*/
class SysTime {
public:
    struct DateTime {
        uint16_t mYear;          //!< e.g. 2005
        uint8_t  mMonth;         //!< 1..12
        uint8_t  mDayOfWeek;     //!< 0..6, 0==Sunday
        uint8_t  mDay;           //!< 1..31
        uint8_t  mHour;          //!< 0..23
        uint8_t  mMinute;        //!< 0..59
        uint8_t  mSecond;        //!< 0..59
    };
    static void GetDateTime(DateTime*);

    static uint32_t GetMSecs();
};

} // namespace IBox

#endif // __cplusplus

#endif // _SysTime_H_
