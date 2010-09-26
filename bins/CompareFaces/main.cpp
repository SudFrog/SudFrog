/***************************************************************************
 *   Copyright (C) 2009 by Télécom SudParis. Major Developer: Anouar Mellakh   *
 *   reference-system@it-sudparis.eu                                       *
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
 * \page CompareFaces CompareFaces
 *
 * \section CompareFaces-H1 How I'am??
 *   I'am a module that compute the score verification between 2 geometrically normalized faces. The verification is based on the param input file
 *
 *    I output the score between the two images.
 * \section CompareFaces-Usage Usage:
 *   CompareList param InputImage1 InputImage2
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param InputImage1
 *   The first geometrically normalized face image (pgm format)
 * @param InputImage2
 *   The second geometrically normalized face image (pgm format)
 *
 * @return 0 when succeed

  * Here is an example of normalized images (use \link NormFaces NormFaces \endlink to get this kind of images )
   *  \image html norm1.jpg
   *  \image latex norm1.eps "geometrically Normalized" width=3cm

 */
int main ( int argc, char **argv )
{

	if ( argc < 4 )
	{
		cout << "Usage :\n\t" <<  applicationname ( argv[0] ) << " param InputImage1 InputImage2" << endl;
		cout << "\t\t\t \"param\": Parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t\t\t \"InputImage1\": The first geometrically normalized face image (pgm format)" << endl;
		cout << "\t\t\t \"InputImage2\": The second geometrically normalized face image (pgm format)" << endl << endl;
		exit ( -1 );
	}

	inputs Recinputs;
	verbose V ( 1 );
	Recinputs.loadfromxml ( argv[1] );

	Mask NormMask ( Recinputs.norm_eye_dist );
	Pfc_Image face1,face2, *Lightenhanced1,*Lightenhanced2;
	face1.ReadPgm ( argv[2] );
	face2.ReadPgm ( argv[3] );
	Lightenhanced1=face1.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
	Lightenhanced2=face2.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );

	Templator templ;
	templ.SetVerbose ( V );
	templ.SetMask ( Recinputs.norm_eye_dist );

	V<< "Loading " << Recinputs.reduction_space_file.c_str() << "\n";
	templ.SetSpacefile ( Recinputs.reduction_space_file.c_str() );


	vector<double> vect1=templ.Template ( Lightenhanced1[0] );
	vector<double> vect2=templ.Template ( Lightenhanced2[0] );

	Comparator Comp;

	Comp.Set_Distance ( Recinputs.sim_measure );
	V << applicationname ( argv[2] ) << " vs " <<  applicationname ( argv[3] ) << "\t" << Comp.GetTemplatesDistance ( vect1,vect2 ) << "\n";

	return 0;
}
