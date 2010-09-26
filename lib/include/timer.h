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

#ifndef PFC_SPACETIMING_H
#define PFC_SPACETIMING_H
#include <time.h>
#include <sys/time.h>
  /**
  	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
  */
namespace TMSP_Face_Space
  {

#define TIKCS 1000000.0

 /**
  *\brief Class that allows to get time performance 
  */
  class Timer
    {
    public:
      /**
       * Constructor
       */
      Timer();

      /**
       * Destructor
       */
      ~Timer();
      /**
       * start the timer
       */
      void start();
      /**
       * turn the elapsed time in second from Timer starting and restart
       * @return the elapsed time
       */
      float Get_Elapsed_restart();
      /**
       * return the elapsed time from Timer starting without restarting
       * @return the elapsed time
       */
      float Get_Elapsed();
      /**
       * return the elapsed time in second from Timer starting without restarting
       * @return the elapsed time in seconds
       */
      float Get_Elapsed_s();
      /**
       * turn the elapsed time in second from Timer starting and restart
       * @return the elapsed time in seconds
       */
      float Get_Elapsed_s_restart();
    private:
      struct timeval tv;
      double starttime,finaltime;

    };

}

#endif
