/***************************************************************************
 *   Copyright (C) 2009 by anouar   *
 *   anouar@eph-6781   *
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
#include "mask.h"

namespace TMSP_Face_Space
{

  Mask::Mask()
  {
    Init ( 50 );
  }

  Mask::Mask ( int eyedist )
  {
    Init ( eyedist );
  }

  Mask::~Mask()
  {
  }

  void Mask::Init ( int EYE_DISTANCE )
  {

    NORMALIZED_ROW = ( int ) ( 2.56*EYE_DISTANCE +0.5f );
    NORMALIZED_COL=NORMALIZED_ROW;//(int)(0.91*heightNorm +0.5f);

// compute the center of the mask (ellipse)
    ellipseX0 = ( int ) ( 0.5f*NORMALIZED_COL +0.5f );
// ellipse center at 53*rows/150
    ellipseY0 = ( int ) ( 0.45f*NORMALIZED_ROW +0.5f );

    int ellipseSemiMinor = ellipseX0-1;
    int ellipseSemiMajor = ( int ) ( ( 1.5f ) *ellipseSemiMinor +0.5f );

// compute the square of the height and width of the mask (ellipse)
    ellipseSemiMinor2 = ellipseSemiMinor*ellipseSemiMinor;
    ellipseSemiMajor2 = ellipseSemiMajor*ellipseSemiMajor;

    //cout << NORMALIZED_ROW << "x" << NORMALIZED_COL << endl;

  }


  bool Mask::Ismask ( const int& x,const int& y )
  {
    double X = ( double ) ( ellipseX0-x+1 );
    double Y = ( double ) ( ellipseY0-y+1 );

    if ( ( ( X*X ) /ellipseSemiMinor2 + ( Y*Y ) /ellipseSemiMajor2 ) < 1.f )
      return true;
    else
      return false;
  }

  void Mask::ApplyMask ( double *&imageout,int imageRows,int imageCols )
  {
    int k=0;
    for ( int i=0; i<imageRows; ++i )
      for ( int j=0; j<imageCols; ++j ) {
        if ( !Ismask ( j+1,i-2 ) )
          imageout[k]=0;

        ++k;
      }

  }

  int Mask::getwidth()
  {
    return NORMALIZED_COL;
  }
  int Mask::getheight()
  {
    return NORMALIZED_ROW;
  }

}
