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
#ifndef TEMPLATOR_H
#define TEMPLATOR_H
#include <pthread.h>
#include "linearreducer.h"
#include "nonlinearreducer.h"
#include "gaborate.h"
#include "TMSP_image.h"
#include "miscellaneous.h"
#include "verbose.h"

  /**
  	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
  */

namespace TMSP_Face_Space
{

  /**
  * \brief Class that extracts template from a given, geometrically normalized and corrected illumination, face 
  */
  class Templator
  {
    public:
      LinearReducer Lin; /** LinearReducer object for the Linear reduction problem like (PCA, LDA and DLDA) **/
      NonLinearReducer NonLin; /** NonLinearReducer object for the NonLinear reduction problem like (KFA and GDA) **/
      Gaborate Pfc_Gab /** a Gaborate object to compute the gabor filtering step*/;
      /**
      *Constructor
      */
      Templator();
      /**
       *Constructor
       * @param SpaceRed 
       */
      Templator ( const char* SpaceRed );
      /**
       *Destructor
       */
      ~Templator();
      /**
       *
       * @param SpaceRed
       */
      void SetSpacefile ( const char* SpaceRed );
      /**
       *
       * @return
       */
      vector <double> Template();
      /**
       *
       * @param F
       * @return
       */
      vector <double> Template ( Matrix &F );
      /**
       *
       * @param F
       * @return
       */
      vector <double> Template ( Pfc_Image &F );
      /**
       *
       * @param height
       * @param width
       * @param nscale
       * @param norient
       * @param minWaveLength
       * @param mult
       * @param sigmaOnf
       * @param dThetaOnSigma
       */
      void  CreateFilters ( int height, int width,int nscale,int norient,int minWaveLength, float mult,float  sigmaOnf,float dThetaOnSigma );
      /**
       *
       * @param templ
       * @return
       */
      vector <double> LoadTemplate ( const char* templ );
      /**
       *
       */
      void SetNoSpace(); // to bypass template control;
      /**
       *
       * @return
       */
      bool GetProblem();
      /**
       *
       * @param eyedist
       */
      void SetMask ( int eyedist );
      /**
       *
       * @param val
       */
      void SetGaborReduction ( int val );
      /**
       *
       * @param templ
       */
      void SaveTemplate ( const char* templ );
      /**
       *
       * @return
       */
      ReturnMatrix GetTemplate ();
      /**
       *
       * @param meth
       */
      void SetGaborMethod ( string meth );
      /**
       *
       * @return
       */
      string GetGaborMethod();
      /**
       *
       * @param v
       */
      void SetVerbose ( verbose &  v);
      /**
       *
       * @param mx
       */
      void SetMutex ( pthread_mutex_t* mx);
    private:
      pthread_mutex_t *mutexout; //for gaborate class
      verbose verb;
      ColumnVector TmpLate;
      bool Problem,loadedspace,nospace,loadedGabor; //false linear, true nonlinear


      void assertlinear();
      void assertnonlinear();
      void GetkernelParam ( string line );

  };

}

#endif
