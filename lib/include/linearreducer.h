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
#ifndef LINEARREDUCER_H
#define LINEARREDUCER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <pthread.h>
#include "Matrix_util.h"
#include "gaborate.h"
#include "verbose.h"


/**
	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/
using namespace std;



namespace TMSP_Face_Space
{

	/** \brief the supported linear problems  */
	enum LProblem
	{
		PCA=0,/**<Principal component Analysis */
		LDA,/**<Linear Disciminant  Analysis */
		DLDA /**<Direct Linear Disciminant  Analysis  */
	};

	/**
	 * \brief Class used to reduce the input space by linear methods (PCA,LDA,DLDA) 
	 */
	class LinearReducer
	{
		public:
			ColumnVector MeanFace /** the mean face */
			, EigenValues /** eigenvalues */;
			Matrix EigenVectors /** Eigen vectors (Eigen faces)*/;

			/**
			 * Constructor
			 */
			LinearReducer();
			/**
			 * 
			 * @param A 
			 */
			LinearReducer ( Matrix & A );
			/**
			 * 
			 * @param A 
			 * @param w 
			 */
			LinearReducer ( Matrix & A,LProblem w );
			/**
			 * 
			 * @param A 
			 * @param lab 
			 */
			LinearReducer ( Matrix & A , vector < int > lab );
			/**
			 * 
			 * @param w 
			 */
			LinearReducer ( LProblem w );
			/**
			 * 
			 */
			~LinearReducer();

			/**
			 * 
			 */
			void free();
			/**
			 * 
			 */
			void Init();
			/**
			 * 
			 * @param A 
			 */
			void SetData ( Matrix & A );
			/**
			 * 
			 * @param w 
			 */
			void SetProblem( LProblem w );
			/**
			 * 
			 * @param w 
			 */
			void SetProblem( string w );
			/**
			 * 
			 * @param labs 
			 */
			void SetLabels(vector <int> labs);
			/**
			 * 
			 * @return 
			 */
			LProblem GetProblem ( );
			/**
			 * 
			 * @return 
			 */
			string GetProblemName ( );
			/**
			 * 
			 * @return 
			 */
			bool GetPCACompound();
			/**
			 * 
			 * @return 
			 */
			bool GetLDACompound();
			/**
			 * 
			 * @return 
			 */
			bool GetDLDACompound();
			/**
			 * 
			 */
			void GetNonZeroEigVal();
			/**
			 * 
			 * @param NVal 
			 */
			void GetNonZeroEigVal ( int NVal );
			/**
			 * 
			 * @param NVect 
			 */
			void GetNonZeroEigVect ( int NVect );
			/**
			 * 
			 * @param CM 
			 * @param D 
			 * @return 
			 */
			int ComputeEigens ( GeneralMatrix & CM, DiagonalMatrix & D );

			/**
			 * 
			 * @return 
			 */
			int ComputeSpace ();
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
		        void SaveSpace ( char * filename,GaborParams &params );
			/**
			 * 
			 * @param filename 
			 */
			void LoadSpace ( const char * filename );

			/**
			 * 
			 * @param face 
			 * @return 
			 */
			ReturnMatrix Projection ( ColumnVector & face );

			/**
			 * 
			 * @param face1 
			 * @param face2 
			 * @param Method 
			 * @return 
			 */
			double Distance ( ColumnVector & face1,ColumnVector & face2,string Method );
			/**
			 * 
			 * @param face1 
			 * @param face2 
			 * @param Method 
			 * @param wheigts 
			 * @return 
			 */
			double Distance ( ColumnVector & face1,ColumnVector & face2,string Method,ColumnVector &wheigts );
			/**
			 * 
			 * @param size 
			 */
			void SetSpaceUsedSize ( long size );
			/**
			 * 
			 * @return 
			 */
			long GetSpaceSize();
			/**
			 * 
			 * @return 
			 */
			ReturnMatrix GetEigensVariance();
			/**
			 * 
			 * @return 
			 */
			int GetVarianceCount();
 
			/**
			 * 
			 * @param line 
			 */
			void Setmaxvariance ( string line );
			/**
			 * 
			 * @param var 
			 */
			void Setmaxvariance ( float var );
			/**
			 * 
			 * @return 
			 */
			float Getmaxvariance();
			/**
			 * 
			 */
			void ReduceToVariance();
			/**
			 * 
			 * @param Comp 
			 * @return 
			 */
			ReturnMatrix Reconstruction ( ColumnVector &Comp );
			/**
			 * 
			 * @param  mx
			 */
			void SetVerbose ( verbose & mx );

		private:			
			verbose verb;
			LProblem which;
			string methodname;
			Matrix Data, CentredData;
			long SpaceSize,SpaceUsedSize;
			float MaxVariance;

			string GetSpacePart ( string line, int & size1, int & size2 );
			bool CenterData();
			vector <int> labels; // for LDA or DLDA
			ReturnMatrix NormalizeColumns ( Matrix & A );
			void NormalizeMat ( Matrix & A );
			ReturnMatrix GetMeanData ( Matrix &A );
			ReturnMatrix GetMeanData ( Matrix &A, int start,int end );
			bool GetCovEigenCompound ( Matrix &Mat,DiagonalMatrix &EigenV,Matrix &EigenVectors );



	};
}
#endif
