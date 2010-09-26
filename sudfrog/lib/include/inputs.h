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
#ifndef PFC_SPACEINPUTS_H
#define PFC_SPACEINPUTS_H
#include "xpath_processor.h"
#include <string>
#include <vector>
#include <iostream>
#include "verbose.h"

	/**
	*	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
	*/
using namespace std;

namespace TMSP_Face_Space
{

	/**
	* 
	* \brief Class that reads framework parameters from the xml parameters file \link paramfile paramfile.xml \endlink
	*/
	class inputs
	{
		public:
			string params_dir,normlight,reduction_method,
			reduction_param,gabor_method,reduction_space_file,sim_measure;
			int nthreads,verboz,norm_eye_dist,
			Gabscales,Gaborientations,GabminWavelet,Gabreduction,
			controlled,noncontrolled,nusers;
			float GabsigmaOnf, GabdThetaOnSigma,Gabmult;
			double* paramnorm;

			/**
			 * Constructor
			 */
			inputs();
			/**
			 * Constructor
			 * @param file
			 */
			inputs ( const char* file );
			/**
			 * Destructor
			 */
			~inputs();
			/**
			 *
			 * @param file
			 * @return
			 */
			int loadfromxml ( const char* file );
			/**
			 *
			 * @param pFilename
			 * @return
			 */
			vector <string> loadwatchlist ( const char* pFilename );
			/**
			 *
			 * @param v
			 */
			void SetVerbose ( verbose & v);
			/**
			 *
			 */
			void SetVerbose ();
			/**
			 *
			 */
			void SetMute ();

		private:
			verbose verb;
			int parsenormparam ( string param );
	};

}

#endif
