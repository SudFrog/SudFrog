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
  /**
  	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com >
  */
#ifndef NONLINEARREDUCER_H
#define NONLINEARREDUCER_H

#include "kernel.h"
#include "Matrix_util.h"
#include "verbose.h"
#include "gaborate.h"


using namespace std;
namespace TMSP_Face_Space
{

#define reg 0.001
/** \brief the supported Nonlinear problems  */
  enum NProblem {
    KFA=0,/**<Kernel Fisher Analysis */
    GDA /**<General Discriminant Analysis */
  };

  /**
   * \brief Class used to reduce the input space by nonlinear methods using kernel approaches  (KFA,GDA)
   */
  class NonLinearReducer
  {

    public:

      kernel ker;
      vector <int> labels;
      Matrix EigenVectors;
      ColumnVector bias;


      /**
       * Constructor
       */
      NonLinearReducer();
      /**
       *
       * @param A
       * @param L
       */
      NonLinearReducer ( Matrix &A,vector<int>& L );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      NonLinearReducer ( Matrix &A,vector<int>& L,kernel &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      NonLinearReducer ( Matrix &A,vector<int>& L,kerlin &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      NonLinearReducer ( Matrix &A,vector<int>& L,kerpoly &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      NonLinearReducer ( Matrix &A,vector<int>& L,kerrbf &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      NonLinearReducer ( Matrix &A,vector<int>& L,kersigmoid &k );
      /**
       *
       */
      ~NonLinearReducer();

      /**
       *
       * @param A
       * @param L
       */
      void SetDatawithLabels ( Matrix &A,vector<int>& L);
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      void Set ( Matrix &A,vector<int>& L,kerlin &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      void Set ( Matrix &A,vector<int>& L,kerpoly &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      void Set ( Matrix &A,vector<int>& L,kerrbf &k );
      /**
       *
       * @param A
       * @param L
       * @param k
       */
      void Set ( Matrix &A,vector<int>& L,kersigmoid &k );

      /**
       *
       * @return
       */
      int ComputeSpace ();
      /**
       *
       * @return
       */
      int GetKFACompound();
      /**
       *
       * @return
       */
      int GetGDACompound();
      /**
       *
       * @param Datain
       * @return
       */
      ReturnMatrix Projection ( GeneralMatrix &Datain );
      /**
       *
       * @param filename
       */
      void SaveSpace ( char * filename );

      /**
       *
       * @param filename
       * @param params
       */
      void SaveSpace ( char * filename ,GaborParams &params );
      /**
       *
       * @param filename
       */
      void LoadSpace ( const char * filename );
      /**
       *
       * @param w
       */
      void SetProblem ( string w );
      /**
       *
       * @param w
       */
      void SetProblem ( NProblem w );
      /**
       *
       * @return
       */
      NProblem GetProblem ( );
      /**
       *
       * @return
       */
      string GetProblemName ( );
      /**
       *
       * @param v
       */
      void SetVerbose ( verbose & v);
      /**
       * 
       * @param line 
       */
      void SetkernelPartFromline ( string line );
    private:
      bool feedback;
      int SpaceDim;
      float eps;
      NProblem which;
      verbose verb;
      string methodname;
      void SetKernel ( kernel &k );
      string GetSpacePart ( string line, int & size1, int & size2 );

      void GetkernelPart ( string line );
  };
}
#endif
