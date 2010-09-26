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

vector <string> ListofQuery, ListofTarget;
string ImageDir;
bool masking,*queryrun, *targetrun;
vector < double > *DATAQuery, *DATATarget;
int countquery = 0,counttarget = 0;

vector <point> intrat, intert;
int intra = 0, inter = 0;

float *SimResult;
bool half = false;

verbose V ( 0 );
int steps = 150,icount=1;
float lambda = 1.0;
int endimageQuery, endimageTarget;

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

			if ( DoesFileExists ( imagefilename.c_str() ) )
			{

				Pfc_Image InputImage;
				InputImage.ReadImage ( imagefilename.c_str() );
				Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
				//Lightenhanced[0].Stretch ( 2, NormMask );

				DATAQuery[i]=Pfc_temp[my_data->thread_id].Template ( Lightenhanced[0] );
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

void* targettemplates ( void *threadpass )
{
	//return tmp;
	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;
	Pfc_Image  *Lightenhanced;
	Mask NormMask ( Recinputs.norm_eye_dist );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )           /////// loop for all Images found in the Xml file
	{

		pthread_mutex_lock ( &mutexout );
		icount++;
		cout  << "\rTemplating target "<< ( int ) 100.0*icount/endimageTarget << "%";
		pthread_mutex_unlock ( &mutexout );

		if ( masking && !targetrun[i] )
			continue;
		else
		{
			string imagefilename = ImageDir + "/" + ListofTarget[i]  ;

			if ( DoesFileExists ( imagefilename.c_str() ) )
			{

				Pfc_Image InputImage;
				InputImage.ReadImage ( imagefilename.c_str() );
				Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
				//Lightenhanced[0].Stretch ( 2, NormMask );

				DATATarget[i]=Pfc_temp[my_data->thread_id].Template ( Lightenhanced[0] );
			}
			else
			{
				V << ListofTarget[i]  << " Not found\n";
				pthread_exit ( ( void* ) -1 );

			}
		}
	}
	return ( void* ) 0;
}


void* similarityintra ( void *threadpass )
{
	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;
	Comparator Comp;
	Comp.Set_Distance ( Recinputs.sim_measure );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )
	{

		if ( !half )
			SimResult[ intrat[i].x*endimageTarget + intrat[i].y ] = Comp.GetTemplatesDistance ( DATAQuery[intrat[i].x], DATATarget[intrat[i].y] );
		else
			SimResult[ intrat[i].x*endimageTarget + intrat[i].y ] = Comp.GetTemplatesDistance ( DATAQuery[intrat[i].x], DATAQuery[intrat[i].y] );

	}
	return ( void* ) 0;
}

void* similarityinter ( void *threadpass )
{
	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;
	Comparator Comp;
	Comp.Set_Distance ( Recinputs.sim_measure );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )
	{

		if ( !half )
			SimResult[ intert[i].x*endimageTarget + intert[i].y ] = Comp.GetTemplatesDistance ( DATAQuery[intert[i].x], DATATarget[intert[i].y] );
		else
			SimResult[ intert[i].x*endimageTarget + intert[i].y ] = Comp.GetTemplatesDistance ( DATAQuery[intert[i].x], DATAQuery[intert[i].y] );

	}
	return ( void* ) 0;
}


void* similarity ( void *threadpass )
{
	struct thread_data *my_data;
	my_data = ( struct thread_data * ) threadpass;
	Comparator Comp;
	Comp.Set_Distance ( C_Angle );
	for ( int i = my_data->startimage; i < my_data->endimage; ++i )
	{
		for ( int j = 0; j < endimageQuery; ++j )
		{
			if ( !half )
				SimResult[ i*endimageTarget + i] = Comp.GetTemplatesDistance ( DATAQuery[i], DATATarget[j] );
			else
				SimResult[ i*endimageTarget + i] = Comp.GetTemplatesDistance ( DATAQuery[i], DATAQuery[j] );

		}
	}
	return ( void* ) 0;
}


/** 
 * \page FRGC_RUN frgc_run
 *
 * \section H1 How I'am??
 *    I'am the module that can run the FRGC-v2 experiments (Never ask the database from my author just go to http://www.frvt.org/FRGC/ and ask). 
 *
 * The author could give you only the xml metadata and the masks. 
 *
 *  I use the multi-threading to accelerate your work!
 * \section Usage Usage: 
 *   frgc_run param queryxml targetxml inputdir maskfile Similaritymatrix
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param queryxml
 *   Xml file containing the list of Query files (Test images) (see \link Metadata Metadata.xml \endlink example of the xml format)
 * @param targetxml
 *   Xml file containing the list of Target files (Reference images) (ame xml format as queryxml)
 * @param inputdir
 *   Path to the input geometrically normalized images (use \link NormFaces NormFaces \endlink for that)
 * @param maskfile
 *   Mask file containing the intra/interclass tests (see MBGC mask format)
 * @param Similaritymatrix
 *   The output file containing the Similarity matrix (see MBGC similarity format)
 * @return 0 when succeed
 */
int main ( int argc, char **argv )
{

	int startimage = 0, i, j, step = 0,rc;
	vector <double> NullVect;
	V.SetMutex ( &mutexout );
	Recinputs.SetVerbose ( V );

	if ( argc < 7 )
	{
		cout << "Usage :" << endl << applicationname ( argv[0] ) << " param queryxml targetxml inputdir maskfile Similaritymatrix" << endl;
		cout << "\t param: parameters xml file (see examples/PFC_param.xml)" << endl;
		cout << "\t queryxml: xml file containing the list of Query files (Test images)" << endl;
		cout << "\t targetxml:xml file containing the list of Target files (Reference images)" << endl;
		cout << "\t inputdir: path to the input geometrically normalized images" << endl;
		cout << "\t mask: a mask file containing the intra/interclass tests (see MBGC mask format)" << endl;
		cout << "\t Similaritymatrix: the file containing the Similarity matrix (see MBGC similaity format)" << endl<< endl;
		exit ( -1 );
	}


	Recinputs.loadfromxml ( argv[1] );
	ImageDir = argv[4];//Recinputs.learning_dir;

//validate distance mesure;
	Comparator Comp;
	Comp.Set_Distance ( Recinputs.sim_measure );

	int nthreads=Recinputs.nthreads;
	pthread_t threads[nthreads];
	Pfc_temp=new Templator[nthreads];
	thread_data_array=new thread_data[nthreads];

	if ( ( string ) argv[2] == ( string ) argv[3] )
		half = true;

	V << "Parsing Query Images from: \n\t" << ( const char * ) argv[2] << "\n";

	ListofQuery = loadnamefromXml ( argv[2] );
	endimageTarget = endimageQuery = ( int ) ListofQuery.size();

	for ( i = startimage; i < endimageQuery; ++i )
		ListofQuery[i]=ChangeFileExt ( ListofQuery[i],"pgm" );

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
	if ( !half )
	{
		V << "Parsing Target Images from: \n\t" << ( const char* ) argv[3] << "\n";
		ListofTarget = loadnamefromXml ( argv[3] );
		startimage = 0;
		endimageTarget = ( int ) ListofTarget.size();

		for ( i = startimage; i < endimageTarget; ++i )
			ListofTarget[i] =ChangeFileExt ( ListofTarget[i] ,"pgm" );

		for ( i = startimage; i < endimageTarget; ++i )           /////// loop for all Images found in the Xml file
		{
			string imagefilename = ImageDir + "/" + ListofTarget[i]  ;
			V << "\rVerifying " << i + 1 << " from " << endimageTarget;
			if ( !DoesFileExists ( imagefilename.c_str() ) )
			{
				cerr << "\n" <<ListofTarget[i]  << " Not found" << endl;
				exit ( -1 );
			}
		}
		V << " Ok\n";
	}

	SimResult = new float[endimageQuery*endimageTarget];
	masking = false;

	if ( argc >= 6 )
	{
		masking = true;
		V << "Loading mask from: \n\t" << ( const char* ) argv[5] << "\n";
		Matrix mask = loadMask ( argv[5] );

		if ( mask.Nrows() != endimageQuery || mask.Ncols() != endimageTarget )
		{

			cerr << "Similarity matrix and mask matrix mismatch in dimension!" << endl;
			mask.ReleaseAndDelete();
			delete []SimResult;
			return ( -1 );
		}
		queryrun = new bool[mask.Nrows() ];
		targetrun = new bool[mask.Ncols() ];

		for ( i = 0;i < mask.Nrows();i++ )
			queryrun[i] = false;

		for ( i = 0;i < mask.Ncols();i++ )
			targetrun[i] = false;

		for ( i = 1;i <= mask.Nrows();i++ )
			for ( j = 1;j <= mask.Ncols();j++ )
			{
				point coord;
				if ( mask ( i, j ) == INTRA )
				{
					coord.x = i - 1;
					coord.y = j - 1;
					intrat.push_back ( coord );
					queryrun[i-1] = true;
					targetrun[j-1] = true;
				}
				else
					if ( mask ( i, j ) == INTER )
					{
						coord.x = i - 1;
						coord.y = j - 1;
						intert.push_back ( coord );
						queryrun[i-1] = true;
						targetrun[j-1] = true;
					}
			}

		intra = ( int ) intrat.size();
		inter = ( int ) intert.size();

		for ( i = 0;i < mask.Nrows();i++ )
			if ( queryrun[i] )
				countquery++;

		V << countquery << " :Query files\n";

		for ( i = 0;i < mask.Ncols();i++ )
			if ( targetrun[i] )
				counttarget++;

		V << counttarget << " :Target files\n";
		mask.ReSize ( 0,0 );
		mask.ReleaseAndDelete();
		V << intra << " : Client tests\n" << inter << " : Impostor tests\n";

	}

	startimage = 0;
	DATAQuery=new vector < double >[endimageQuery];
	DATATarget=new vector < double >[endimageTarget];

	V << "Creating templator for " << nthreads << " threads\n";
	for ( i=0;i<nthreads;i++ )
	{

		Pfc_temp[i].SetVerbose ( V );
		Pfc_temp[i].SetMutex ( &mutexout );
		Pfc_temp[i].SetMask ( Recinputs.norm_eye_dist );
		Pfc_temp[i].SetSpacefile ( Recinputs.reduction_space_file.c_str() );
	}
	V << "Templators created\n" ;
//template query
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

//template target
	V << "\n";
	if ( !half )
	{
		icount=0;
		step= ( endimageTarget-startimage ) /nthreads;

		for ( i=0;i<nthreads;i++ )
		{
			thread_data_array[i].thread_id=i;
			thread_data_array[i].startimage=i*step;
			thread_data_array[i].endimage= ( i+1 ) *step ;
			rc = pthread_create ( &threads[i], NULL, targettemplates, ( void * ) &thread_data_array[i] );
		}


		for ( i=0;i<nthreads;i++ )
		{
			rc = pthread_join ( threads[i], &status );
			if ( rc )
			{
				cerr << "\nERROR; return code from pthread_join() is " << rc << endl;
				exit ( -1 );
			}
		}


		rest= ( int ) fmod ( ( double ) endimageTarget-startimage,nthreads );
		if ( rest>0 )
		{
			thread_data_array[i-1].thread_id=i-1;
			thread_data_array[i-1].startimage=i*step;
			thread_data_array[i-1].endimage= i*step +rest;
			targettemplates ( ( void * ) &thread_data_array[i-1] );
		}
	}

	V << "\nElapsed time "<< s.Get_Elapsed_s() << "\n";
	for ( i = 0; i < endimageQuery*endimageTarget; i++ )  /////// loop for all Images found in the Xml file
		SimResult[i] = 0.0;

	int stepgap = 10000;
	s.start();
	if ( masking )
	{
		V << "Computing " << intra << " intra tests\n";

		step= intra /nthreads;
		for ( i=0;i<nthreads;i++ )
		{
			thread_data_array[i].thread_id=i;
			thread_data_array[i].startimage=i*step;
			thread_data_array[i].endimage= ( i+1 ) *step ;
			rc = pthread_create ( &threads[i], NULL, similarityintra, ( void * ) &thread_data_array[i] );
		}

		for ( i=0;i<nthreads;i++ )
		{
			rc = pthread_join ( threads[i], &status );
			if ( rc )
			{
				cerr << "\n*ERROR; return code from pthread_join() is " << rc << endl;
				exit ( -1 );
			}
		}

		rest= ( int ) fmod ( ( double ) intra,nthreads );
		if ( rest>0 )
		{
			thread_data_array[i-1].thread_id=i-1;
			thread_data_array[i-1].startimage=i*step;
			thread_data_array[i-1].endimage= i*step +rest;
			similarityintra ( ( void * ) &thread_data_array[i-1] );
		}

		V << "Computing " << inter << " inter tests\n";
		step= inter /nthreads;
		for ( i=0;i<nthreads;i++ )
		{
			thread_data_array[i].thread_id=i;
			thread_data_array[i].startimage=i*step;
			thread_data_array[i].endimage= ( i+1 ) *step ;
			rc = pthread_create ( &threads[i], NULL, similarityinter, ( void * ) &thread_data_array[i] );
		}

		for ( i=0;i<nthreads;i++ )
		{
			rc = pthread_join ( threads[i], &status );
			if ( rc )
			{
				cerr << "\n*ERROR: return code from pthread_join() is " << rc << endl;;
				exit ( -1 );
			}
		}

		rest= ( int ) fmod ( ( double ) inter,nthreads );
		if ( rest>0 )
		{
			thread_data_array[i-1].thread_id=i-1;
			thread_data_array[i-1].startimage=i*step;
			thread_data_array[i-1].endimage= i*step +rest;
			similarityinter ( ( void * ) &thread_data_array[i-1] );
		}

		intrat.clear();
		intert.clear();
	}
	else
	{

		icount=0;
		step= ( endimageTarget-startimage ) /nthreads;
		V << "Computing " << endimageTarget*endimageQuery << " tests\n";
		step= inter /nthreads;
		for ( i=0;i<nthreads;i++ )
		{
			thread_data_array[i].thread_id=i;
			thread_data_array[i].startimage=i*step;
			thread_data_array[i].endimage= ( i+1 ) *step ;
			rc = pthread_create ( &threads[i], NULL, similarity, ( void * ) &thread_data_array[i] );
		}

		for ( i=0;i<nthreads;i++ )
		{
			rc = pthread_join ( threads[i], &status );
			if ( rc )
			{
				cerr << "\n*ERROR: return code from pthread_join() is " << rc << endl;;
				exit ( -1 );
			}
		}

		int rest= ( int ) fmod ( ( double ) inter,nthreads );
		if ( rest>0 )
		{
			thread_data_array[i-1].thread_id=i-1;
			thread_data_array[i-1].startimage=i*step;
			thread_data_array[i-1].endimage= i*step +rest;
			similarity ( ( void * ) &thread_data_array[i-1] );
		}

	}

	V << "Elapsed time "<< s.Get_Elapsed_s() << "\n";
	V << "Saving Similarity matrix...\n";

	CreateDirForFile ( argv[6] ,false );
	ofstream SimFile ( argv[6] , ios::binary );
	if ( !SimFile.good() )
	{
		cerr << "*Error opening \"" << argv[6] << "\" for writing!" << endl;
		delete []DATATarget;
		delete []DATAQuery;
		delete []SimResult;
		exit ( -1 );
	}

	InitialSimMatrix ( SimFile,Comp.GetMeasuretype(), argv[2], argv[3], endimageQuery, endimageTarget );
	SimFile.write ( ( char * ) SimResult, sizeof ( float ) *endimageTarget*endimageQuery );
	SimFile.close();
	V << "Similarity saved and work done!\n";
	delete []DATATarget;
	delete []DATAQuery;
	delete []SimResult;
	delete []Pfc_temp;
	delete []thread_data_array;

	V.Closelog();
	return 0;
}
