/***************************************************************************
 *   Copyright (C) 2009 by Télécom SudParis. Major Developer: Anouar Mellakh   *
 *   reference-system@it-sudparis.eu                     *
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
 * \page CompareList CompareList
 *
 * \section H1 How I'am??
 *    I'am the module that allows the comparison of list of tests contained in a text file.

 *    Each comparison test is composed of two geometrically normalized face images. 

 *    I output the scores of each test to the terminal and if the output score file is given I'am able to write into it.
 * \section Usage Usage: 
 *   CompareList param ListofTest Imagesindir [outputscores]
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param ListofTest
 *   A text file containing the list of comparisons (see \link Testlist Testlist.lst \endlink)
 *  	\li The reference and test images must be in pgm format 
 * @param Imagesindir
 *   Path to the normalized images (only geometrically) 
 * @param outputscores
 *   [Optional] if set the scores will be directed to this file 
 *
 * @return 0 when succeed
 */

int main ( int argc, char **argv )
{
	inputs Recinputs;
	verbose V ( 0 );
	vector < string> ListRef,ListTest;
	//Recinputs.SetVerbose ( V );
	int numberoftests,i;
	Templator templ;
	Comparator Comp;
	Pfc_Image face1,face2, *Lightenhanced1,*Lightenhanced2;

	if ( argc < 4 || argc > 5 )
	{
		cout << "Usage :" << endl << applicationname ( argv[0] ) << " param ListofTest Imagesindir [outputscores]" << endl;
		cout << "\t param: parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t ListofTest: a text file containing the list of comparisons (see examples/listoftest.xml)" << endl;
		cout << "\t\t The reference and test images must be in pgm format" << endl;
		cout << "\t Imagesindir: path to the normalized images (only geometrically) " << endl ;
		cout << "\t outputscores: Optional if set the score will be directed to this file" << endl<< endl;
		exit ( -1 );
	}

	string ImageDir = argv[3];//Recinputs.learning_dir;
	Recinputs.loadfromxml ( argv[1] );

	Comp.Set_Distance ( Recinputs.sim_measure );
	ofstream outscores;

	Mask NormMask ( Recinputs.norm_eye_dist );
	templ.SetVerbose ( V );
	templ.SetMask ( Recinputs.norm_eye_dist );

	V << "Parsing the list of tests\n";
	GetComparisonList ( argv[2],ListRef,ListTest );
	numberoftests = ( int ) ListRef.size();

	V << numberoftests << " Tests found\n";

	for ( i = 0; i < numberoftests; ++i )
	{
		ListRef[i]=ChangeFileExt ( ( string ) ( ImageDir + "/" + ListRef[i] ),"pgm" );
		ListTest[i]=ChangeFileExt ( ( string ) ( ImageDir + "/" + ListTest[i] ),"pgm" );
	}
	int count=0;
	V << "\nVerifying used images\n";
	for ( i = 0; i < numberoftests; ++i )           /////// loop for all Images found in the test file
	{

		V << "\rVerifying " << count << " from " << 2*numberoftests;
		if ( !DoesFileExists ( ListRef[i].c_str() ) )
		{
			cerr << "\n" << ListRef[i] << " Not found" << endl;
			exit ( -1 );
		}
		count++;
		V << "\rVerifying " << count << " from " << 2*numberoftests;
		if ( !DoesFileExists ( ListTest[i].c_str() ) )
		{
			cerr << "\n" << ListTest[i]  << " Not found" << endl;
			exit ( -1 );
		}
		count++;
	}

	V<< "\nLoading reduction space: " << Recinputs.reduction_space_file.c_str() << "\n";
	templ.SetSpacefile ( Recinputs.reduction_space_file.c_str() );

	if ( argc==5 )
	{
		outscores.open ( argv[4] );
	}

	for ( i=0;i<numberoftests;i++ )
	{
		face1.ReadPgm ( ListRef[i].c_str() );
		face2.ReadPgm ( ListTest[i].c_str() );
		Lightenhanced1=face1.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
		Lightenhanced2=face2.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );

		vector<double> vect1=templ.Template ( Lightenhanced1[0] );
		vector<double> vect2=templ.Template ( Lightenhanced2[0] );

		V << applicationname ( ListRef[i].c_str() ) << " vs " <<  applicationname ( ListTest[i].c_str() ) << "\t" << 
		Comp.GetTemplatesDistance ( vect1,vect2 ) << "\n";
		if ( argc==5 )
			outscores << ListRef[i]  << "\t" <<   ListTest[i]  << "\t" << Comp.GetTemplatesDistance ( vect1,vect2 ) << "\n";

//free memory
		vect1.clear();
		vect2.clear();
		delete []Lightenhanced1;
		delete []Lightenhanced2;
	}

	if ( argc==5 )
		outscores.close();
	V.Closelog();

	return 0;
}
