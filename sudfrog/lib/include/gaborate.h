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

/**
	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/
#ifndef PFC_SPACEGABORATE_H
#define PFC_SPACEGABORATE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>
#include <stdint.h>
#include <fftw3.h>

#include "miscellaneous.h"
#include "TMSP_image.h"
#include "verbose.h"
#include <pthread.h>

using namespace std;


#ifndef GaborParameters
#define GaborParameters

/**
 * \brief Structure that stores the gabor filters parameters and the returned complexe part (real, imaginary, magnitude, phase).
*/

struct GaborParams
{
	int height /**The height of the gabor filters*/,
	width /**The width of the gabor filters*/,
	nscale/**The number of scales of the gabor filters**/,
	norient /**The number of orientation of the gabor filters */,
	minWaveLength /**The lentgh of the first scale gabor wavelet */,
	reductcoef /**the coefficient sampling of the gabor features (note that for a reductcoef="2" just 1 gabor component on 2 is kept)*/;

	float mult /**To go from a scale wavelet to the next one the lentgh is multiplied by mult */,
	sigmaOnf /**Ratio of the standard deviation of the Gaussian describing the log Gabor filter's
	transfer function in the frequency domain to the filter center frequency. */,
	dThetaOnSigma /**Ratio of angular interval between filter orientations and the standard deviation
	of the angular Gaussian function used to construct filters in the freq. plane */;

	string methodname /** Complexe part to be computed:
			 *    \li  "REAL": real part of complex features
			 *    \li "IMAG": imaginary part
			 *    \li "MAGN": magnitude part
			 *    \li "PHASE": angle part
			 *    \li a combination example methodname="REAL, IMAG, MAGN" */;
};
#endif

namespace TMSP_Face_Space
{

	/**
	*  \brief Class that computes the gabor filtering of an input image.
	*
	*	  It creates the log-gabor filters in frequency space based on the work of Peter Kovesi
	*	      http://www.csse.uwa.edu.au/~pk/research/matlabfns/PhaseCongruency/Docs/convexpl.html
	*
	*/

	class Gaborate
	{

		public:

			/**
			  * \brief Structure in which are stored the gabor filters parameters.
			*/
			GaborParams parameters;
			/**
			 * Constructor
			 */
			Gaborate();

			/**
			 * Constructor
			 * @param  Gabparam Structure in which are stored the gabor filters parameters.
			 */
			Gaborate ( GaborParams Gabparam );
			/**
			 *
			 * @param  Gabparam Structure in which are stored the gabor filters parameters
			 * @param  problem  Complexe part to be computed:
			 *    \li  "REAL": real part of complex features
			 *    \li "IMAG": imaginary part
			 *    \li "MAGN": magnitude part
			 *    \li "PHASE": angle part
			 */
			Gaborate ( GaborParams Gabparam ,const char * problem );
			/**
			 * Destructor
			 */
			~Gaborate();

			/**
			 *
			 * @param P complexe part to be computed
			 */
			void SetProblem ( string P );
			/**
			 *
			 * @param v copy the verbosing v to the member verb of *this;
			 */
			void SetVerbose ( verbose & v );
			/**
			 *
			 * @param redcoef the coefficient sampling of the gabor features (note that for a reductcoef="2" just 1 gabor component on 2 is kept)
			*
			*	a normalisation of the gabor features is done before the sampling to get mean=0 and std=1 for the real and imaginary parts.
			 * @return 0 if well done
			 */
			int SetReduction ( int redcoef );

			/**
			 *
			 * @param height The height of the gabor filters
			 * @param width The width of the gabor filters
			 * @param nscale The number of scales of the gabor filters
			 * @param norient The number of orientation of the gabor filters
			 * @param minWaveLength The lentgh of the first scale gabor wavelet
			 * @param mult To go from a scale wavelet to the next one the lentgh is multiplied by mult
			 * @param sigmaOnf Ratio of the standard deviation of the Gaussian describing the log Gabor filter's
						transfer function in the frequency domain to the filter center frequency.
			 * @param dThetaOnSigma Ratio of angular interval between filter orientations and the standard deviation
						of the angular Gaussian function used to construct filters in the freq. plane
			 */
			void SetGaborParams ( int height, int width,int nscale,int norient,int minWaveLength, float mult,float  sigmaOnf,float dThetaOnSigma );
			/**
			 *
			 * @param param structure of gabor filters parameters to be copied to GaborParams structure of this class.
			 */
			void SetGaborParams ( GaborParams param );
			/**
			 *
			 * @param param structure of gabor filters parameters to be copied to GaborParams structure of this class  and create the gabor filters in frequency domain.
			 */
			void CreateFFTGaborFilters ( GaborParams param );
			/**
			 * Create the gabor filters in frequency domain.
			 */
			void CreateFFTGaborFilters ();

			/**
			 *
			 * @param data Pointer to the data of the gray-scale face image
			 * @param rows height of the image
			 * @param cols width of the image
			 */
			void SetFace ( uint8_t *data,int rows,int cols );
			/**
			 *
			 * @param face the Pfc_Image is used to be gaborated.
			 */
			void SetFace ( Pfc_Image & face );

			/**
			 *
			 * @param data Pointer to the data of the gray-scale face image
			 * @param rows height of the image
			 * @param cols width of the image
			 * @return a vector containing of the gabor components of data based on the chosen gabor part
			 */
			vector <double>  Gaborating ( uint8_t *data,int rows,int cols );
			/**
			 *
			 * @param face Pfc_Image class used to be gaborated.
			 * @return a vector containing of the gabor components of data based on the chosen gabor part
			 */
			vector <double>  Gaborating ( Pfc_Image & face );
			/**
			 *
			 * @return a vector containing of the gabor components of the face based on the chosen gabor parts
			 */
			vector <double> Gaborating();
			/**
			 *
			 * @return the size of the vector Features to be computed based on the given GaborParams
			 */
			int GetFeaturesize();
			/**
			 *
			 * @return return a string containing the used gabor parts
			 */
			string GetMethod();
			/**
			 *
			 * @return return the vector of computed features
			 */
			vector <double> GetGaborFeatures();

			/**
			 *
			 * @param eyedist create a mask to be applied to the gabor filtrerd response
			 */
			void SetMask ( int eyedist );
			/**
			 * remove the mask
			 */
			void UnSetMask ( );

			/**
			 *
			 * @param filename file name to output the computed features
			 * @return 0 if well done
			 */
			int  SaveFeatures ( const char* filename ) ;
			/**
			 *
			 * @param filename name to output the log-gabor filters in Frequency domain.
			 * @return 0 if well done
			 */
			int  SaveFilters ( const char* filename );
			/**
			 *
			 * @param mux set mutex to mux to avoid fftw3 problem when using multi-threading.
			 */
			void SetMutex ( pthread_mutex_t* mux ); //problem with planer of fftw

		private:

			pthread_mutex_t *mutexout;
			verbose verb;
			vector <double> Features;
			Mask GabMask;
			bool useMag,usePhase,useReal,useImag,applymask,FirstFace;
			fftw_complex *Original,*FFTFace,*IFFTFace,*Reconst;
			fftw_plan plan_forward,plan_backward;
			vector < vector < double * > > Filters;
			void init();
			//  bool GetGaborsize ( string line, int & size1, int & size2 ,int &sizeheight,int &sizewidth );

	};
};


#endif
