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

string ImageDir;
vector < vector < int > > SelectedQuery;
vector < double > *DATATarget;
vector < vector < double > > *DATAQuery;

vector <XmlFace> ListofTargetStill;
vector <vector <XmlFace> > ListofQueryVideo;
int endimageTargetStill,endimageQueryVideo;

vector < vector < float * > > SimResult;

verbose V ( 0 );
int icount=0;

int saveresselect ( const char* filename, double *res,int size )
{
  ofstream selectedQ ( filename );
  if ( selectedQ.good() )
    for ( int i=0;i<size;i++ ) {
      for ( int j=0;j<size;j++ )
        selectedQ << res[i*size+j] << "\t";

      selectedQ << endl;
    }

  selectedQ.close();
}

bool findInVector ( vector<int>&select,int val )
{

  for ( int i =0; i<select.size();i++ )
    if ( !isnan ( select[i] ) )
      if ( select[i]==val )
        return true;

  return false;
}

int countnonnan ( double* res,int size )
{
  int count=0;
  for ( int h = 0; h < size; ++h )
    if ( !isnan ( res[h] ) )
      count++;

  return count;
}

int Discardmax ( double* res,int width,int height,vector<int>&select ,int keep )
{
  float val=MINFLOAT;
  int maxi=0;
  while ( countnonnan ( res,height*width ) > keep ) {
    for ( int h = 0; h < height*width; ++h )
      if ( !isnan ( res[h] ) )
        if ( val < res[h] ) {
          val=res[h];
          maxi=h;
        }
    val=MINFLOAT;
    res[maxi]=NAN;
  }

  for ( int h = 0; h < height; ++h )
    for ( int w = h+1; w < width; ++w ) {
      if ( !isnan ( res[h*width+w] ) ) {
        if ( !findInVector ( select,h ) )
          select.push_back ( h );
        if ( !findInVector ( select,w ) )
          select.push_back ( w );
      }
    }


  return 0;

}

float getpseudodist ( float *vect,int  size,int method )
{

  float val=0;
  if ( size>0 ) {
    switch ( method ) {
      case 0:
        val=vect[0];
        for ( int i=0;i<size;i++ )
          if ( val>vect[i] )
            val=vect[i];
        break;
      case 1:
        val=vect[0];
        for ( int i=0;i<size;i++ )
          if ( val<vect[i] )
            val=vect[i];
        break;

      case 2:
        val=0.0;
        for ( int i=0;i<size;i++ )
          val+=vect[i];
        val/=size;
        break;
    }
  }
  return val;

}

void* querytemplates ( void *threadpass )
{

  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;

  Pfc_Image *Lightenhanced;
  Mask NormMask ( Recinputs.norm_eye_dist );
  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {         /////// loop for all Images found in the Xml file
    pthread_mutex_lock ( &mutexout );
    icount++;
    cout  << "\rTemplating query "<< ( int ) 100.0*icount/endimageQueryVideo << "%";
    cout.flush();
    pthread_mutex_unlock ( &mutexout );
    for ( int j = 0; j < ListofQueryVideo[i].size(); ++j ) {

      string imagefilename = ImageDir + "/" + ListofQueryVideo[i][j].filename  ;

      if ( DoesFileExists ( imagefilename.c_str() ) ) {

        Pfc_Image InputImage;
        InputImage.ReadImage ( imagefilename.c_str() );
        Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );

        DATAQuery[i].push_back ( Pfc_temp[my_data->thread_id].Template ( Lightenhanced[0] ) )  ;
      } else {
        V << ListofQueryVideo[i][j].filename  << " Not found\n";
        pthread_exit ( ( void* ) -1 );
      }
    }
  }
  return ( void* ) 0;
}



void* targettemplates ( void *threadpass )
{
  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;
  Pfc_Image *Lightenhanced;
  Mask NormMask ( Recinputs.norm_eye_dist );
  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {
    pthread_mutex_lock ( &mutexout );
    icount++;
    cout  << "\rTemplating target "<< ( int ) 100.0*icount/endimageTargetStill << "%";
    pthread_mutex_unlock ( &mutexout );
    string imagefilename = ImageDir + "/" + ListofTargetStill[i].filename  ;

    if ( DoesFileExists ( imagefilename.c_str() ) ) {

      Pfc_Image InputImage;
      InputImage.ReadImage ( imagefilename.c_str() );
      Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
      //Lightenhanced[0].Stretch ( 2, NormMask );


      DATATarget[i]=Pfc_temp[my_data->thread_id].Template ( Lightenhanced[0] );
      //Pfc_temp[my_data->thread_id].SaveTemplate();

    } else {
      V << ListofTargetStill[i].filename   << " Not found\n";
      pthread_exit ( ( void* ) -1 );
    }
  }
  return ( void* ) 0;
}




void* similarity ( void *threadpass )
{
  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;
  Comparator Comp;
  Comp.Set_Distance ( Recinputs.sim_measure );
  int j;
  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {
    for ( int j = 0; j < endimageTargetStill; ++j ) {
      pthread_mutex_lock ( &mutexout );
      icount++;
      cout  << "\rSimilarity "<< ( int ) 100.0*icount/ ( endimageQueryVideo*endimageTargetStill ) << "%";
      cout.flush();
      pthread_mutex_unlock ( &mutexout );
      for ( int h = 0; h < SelectedQuery[i].size(); ++h )
        SimResult[i][j][h] = Comp.GetTemplatesDistance ( DATAQuery[i][SelectedQuery[i][h]] ,DATATarget[j] );
    }
  }
  return ( void* ) 0;
}



void* SelectStableQuery ( void *threadpass )
{
  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;
  Comparator Comp;
  Comp.Set_Distance ( C_Angle );


  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {
    double* localsim=new double [DATAQuery[i].size() *DATAQuery[i].size() ];

    for ( int h = 0; h < DATAQuery[i].size() *DATAQuery[i].size(); ++h )
      localsim[h]=NAN;

    for ( int h = 0; h < DATAQuery[i].size(); ++h )
      for ( int j = h+1; j < DATAQuery[i].size(); ++j )
        localsim[h*DATAQuery[i].size() +j]=Comp.GetTemplatesDistance ( DATAQuery[i][h] ,DATAQuery[i][j] );

    Discardmax ( localsim,DATAQuery[i].size(),DATAQuery[i].size(),SelectedQuery[i], ( DATAQuery[i].size() *DATAQuery[i].size() ) );

    ostringstream fn;
    fn<< ( string ) "res/video_" << i;
    saveresselect ( fn.str().c_str(),localsim,DATAQuery[i].size() );
    pthread_mutex_lock ( &mutexout );
    icount++;
    cout  << "\rvideo :"<< ( int ) icount;
    pthread_mutex_unlock ( &mutexout );
    delete []localsim;
  }

  return ( void* ) 0;
}

/** 
 * \page MBGC_RUN mbgc_run
 *
 * \section H1 How I'am??
 *    I'am the module that can run the MBGC-v1 and v2 experiments (Never ask the database from my author just go to http://www.frvt.org/MBGC/ and ask). 
 *
 * The author could give you only the xml metadata and the masks. 
 *
 *  I use the multi-threading to accelerate your work!
 * \section Usage Usage: 
 *   mbgc_run param queryxml query_meta targetxml target_meta inputdir Similaritymatrix
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param queryxml
 *   Xml file containing the list of Query files (Test Videos) (see mbgc query xml format)
 * @param query_meta
 *   Xml file containing the metadata of the Query files (see \link Metadata Metadata.xml \endlink example of the xml format)
 * @param targetxml
  *   Xml file containing the list of Target files (Reference images) (see mbgc target xml format)
 * @param target_meta
 *   Xml file containing the metadata of the Target files (see \link Metadata Metadata.xml \endlink example of the xml format)
 * @param inputdir
 *   Path to the input geometrically normalized images (use \link NormFaces NormFaces \endlink for that)
 * @param Similaritymatrix
 *   The output file containing the Similarity matrix (see MBGC similarity format)
 * @return 0 when succeed
 */

int main ( int argc, char **argv )
{

  int startimage = 0, i, j, step = 0,rc;
  vector <int> NullVect;
  V.SetMutex ( &mutexout );
  Recinputs.SetVerbose ( V );

  if ( argc < 8 ) {
    cout << "Usage :" << endl << applicationname ( argv[0] ) << " param query query_meta target target_meta normdir simfile" << endl;
    cout << "\t param: parameters xml file" << endl << endl;
    exit ( -1 );
  }

  Recinputs.loadfromxml ( argv[1] );
  ImageDir = argv[6];

//validate distance mesure;
  Comparator Comp;
  Comp.Set_Distance ( Recinputs.sim_measure );

  int nthreads=Recinputs.nthreads;
  pthread_t threads[nthreads];
  Pfc_temp=new Templator[nthreads];
  thread_data_array=new thread_data[nthreads];

  CreateDir ( "res" );
  V << "Parsing target images from: \n\t" << ( const char * ) argv[4] << "\n";
  ListofTargetStill = MbgcStillMetaXml ( argv[4],argv[5] );
  endimageTargetStill= ( int ) ListofTargetStill.size();

  for ( i = startimage; i < endimageTargetStill; ++i )
    ListofTargetStill[i].filename=ChangeFileExt ( ListofTargetStill[i].filename,"pgm" );

  for ( i = startimage; i < endimageTargetStill; ++i ) {         /////// loop for all Images found in the Xml file
    string imagefilename = ImageDir + "/" + ListofTargetStill[i].filename  ;
    V << CLEARLINE << "Verifying " << i + 1 << " from " << endimageTargetStill;
    if ( !DoesFileExists ( imagefilename.c_str() ) ) {
      cerr << "\n" << ListofTargetStill[i].filename  << " Not found" << endl;
      exit ( -1 );
    }
  }

  Timer s;
  s.start();
  V << "\nParsing query images from: \n\t" << ( const char * ) argv[2] << "\n";
  ListofQueryVideo = MbgcVideoMetaXml ( argv[2],argv[3] );
  endimageQueryVideo = ( int ) ListofQueryVideo.size();

  for ( i = startimage; i < endimageQueryVideo; ++i )
    for ( j = 0; j < ListofQueryVideo[i].size(); ++j )
      ListofQueryVideo[i][j].filename=ChangeFileExt ( ListofQueryVideo[i][j].filename,"pgm" );

  //cout << endimageQueryVideo << endl;
  for ( i = startimage; i < endimageQueryVideo; ++i ) {
    //V << "Video: " << i << "\n";
    for ( j = startimage; j < ListofQueryVideo[i].size(); ++j ) {
      string imagefilename = ImageDir + "/" + ListofQueryVideo[i][j].filename  ;
      V << "\rVerifying " << i + 1 << " from " << endimageQueryVideo;
      // V << "\t" << imagefilename << "\n";
      if ( !DoesFileExists ( imagefilename.c_str() ) ) {
        cerr << "\n" << ListofQueryVideo[i][j].filename << " Not found" << endl;
        exit ( -1 );
      }
    }
  }

  V<< "\nElapsed time: " << s.Get_Elapsed_s() << "\n";

  DATAQuery=new vector < vector < double > >[endimageQueryVideo];
  DATATarget=new vector < double >[endimageTargetStill];

  for ( i=0;i<endimageQueryVideo;i++ )
    SelectedQuery.push_back ( NullVect );


  V << "Creating templator for " << nthreads << " threads\n";
  for ( i=0;i<nthreads;i++ ) {
    Pfc_temp[i].SetVerbose ( V );
    Pfc_temp[i].SetMutex ( &mutexout );
    Pfc_temp[i].SetMask ( Recinputs.norm_eye_dist );
    Pfc_temp[i].SetSpacefile ( Recinputs.reduction_space_file.c_str() );
  }

  void *status;
  icount=0;
  step= ( endimageTargetStill-startimage ) /nthreads;

  for ( i=0;i<nthreads;i++ ) {
    thread_data_array[i].thread_id=i;
    thread_data_array[i].startimage=i*step;
    thread_data_array[i].endimage= ( i+1 ) *step ;
    rc = pthread_create ( &threads[i], NULL, targettemplates, ( void * ) &thread_data_array[i] );
  }

  for ( i=0;i<nthreads;i++ ) {
    rc = pthread_join ( threads[i], &status );
    if ( rc ) {
      cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
      exit ( -1 );
    }
  }

  int  rest= ( int ) fmod ( ( double ) endimageTargetStill-startimage,nthreads );
  if ( rest>0 ) {
    thread_data_array[i-1].thread_id=i-1;
    thread_data_array[i-1].startimage=i*step;
    thread_data_array[i-1].endimage= i*step +rest;
    targettemplates ( ( void * ) &thread_data_array[i-1] );
  }

  V << "\n";
  icount=0;
  step= ( endimageQueryVideo-startimage ) /nthreads;

  for ( i=0;i<nthreads;i++ ) {
    thread_data_array[i].thread_id=i;
    thread_data_array[i].startimage=i*step;
    thread_data_array[i].endimage= ( i+1 ) *step ;
    rc = pthread_create ( &threads[i], NULL, querytemplates, ( void * ) &thread_data_array[i] );
  }

  for ( i=0;i<nthreads;i++ ) {
    rc = pthread_join ( threads[i], &status );
    if ( rc ) {
      cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
      exit ( -1 );
    }
  }


  rest= ( int ) fmod ( ( double ) endimageQueryVideo-startimage,nthreads );
  if ( rest>0 ) {
    thread_data_array[i-1].thread_id=i-1;
    thread_data_array[i-1].startimage=i*step;
    thread_data_array[i-1].endimage= i*step +rest;
    querytemplates ( ( void * ) &thread_data_array[i-1] );
  }

  V << "\nElapsed time "<< s.Get_Elapsed_s() << "\n";

  for ( i=0;i<endimageQueryVideo;i++ ) {
    SelectedQuery.push_back ( NullVect );
    //for ( j=0;j<ListofQueryVideo[i].size();j++ )
    //SelectedQuery[i].push_back ( j );
  }

  V <<"Selecting stable query\n";

  icount=0;

  for ( i=0;i<nthreads;i++ ) {
    thread_data_array[i].thread_id=i;
    thread_data_array[i].startimage=i*step;
    thread_data_array[i].endimage= ( i+1 ) *step ;
    rc = pthread_create ( &threads[i], NULL, SelectStableQuery, ( void * ) &thread_data_array[i] );
  }

  for ( i=0;i<nthreads;i++ ) {
    rc = pthread_join ( threads[i], &status );
    if ( rc ) {
      cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
      exit ( -1 );
    }
  }

  if ( rest>0 ) {
    thread_data_array[i-1].thread_id=i-1;
    thread_data_array[i-1].startimage=i*step;
    thread_data_array[i-1].endimage= i*step +rest;
    SelectStableQuery ( ( void * ) &thread_data_array[i-1] );
  }

  V <<"\nSelection done!\n";
  ofstream selectedQ ( "res/SelectedQ.txt" );
  if ( selectedQ.good() )
    for ( i=0;i<endimageQueryVideo;i++ ) {
      selectedQ << "Video " << i << ": ";
      for ( int h = 0; h < SelectedQuery[i].size(); ++h )
        selectedQ << SelectedQuery[i][h] <<"\t";

      selectedQ << endl;
    }
  selectedQ.close();

  V << "Computing similarity \n";

  for ( i=0;i<endimageQueryVideo;i++ ) {
    vector < float * > tar;
    for ( j=0;j<endimageTargetStill;j++ ) {
      float *vect=new float [ SelectedQuery[i].size() ];
      tar.push_back ( vect );
    }
    SimResult.push_back ( tar );
  }

  s.start();
  icount=1;
  step= endimageQueryVideo /nthreads;

  for ( i=0;i<nthreads;i++ ) {
    thread_data_array[i].thread_id=i;
    thread_data_array[i].startimage=i*step;
    thread_data_array[i].endimage= ( i+1 ) *step ;
    rc = pthread_create ( &threads[i], NULL, similarity, ( void * ) &thread_data_array[i] );
  }

  for ( i=0;i<nthreads;i++ ) {
    rc = pthread_join ( threads[i], &status );
    if ( rc ) {
      cerr << "\n*ERROR: return code from pthread_join() is " << rc << endl;;
      exit ( -1 );
    }
  }

  rest= ( int ) fmod ( ( double ) endimageQueryVideo,nthreads );
  if ( rest>0 ) {
    thread_data_array[i-1].thread_id=i-1;
    thread_data_array[i-1].startimage=i*step;
    thread_data_array[i-1].endimage= i*step +rest;
    similarity ( ( void * ) &thread_data_array[i-1] );
  }

  V << "\nElapsed time "<< s.Get_Elapsed_s() << "\n";
  V << "Saving Similarity matrix...\n";
  ofstream SimFile ( argv[7] , ios::binary );
  if ( !SimFile.good() ) {
    cerr << "*Error opening \"" << argv[7] << "\" for writing!" << endl;
    delete []DATATarget;
    delete []DATAQuery;
    exit ( -1 );
  }

  float *Sim=new float[endimageTargetStill*endimageQueryVideo];
  for ( i=0;i<endimageQueryVideo;i++ )
    for ( j=0;j<endimageTargetStill;j++ )
      Sim[i*endimageTargetStill+j]=getpseudodist ( SimResult[i][j], ( int ) SelectedQuery[i].size(),2 );

  InitialSimMatrix ( SimFile,Comp.GetMeasuretype(), argv[2], argv[5], endimageQueryVideo, endimageTargetStill );
  SimFile.write ( ( char * ) Sim, sizeof ( float ) *endimageTargetStill*endimageQueryVideo );
  SimFile.close();
  V << "Similarity saved and work done!\n";
  delete []DATATarget;
  delete []DATAQuery;
  delete []Pfc_temp;
  delete []thread_data_array;

  V.Closelog();
  return 0;
}
