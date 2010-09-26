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
#include "comparator.h"

namespace TMSP_Face_Space
{

	Comparator::Comparator()
	{
		distance=C_L1;
	}


	Comparator::~Comparator()
	{
	}

	Comparator::Comparator ( vector <double> &templatesrc,
	                         vector <double> &templatedis )
	{
		template1=templatesrc;
		template2=templatedis;
		distance=C_L1;
	}

	Comparator::Comparator ( vector <double> &templatesrc,
	                         vector <double> &templatedis,
	                         DISTANCE dist )
	{
		template1=templatesrc;
		template2=templatedis;
		distance=dist;
	}

	DISTANCE Comparator::Get_Distance()
	{
		return distance;
	}

	void Comparator::Set_Distance ( DISTANCE dist )
	{
		distance=dist;
	}

	void Comparator::Set_Distance ( string dist )
	{

		strupr ( ( char * ) dist.c_str() );
		if ( dist=="L1" )
			distance=C_L1;
		else if ( dist=="L2" )
			distance=C_L2;
		else if ( dist=="COS" )
			distance=C_Angle;
		else if ( dist=="NORMDIST" )
			distance=C_NormDist;
		else
		{
			cerr << "Unkown distance mesure\nKnown distances L1,L2,cos,NormDist "<< endl;
			exit ( -1 );
		}

	}

	char Comparator::GetMeasuretype ( )
	{
		switch ( distance )
		{
			case C_L1:
				return 'D';

			case C_L2:

				return 'D';

			case C_Angle:
				return 'D';

			case C_NormDist:

				return 'D';
		}

	}

	double Comparator::GetTemplatesDistance()
	{
		return Distance();
	}



	double Comparator::GetTemplatesDistance ( vector <double> &templatesrc,
	        vector <double> &templatedis,
	        DISTANCE dist )
	{
		template1.clear();
		template2.clear();
		template1=templatesrc;
		template2=templatedis;
		distance=dist;
		return Distance();
	}

	double  Comparator::GetTemplatesDistance ( vector <double> &templatesrc,
	        vector <double> &templatedis )
	{
		template1.clear();
		template2.clear();
		template1=templatesrc;
		template2=templatedis;
		return Distance();
	}

	double  Comparator::Distance()
	{
		double val=NAN;
		vector <double> T1,T2;
		switch ( distance )
		{
			case C_L1:
				T1=template1-template2;
				val=sum_absolute_value ( &T1 );
				return val;

			case C_L2:
				T1=template1-template2;
				val=norm_Frobenius ( &T1 );
				return val;

			case C_Angle:
				val=dotproduct ( &template1,&template2 ) / ( sqrt ( sum_square ( &template1 ) *sum_square ( &template2 ) ) );
				return -val;

			case C_NormDist:
				T1=template1/norm_Frobenius ( &template1 );
				T2=template2/norm_Frobenius ( &template2 );
				T1=T1-T2;
				val=norm_Frobenius ( &T1 );
				return val;

			default:
				cout << "Not supported distance" << endl;
				exit ( -1 );
				break;
		}
	};
}
