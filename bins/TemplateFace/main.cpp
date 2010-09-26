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
 * \page TemplateFace TemplateFace
 *
 * \section H1 How I'am??
 *    I'am the module that allows the extraction of template from a geometrically normalized face
 * 
 * \section Usage Usage: 
 *   TemplateFace param InputImage OutputTemplate
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param InputImage
 *   The geometrically normalized face image (pgm format) 
 * @param OutputTemplate
 *   The output filename template
 * @return 0 when succeed
 *
 * Here is an example of normalized face image (use \link NormFaces NormFaces \endlink to get this kind of images )
 *  \image html norm1.jpg
 *  \image latex norm1.eps "geometrically Normalized face" width=3cm
 */
int main ( int argc, char **argv )
{

	if ( argc < 4 )
	{
		cout << "Usage :\n\t" <<  applicationname ( argv[0] ) << " param InputImage OutputTemplate" << endl;
		cout << "\t\t\t \"param\": Parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t\t\t \"InputImage\": The geometrically normalized face image (pgm format)" << endl;
		cout << "\t\t\t \"OutputTemplate\": The Output template file" << endl << endl;
		exit ( -1 );
	}
	inputs Recinputs;
	Recinputs.loadfromxml ( argv[1] );
	verbose V ( 1 );
	Mask NormMask ( Recinputs.norm_eye_dist );
	Pfc_Image face, *Lightenhanced;
	face.ReadPgm ( argv[2] );
	Lightenhanced=face.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );

	Templator templ;
	templ.SetVerbose ( V );
	templ.SetMask ( Recinputs.norm_eye_dist );
	V << "Loading Space Reduction from " <<  Recinputs.reduction_space_file << "\n";

	templ.SetSpacefile ( Recinputs.reduction_space_file.c_str() );
	V << "Computing template\n";
	templ.Template ( Lightenhanced[0] );
	CreateDirForFile ( argv[3],false );
	templ.SaveTemplate ( argv[3] );
	V << "Template saved\n";

	return 0;
}
