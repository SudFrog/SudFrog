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
#include "verbose.h"
namespace TMSP_Face_Space
{
  verbose::verbose()
  {
    verboser=new bool;
    outfs=new ofstream;
    internal=true;
    //*verboser=NULL;
    ilevel=0;
    verblevel=0;
    mout=NULL;
//cout << "test" << endl;
// *verboser=true;
  }

  verbose::verbose ( int mlevel )
  {

    verboser=new bool;
    outfs=new ofstream;
    internal=true;
    *verboser=true;
    ilevel=0;
    verblevel=mlevel;
    mout=NULL;

  }


  verbose::verbose ( bool f )
  {
    verboser=new bool;
    outfs=new ofstream;
    *verboser=f;
    internal=f;
    ilevel=0;
    verblevel=0;
    mout=NULL;
  }

  verbose::verbose ( const char* f )
  {
    verboser=new bool;
    *verboser=true;
    outfs=new ofstream;
    ilevel=0;
    verblevel=0;
    mout=NULL;
    Setlog ( f );
  }

  verbose::verbose ( bool f1,const char* f2 )
  {
    verboser=new bool;
    outfs=new ofstream;
    *verboser=f1;
    ilevel=0;
    verblevel=0;
    mout=NULL;
    Setlog ( f2 );
  }

  verbose::~verbose()
  {
    //delete outfs;
    //delete verboser;
// never close logging from here closed from the first parent
//Error: Closelog();
  }

  void verbose::Verbose() {internal=true;}
  void verbose::UnVerbose() {internal=false;}

// template<class T>
// verbose& verbose::operator<< ( T f )
// {
//   if ( verboser ) {
//   if ( mout )
//       pthread_mutex_lock ( & ( *mout ) );
//     cout << f;
//    // cout.flush();
//     if ( ( *outfs ).good() )
//       ( *outfs )<< f;
//     if ( mout )
//       pthread_mutex_unlock ( & ( *mout ) );
//   }
//   return *this;
// }

  verbose& verbose::operator<< ( const char* f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;

      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }

  verbose& verbose::operator<< ( const string& f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;

      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }


  verbose& verbose::operator << ( long f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;

      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }

  verbose& verbose::operator<< ( float f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;

      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }

    return *this;
  }

  verbose& verbose::operator<< ( double f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;
      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }

  verbose& verbose::operator<< ( int f )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );

      cout << f;
      // cout.flush();
      if ( ( *outfs ).good() )
        ( *outfs ) << f;
      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }

  verbose& verbose::operator<< ( bool )
  {
    if ( *verboser && internal && iflevelspeak() ) {
      if ( mout )
        pthread_mutex_lock ( & ( *mout ) );
      cout << "\r";
      cout.flush();
      if ( mout )
        pthread_mutex_unlock ( & ( *mout ) );
    }
    return *this;
  }

  verbose& verbose::operator= ( verbose& V )
  {
    ilevel=V.getlevel() +1;
    verblevel=V.getmaxlevel();
    outfs=V.outfs;
    verboser=new bool;
    verboser=V.verboser;
    internal=*verboser;
    return *this;
  }

  bool verbose::iflevelspeak()
  {
    if ( ilevel<=verblevel )
      return true;
    else
      return false ;
  }

  int verbose::setmaxlevel ( int l )
  {
    verblevel=l;
    return verblevel;
  }

  int verbose::getlevel()
  {
    return ilevel;
  }

  int verbose::getmaxlevel()
  {
    return verblevel;
  }

  void verbose::SetMutex ( pthread_mutex_t* t )
  {
    mout=t;
  }

  void verbose::Setlog ( const char* filename )
  {
    *verboser=true;
    internal=true;
    if ( ( *outfs ).is_open() ) {
      ( *outfs ).close();
      ( *outfs ).open ( filename );
    } else
      ( *outfs ).open ( filename );
  }

  void verbose::Closelog()
  {
    if ( ( *outfs ).is_open() )
      ( *outfs ).close();
  }

}

