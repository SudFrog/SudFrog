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
pthread_mutex_t mutexout;
vector < XmlFace >  ListofImages;
int endimagelist,icount=0;
string ImageDir,ImageOutdir;

inputs Recinputs;
verbose V ( 0 );
struct thread_data *thread_data_array;


void* normlist ( void *threadpass )
{

	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;
	Mask NormMask ( Recinputs.norm_eye_dist );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )           /////// loop for all Images found in the Xml file
	{
		string imagefilename = ImageDir + "/" + ListofImages[i].filename;
		pthread_mutex_lock ( &mutexout );
		icount++;
		cout  << "\rNormalizing  "<< ( int ) 100.0*icount/endimagelist<< "%";
		pthread_mutex_unlock ( &mutexout );

		if ( DoesFileExists ( imagefilename.c_str() ) )
		{

			string outname=ImageOutdir+"/"+ListofImages[i].filename;
			outname=ChangeFileExt ( outname,"pgm" );
			PFCface Face;
			Face.LoadOriginal ( imagefilename.c_str() );
			Face.SetEyeDistance ( Recinputs.norm_eye_dist );
			Face.SetFaceCoordinate ( ( int ) ListofImages[i].metadata[2], ( int ) ListofImages[i].metadata[3],
			                         ( int ) ListofImages[i].metadata[0], ( int ) ListofImages[i].metadata[1],
			                         ( int ) ListofImages[i].metadata[4], ( int ) ListofImages[i].metadata[5],
			                         ( int ) ListofImages[i].metadata[6], ( int ) ListofImages[i].metadata[7] );

			Face.NormGeometric();
			Face.GeomNorm.Stretch ( 2,NormMask );

			pthread_mutex_lock ( &mutexout );
			CreateDirForFile ( outname,false );
			pthread_mutex_unlock ( &mutexout );
			Face.GeomNorm.SaveImage ( ( char * ) outname.c_str() );

		}
		else
		{
			V << ListofImages[i].filename  << " Not found\n";
			pthread_exit ( ( void* ) -1 );
		}

	}

	return ( void* ) 0;
}


/**
 * 
 * \page NormFaces NormFaces
 *
 * \section H1 How I'am??
 *    I'am the module that allows the geometric extraction and normalization of a list of faces and store them in a gray-scale pgm file
 * 
 *    I use the multi-threading to accelerate your work!
 *
 * \section Usage Usage: 
 *   NormFaces param xmllist imageindir imageoutdir
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param xmllist
 *   xml file containing the list of files with there metadata (see \link Metadata Metadata.xml \endlink or \link Learning Learning.xml \endlink as examples)
 * @param imageindir
 *   path to the original images
 * @param imageoutdir
 *   path to the output normalized images
 * @return 0 when succeed

 * Here is an output normalized images
 *\image html original.jpg 
 *
 * \image html norm1.jpg
 *
 * \image latex original.eps "Original Image" width=5cm
 *
 * \image latex norm1.eps "geometrically Normalized" width=3cm

 */

int main ( int argc, char **argv )
{

	int startimage = 0, i, step = 0,rc;
	V.SetMutex ( &mutexout );
	Recinputs.SetVerbose ( V );

	if ( argc != 5 )
	{
		cout << "Usage :" << endl << applicationname ( argv[0] ) << " param xmllist imageindir imageoutdir " << endl;
		cout << "\t param: parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t xmllist: xml file containing the list of files with there metadata (see examples/Learning.xml)" << endl;
		cout << "\t imageindir: path to the original images" << endl;
		cout << "\t imageoutdir: path to the output normalized images" << endl << endl;
		exit ( -1 );
	}

	Recinputs.loadfromxml ( argv[1] );
	ImageDir =argv[3];
	ImageOutdir =argv[4];

	int nthreads=Recinputs.nthreads;
	pthread_t threads[nthreads];
	thread_data_array=new thread_data[nthreads];


	V << "Parsing Images from: \n\t" << ( const char * ) argv[2] << "\n";

	ListofImages=loadXml ( argv[2] );
	endimagelist=ListofImages.size();

	V << "Start normalizing " << endimagelist << " images with " << nthreads << " threads\n";
	for ( i = startimage; i < endimagelist; ++i )           /////// loop for all Images found in the Xml file
	{
		string imagefilename = ImageDir + "/" + ListofImages[i].filename  ;
		V << "\rVerifying " << i + 1 << " from " << endimagelist;
		if ( !DoesFileExists ( imagefilename.c_str() ) )
		{
			cerr << "\n" << imagefilename << " Not found" << endl;
			exit ( -1 );
		}
	}

	V << "\n";
	Timer s;
	s.start();
	void *status;
	step= ( endimagelist-startimage ) /nthreads;

	for ( i=0;i<nthreads;i++ )
	{
		thread_data_array[i].thread_id=i;
		thread_data_array[i].startimage=i*step;
		thread_data_array[i].endimage= ( i+1 ) *step ;
		rc = pthread_create ( &threads[i], NULL, normlist, ( void * ) &thread_data_array[i] );
	}


	for ( i=0;i<nthreads;i++ )
	{
		rc = pthread_join ( threads[i], &status );
		if ( rc )
		{
			cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
			exit ( -1 );
		}
	}


	int rest= ( int ) fmod ( ( double ) endimagelist-startimage,nthreads );
	if ( rest>0 )
	{
		thread_data_array[i-1].thread_id=i-1;
		thread_data_array[i-1].startimage=i*step;
		thread_data_array[i-1].endimage= i*step +rest;
		normlist ( ( void * ) &thread_data_array[i-1] );
	}

	V << "\nElapsed time "<< s.Get_Elapsed_s() << "\n";
	return 0;

}

