/***************************************************************************
 *   Copyright (C) 2009 by Télécom SudParis. Major Developer: Anouar Mellakh   *
 *   reference-system@it-sudparis.eu                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "timer.h"

namespace TMSP_Face_Space
  {

  Timer::Timer()
  {
  }


  Timer::~Timer()
  {
  }

  void Timer::start()
  {
    gettimeofday(&tv, NULL);
    starttime=tv.tv_sec*TIKCS + tv.tv_usec;
  }

  float Timer::Get_Elapsed_restart()
  {
    gettimeofday(&tv, NULL);
    finaltime=tv.tv_sec*TIKCS + tv.tv_usec;
    float val=finaltime-starttime;
    starttime=finaltime;
    return val;
  }

  float Timer::Get_Elapsed()
  {
    gettimeofday(&tv, NULL);
    finaltime=tv.tv_sec*TIKCS + tv.tv_usec;
    return (finaltime-starttime);
  }

  float Timer::Get_Elapsed_s()
  {
    return (float)Get_Elapsed()/TIKCS;
  }

  float Timer::Get_Elapsed_s_restart()
  {
    return (float)Get_Elapsed_restart()/TIKCS;
  }

}
