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
	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/

#ifndef PFC_SPACECOMPARATOR_H
#define PFC_SPACECOMPARATOR_H
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <math.h>
#include <vectorextension.h>
#include "miscellaneous.h"
using namespace std;


namespace TMSP_Face_Space
{

	/** \brief Supported measure distances between templates */
	enum DISTANCE
	{
		C_L1=1 /**< sum_absolute_value(T1-T2) */,
		C_L2 /**<norm_Frobenius(T1-T2) */,
		C_Angle/**< minus cosinus between the 2 templates vectors 
				[-dotproduct ( T1,T2) / ( sqrt ( sum_square ( T1 ) *sum_square ( T2 ) ) )]*/,
		C_NormDist/**<Norm_Frobenius (T1/Norm_Frobenius(T1) - T2/Norm_Frobenius(T2)); */
	};

	/**
	*  \brief Class that compares two given templates based on the chosen Distance
	*/
	class Comparator
	{
		public:
			/**
			 * Constrcutor
			 */
			Comparator();
			/**
			 * Destructor
			 */
			~Comparator();
			/**
			 * initialize the comparator with the two templates
			 * @param template1 a vector of double (Reference)
			 * @param template2 a vector of double (test) 
			 */
			Comparator ( vector <double> &template1,vector <double> &template2 );
			/**
			 * initialize the comparator with the two templates and the distance
			 * @param template1 a vector of double (Reference)
			 * @param template2 a vector of double (test) 
			 * @param dist Set the distance (C_L1, C_L2, C_Angle, C_NormDist)
			 */
			Comparator ( vector <double> &template1,vector <double> &template2,
			             DISTANCE dist );
			/**
			 *
			 * @return the distance chosen
			 */
			DISTANCE Get_Distance();
			/**
			 *
			 * @param dist Set the chosen distance (C_L1, C_L2, C_Angle, C_NormDist)
			 */
			void Set_Distance ( DISTANCE dist );

			/**
			 *
			 * @param dist Set the chosen distance (L1, L2, COS, NORMDIST)
			 */
			void Set_Distance ( string dist );

			/**
			 *
			 * @return 'S' for similarity  or 'D' for distance
			 */
			char GetMeasuretype ( );
			/**
			 *
			 * @return the score similarity or distance between the templates
			 */
			double GetTemplatesDistance();
			/**
			 *
		         * @param template1 a vector of double (Reference)
			 * @param template2 a vector of double (test) 
			 * @param dist
			 * @return the score similarity or distance between the templates
			 */
			double GetTemplatesDistance ( vector <double> &template1,
			                              vector <double> &template2,
			                              DISTANCE dist );
			/**
			 *
			 * @param template1 a vector of double (Reference)
			 * @param template2 a vector of double (test) 
			 * @return the score similarity or distance between the templates
			 */
			double  GetTemplatesDistance ( vector <double> &template1,
			                               vector <double> &template2 );

		private:
			vector <double> template1,template2;
			DISTANCE distance;
			double  Distance();

	};

}

#endif
