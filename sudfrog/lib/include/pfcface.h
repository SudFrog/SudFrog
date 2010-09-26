/***************************************************************************
 *   Copyright (C) 2009 by Télécom SudParis. Major Developer: Anouar Mellakh   *
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
#ifndef PFCFACE_H
#define PFCFACE_H

#include "miscellaneous.h"
#include "TMSP_image.h"
#include "mask.h"

/**
 @author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/
namespace TMSP_Face_Space
{
  /**
   * \brief Class that stores all the data we need to do a verification.
  */
  class PFCface
  {
    public:
      /**The Gobal image scene**/
      Pfc_Image Original,OriginalGray;
      /**Geometrically Normalized face**/
      Pfc_Image GeomNorm;
      /** Illumination correction (Histogram Equalization,Gamma transformation,Log)*/
      Pfc_Image LightNorm;
      /** Reserved for the illumination componnet extracted fro anisotropic smoothing LightEnhence*/
      Pfc_Image Reserved;
      /**
       * constructor
       */
      PFCface();
      /**
       * destructor
       */
      ~PFCface();

      /**
       *
       * @param Norm
       */
      void SetNormLight ( int Norm );

      /**
       *
       * @return
       */
      int GetNormLight();

      /**
       *
       * @param Eydist
       */
      void SetEyeDistance ( int Eydist );

      /**
       *
       * @return
       */
      int GetEyeDistance();


      /**
       *
       * @return
       */
      int NormGeometric();

      /**
       *
       * @param method
       * @param param
       * @return
       */
      int LightCorrect ( string method,double* param );


      /**
       *
       * @param methodname
       * @param param
       * @param ROI
       * @return
       */
      int LightCorrect ( string methodname,double* param,Mask ROI );

      /**
       *
       * @param face
       */
      void SetOriginal ( Pfc_Image &face );

      /**
       *
       * @param Fc
       */
      void SetFaceCoordinate ( Face_Coordinates Fc );
      /**
       *
       * @param LEX
       * @param LEY
       * @param REX
       * @param REY
       * @param NX
       * @param NY
       * @param MX
       * @param MY
       */
      void SetFaceCoordinate ( int LEX,int LEY,int REX,int REY,
                               int NX,int NY,int MX,int MY );

      /**
       *
       * @param o
       * @return
       */
      PFCface & operator= ( const PFCface & o );


      /**
       * 
       */
      void Printmeta (  );

      /**
       *
       * @param filename
       */
      void LoadOriginal ( const char* filename );

      /**
       *
       * @param filename
       * @param ext
       */
      void LoadOriginal ( const char* filename,Extension ext );
    private:
      /** face coordinate*/
      Face_Coordinates Face_pts;
      int NormLight;
      int EyesDistance;
      Mask Pfc_mask;
      int8_t interpData ( double xl, double yl, double xu, double yu );

  };
}
#endif
