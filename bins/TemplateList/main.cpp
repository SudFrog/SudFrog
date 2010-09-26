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

Templator *Pfc_temp;
struct thread_data *thread_data_array;
pthread_mutex_t mutexout;
inputs Recinputs;

vector <string> ListofQuery, ListofCode;
string ImageDir,Outdir;
bool masking,*queryrun;

int countquery = 0;

vector <point> intrat, intert;


verbose V ( 0 );
int icount=1;
int endimageQuery;

void* querytemplates ( void *threadpass )
{
	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;

	Pfc_Image *Lightenhanced;
	Mask NormMask ( Recinputs.norm_eye_dist );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )           /////// loop for all Images found in the Xml file
	{
		pthread_mutex_lock ( &mutexout );
		icount++;
		cout  << "\rTemplating query "<< ( int ) 100.0*icount/endimageQuery << "%";
		pthread_mutex_unlock ( &mutexout );

		if ( masking && !queryrun[i] )
			continue;
		else
		{
			string imagefilename = ImageDir + "/" + ListofQuery[i]  ;
			string codefilename = Outdir + "/" + ListofCode[i];
			if ( DoesFileExists ( imagefilename.c_str() ) )
			{
				Pfc_Image InputImage;
				InputImage.ReadImage ( imagefilename.c_str() );
				Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
				Pfc_temp[my_data->thread_id].Template ( Lightenhanced[0] );
				pthread_mutex_lock ( &mutexout );
				CreateDirForFile ( codefilename.c_str() ,false );
				pthread_mutex_unlock ( &mutexout );
				Pfc_temp[my_data->thread_id].SaveTemplate ( codefilename.c_str() );
			}
			else
			{
				V << ListofQuery[i]  << " Not found\n";
				pthread_exit ( ( void* ) -1 );
			}
		}

	}
	return ( void* ) 0;
}




/**
 * \page TemplateList TemplateList
 *
 * \section H1 How I'am??
 *    I'am the module that allows the extraction of templates from a list if geometrically normalized faces
 * 
 *    I use the multi-threading to accelerate your work!
 *
 * \section Usage Usage: 
 *   TemplateList param xmllist Imagesindir Templateoutdir
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param xmllist
 *   xml file containing the list of files with there metadata (see \link Metadata Metadata.xml \endlink or \link Learning Learning.xml \endlink as examples)
 * @param Imagesindir
 *   path to the normalized images
 * @param Templateoutdir
 *   path to the output templates
 * @return 0 when succeed


 */
int main ( int argc, char **argv )
{

	int startimage = 0, i, j, step = 0,rc;
	vector <double> NullVect;
	V.SetMutex ( &mutexout );
	Recinputs.SetVerbose ( V );

	if ( argc < 5 )
	{
		cout << "Usage :" << endl << applicationname ( argv[0] ) << " param xmllist Imagesindir Templateoutdir" << endl;
		cout << "\t param: parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t xmllist: xml file containing the list of files with there metadata (see examples/Learning.xml)" << endl;
		cout << "\t Imagesindir: path to the normalized images (only geometrically) " << endl;
		cout << "\t Templateoutdir: path to the output templates codes" << endl << endl;
		exit ( -1 );
	}

	Recinputs.loadfromxml ( argv[1] );
	ImageDir = argv[3];//Recinputs.learning_dir;
	Outdir=argv[4];

	int nthreads=Recinputs.nthreads;
	pthread_t threads[nthreads];
	Pfc_temp=new Templator[nthreads];
	thread_data_array=new thread_data[nthreads];

	V << "Parsing Query Images from: \n\t" << ( const char * ) argv[2] << "\n";

	ListofQuery = loadnamefromXml ( argv[2] );
	endimageQuery = ( int ) ListofQuery.size();

	for ( i = startimage; i < endimageQuery; ++i )
	{
		ListofQuery[i]=ChangeFileExt ( ListofQuery[i],"pgm" );
		ListofCode.push_back ( ChangeFileExt ( ListofQuery[i],"cod" ) );
	}

	for ( i = startimage; i < endimageQuery; ++i )           /////// loop for all Images found in the Xml file
	{
		string imagefilename = ImageDir + "/" + ListofQuery[i]  ;
		V << "\rVerifying " << i + 1 << " from " << endimageQuery;
		if ( !DoesFileExists ( imagefilename.c_str() ) )
		{
			cerr << "\n" << ListofQuery[i]  << " Not found" << endl;
			exit ( -1 );
		}
	}

	V << " Ok\n";

	startimage = 0;

	V << "Creating templator for " << nthreads << " threads\n";
	for ( i=0;i<nthreads;i++ )
	{

		Pfc_temp[i].SetVerbose ( V );
		Pfc_temp[i].SetMutex ( &mutexout );
		Pfc_temp[i].SetMask ( Recinputs.norm_eye_dist );
		Pfc_temp[i].SetSpacefile ( Recinputs.reduction_space_file.c_str() );
	}
	V << "Templators created\n" ;

	Timer s;
	s.start();
	void *status;
	step= ( endimageQuery-startimage ) /nthreads;

	for ( i=0;i<nthreads;i++ )
	{
		thread_data_array[i].thread_id=i;
		thread_data_array[i].startimage=i*step;
		thread_data_array[i].endimage= ( i+1 ) *step ;
		rc = pthread_create ( &threads[i], NULL, querytemplates, ( void * ) &thread_data_array[i] );
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


	int rest= ( int ) fmod ( ( double ) endimageQuery-startimage,nthreads );
	if ( rest>0 )
	{
		thread_data_array[i-1].thread_id=i-1;
		thread_data_array[i-1].startimage=i*step;
		thread_data_array[i-1].endimage= i*step +rest;
		querytemplates ( ( void * ) &thread_data_array[i-1] );
	}

	delete []Pfc_temp;
	delete []thread_data_array;

	V.Closelog();
	return 0;
}
