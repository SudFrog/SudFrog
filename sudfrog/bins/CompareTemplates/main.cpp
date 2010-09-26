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

#include <pthread.h>
#include "TMSPFace.h"


using namespace TMSP_Face_Space;
/** 
 * 
 * \page CompareTemplates CompareTemplates
 *
 * \section H1 How I'am??
 *   I'am the module that computes the score verification between 2 constructed templates. I load the two templates and I give you the distance between them.
 *
 * \section Usage Usage: 
 *   CompareTemplates Template1 Template2 Distance
 *
 * @param Template1
 *   The first face template
 * @param Template2
 *   The second face template
 * @param Distance
 *   The distance/similarity measure
 * \li L1: sum_absolute_value(T1-T2)
 * \li L2: norm_Frobenius(T1-T2)
 * \li COS: -dotproduct ( T1,T2) / ( sqrt ( sum_square ( T1 ) *sum_square ( T2 ) ) )
 * \li NORMDIST: Norm_Frobenius (T1/Norm_Frobenius(T1) - T2/Norm_Frobenius(T2))
 * @return 0 when succeed

 */
int main ( int argc, char **argv )
{

	if ( argc < 4 )
	{
		cout << "Usage :" << endl << applicationname ( argv[0] ) << " Template1 Template2 Distance" << endl;
		cout << "\t Template1: the first face template" << endl;
		cout << "\t Template2: the second face template" << endl;
		cout << "\t Distance: The distance/similarity measure" << endl;
		cout<< "\t\t\t- L1: sum_absolute_value(T1-T2);\n\t\t\t- L2: norm_Frobenius(T1-T2);"<< endl;
		cout << "\t\t\t- COS: -dotproduct ( T1,T2) / ( sqrt ( sum_square ( T1 ) *sum_square ( T2 ) ) );\n\t\t\t- NORMDIST: Norm_Frobenius (T1/Norm_Frobenius(T1) - T2/Norm_Frobenius(T2));" << endl << endl;
		exit ( -1 );
	}


	Templator templ1,templ2;
	templ1.SetNoSpace();
	templ2.SetNoSpace();
	vector<double> vect1=templ1.LoadTemplate ( argv[1] );
	vector<double> vect2=templ2.LoadTemplate ( argv[2] );
	Comparator Comp;
	string Distance=argv[3];
	Comp.Set_Distance ( Distance );

	cout << applicationname (argv[1] ) << " vs " <<  applicationname (argv[2] ) << "\t" << Comp.GetTemplatesDistance ( vect1,vect2 ) << endl;

	return 0;
}
