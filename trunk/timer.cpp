/*****************************************************************************
 * noname proxy
 *****************************************************************************
 *
 *****************************************************************************
 * This program is free software; you can redistribute it and*or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *****************************************************************************/

#include "timer.h"

#ifdef WIN32
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
        #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
        #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

/**************************************************************************
 * i stole this code from 
 * http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/
 * "This entry was written by ponnada 
 *  and posted on December 26, 2006 at 12:05 pm"
 **************************************************************************/

int Timer::gettimeofday (struct timeval *tv, struct timezone *tz)
{
        FILETIME ft;
        unsigned __int64 tmpres = 0;
        static int tzflag;
 
        if (NULL != tv)
        {
                GetSystemTimeAsFileTime(&ft);
 
                tmpres |= ft.dwHighDateTime;
                tmpres <<= 32;
                tmpres |= ft.dwLowDateTime;
 
                /*converting file time to unix epoch*/
                tmpres /= 10;  /*convert into microseconds*/
                tmpres -= DELTA_EPOCH_IN_MICROSECS; 
                tv->tv_sec = (long)(tmpres / 1000000UL);
                tv->tv_usec = (long)(tmpres % 1000000UL);
        }
 
        if (NULL != tz)
        {
                if (!tzflag)
                {
                        _tzset();
                        tzflag++;
                }
                tz->tz_minuteswest = _timezone / 60;
                tz->tz_dsttime = _daylight;
        }

        return 0;
}

#else

#include <sys/time.h>

int Timer::gettimeofday (struct timeval *tv, struct timezone *tz)
{
        int ret = ::gettimeofday (tv, tz);
        return ret;
}

#endif

