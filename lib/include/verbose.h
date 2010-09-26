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
#ifndef VERBOSE_H
#define VERBOSE_H

#define CLEARLINE true
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <string>
#include <pthread.h>

using namespace std;

namespace TMSP_Face_Space
{
  /**
  	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
	\brief Class that controls the verbozing of different classes.
  */
  class verbose
  {
    public:
      /**
       * Contructor
       */
      verbose();
      /**
       * Contructor
       * @param f
       */
      verbose ( bool f );
      /**
       *
       * @param maxlevel
       */
      verbose ( int maxlevel );
      /**
       *
       * @param f
       */
      verbose ( const char* f );
      /**
       *
       * @param f1
       * @param f2
       */
      verbose ( bool f1,const char* f2 );

      /**
       *
       */
      ~verbose();
      /**
       *
       * @param filename
       */
      void Setlog ( const char* filename );
      /**
       *
       */
      void Closelog();
      /**
       *
       */
      void Verbose();
      /**
       *
       */
      void UnVerbose();

      verbose& operator<< ( const char* );//ok
      verbose& operator<< ( const string& );//ok
      verbose& operator<< ( char );
      verbose& operator<< ( long );
      verbose& operator<< ( double );
      verbose& operator<< ( float );
      verbose& operator<< ( int );
      verbose& operator<< ( bool );

      /**
       *
       * @param verb
       * @return
       */
      verbose& operator= ( verbose& verb );//ok

      /**
       *
       * @param mx
       */
      void SetMutex ( pthread_mutex_t* mx );

      /**
       *
       * @param l
       * @return
       */
      int setmaxlevel ( int l );

    private:
      int getlevel();
      int getmaxlevel();
      pthread_mutex_t* mout;
      ofstream *outfs;
      bool *verboser,internal;
      int ilevel,verblevel;
      bool iflevelspeak();

  };
}
#endif
