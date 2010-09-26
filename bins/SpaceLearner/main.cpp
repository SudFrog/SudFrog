/***************************************************************************
 *   Copyright (C) 2009 by anouar   *
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

#include <pthread.h>
#include "TMSPFace.h"

using namespace TMSP_Face_Space;

struct thread_data *thread_data_array;
pthread_mutex_t mutexout;

inputs Recinputs;
Matrix DATA;
string ImageDir;
int steps=150,icount=1,startimage,endimage;
float lambda=1.0;
vector <XmlFace> ListofFile;

Gaborate *Pfc_Gab;
verbose V(2);

void *boostcreation ( void *threadpass )
{

  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;


  Pfc_Image *Lightenhanced;
  Mask NormMask ( Recinputs.norm_eye_dist );
  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {         /////// loop for all Images found in the Xml file
    pthread_mutex_lock ( &mutexout );
    icount++;
    cout << "\rGaborating " << icount << " from " << endimage;
    pthread_mutex_unlock ( &mutexout );

    string imagefilename = ImageDir + "/" + ListofFile[i].filename  ;

    if ( DoesFileExists ( imagefilename.c_str() ) ) {


      Pfc_Image InputImage;
      InputImage.ReadImage ( imagefilename.c_str() );
      Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );
      vector <double> Gabs=Pfc_Gab[my_data->thread_id].Gaborating ( Lightenhanced[0] );

      for ( int l=0;l< ( int ) Gabs.size();l++ )
        DATA ( l+1, i+1 ) =Gabs[l];
      Gabs.clear();


    } else {
      pthread_mutex_lock ( &mutexout );
      cerr << ListofFile[i].filename  << " Not found" << endl;
      pthread_mutex_unlock ( &mutexout );
      pthread_exit ( ( void* ) 0 );

    }
  }
  return ( void* ) 0;
// pthread_exit ( ( void* ) 0 );


}

void *boostloading ( void *threadpass )
{

  struct thread_data *my_data;
  my_data = ( struct thread_data * ) threadpass;


  Pfc_Image *Lightenhanced;
  Mask NormMask ( Recinputs.norm_eye_dist );
  for ( int i = my_data->startimage; i < my_data->endimage; ++i ) {         /////// loop for all Images found in the Xml file
    pthread_mutex_lock ( &mutexout );
    icount++;
    cout << "\rLoading " << icount << " from " << endimage;
    pthread_mutex_unlock ( &mutexout );

    string imagefilename = ImageDir + "/" + ListofFile[i].filename  ;

    if ( DoesFileExists ( imagefilename.c_str() ) ) {

      Pfc_Image InputImage;
      InputImage.ReadImage ( imagefilename.c_str() );
      Lightenhanced=InputImage.LightCorrect ( Recinputs.normlight, Recinputs.paramnorm,NormMask );

      int s=Lightenhanced[0].GetImSize();
      for ( int l=0;l<s;l++ )
        DATA ( l+1, i+1 ) =Lightenhanced[0].GetData ( l );

    } else {
      pthread_mutex_lock ( &mutexout );
      cerr << ListofFile[i].filename  << " Not found" << endl;
      pthread_mutex_unlock ( &mutexout );
      pthread_exit ( ( void* ) 0 );

    }
  }
  return ( void* ) 0;
// pthread_exit ( ( void* ) 0 );


}


/**
 * 
 * \page SpaceLearner SpaceLearner
 *
 * \section H1 How I'am??
 *    I'am the module that will compute the reduced space, based on the parameters given in the param xml file.
 * 
 *    I'am able to create different reduced spaces from gray-scale images using just the pixels values, or also using the Gabor filtering method
 * 
 * The follwing is the list of space reduction methods that I could handle (with or without Gabor filtering):
 * \li PCA, LDA, DLDA, KFA, GDA
 * \li when using Gabor I can handle the real, imaginary, magnitude, angle and a combination of those components.
 *
 * for more details please read the description of the nodes \b "gabor" and \b "reduction_space" at \link paramfile paramfile.xml \endlink.
 * \section Usage Usage: 
 *   SpaceLearner param xmlfile inputdir
 *
 * @param param
 *   Parameters xml file (see \link paramfile paramfile.xml \endlink)
 * @param xmlfile
 *   Xml file containing the list of users and there images files (Test images) (see \link Learning Learning.xml \endlink)
 * @param inputdir
 *   Path to the input geometrically normalized images (use \link NormFaces NormFaces \endlink for that)
 * @return 0 when succeed
 */

int main ( int argc, char **argv )
{

  if ( argc < 4 ) {
    cout << "Usage :" << endl << applicationname ( argv[0] ) << " param xmlfile inputdir" << endl;
    cout << "\t param: parameters xml file" << endl;
    cout << "\t xmlfile: Learning xml file" << endl;
    cout << "\t inputdir: input normalized image directory" << endl;
    exit ( -1 );
  }

  V.SetMutex ( &mutexout );
  vector <vector < XmlFace > > LDABASE;
  vector <int> labels;


  Recinputs.SetVerbose ( V );
  Recinputs.loadfromxml ( argv[1] );
  V.setmaxlevel(Recinputs.verboz);

  ImageDir=argv[3];
  int nthreads=Recinputs.nthreads;
  steps= ( int ) Recinputs.paramnorm[0];
  lambda= ( float ) Recinputs.paramnorm[1];

  pthread_t threads[nthreads];

  Pfc_Gab=new Gaborate[nthreads];
  thread_data_array=new thread_data[nthreads];
  Mask NormMask ( Recinputs.norm_eye_dist );
  strupr ( ( char * ) Recinputs.gabor_method.c_str() );
  if ( Recinputs.gabor_method!="NONE" ) {
    V << "Creating Gabor filters for " << nthreads << " threads\n";
    for ( int i=0;i<nthreads;i++ ) {
      Pfc_Gab[i].SetVerbose ( V );
      Pfc_Gab[i].SetGaborParams ( NormMask.getheight(),NormMask.getwidth(),
                                  Recinputs.Gabscales,Recinputs.Gaborientations,
                                  Recinputs.GabminWavelet, Recinputs.Gabmult,
                                  Recinputs.GabsigmaOnf,Recinputs.GabdThetaOnSigma );
      Pfc_Gab[i].SetProblem ( Recinputs.gabor_method );
      Pfc_Gab[i].SetReduction ( Recinputs.Gabreduction );
      Pfc_Gab[i].SetMask ( Recinputs.norm_eye_dist );
      Pfc_Gab[i].SetMutex ( &mutexout );
    }
  } else {
    Pfc_Gab[0].SetProblem ( "NONE" );
  }

  int ncon=Recinputs.controlled;
  int nnoncon=Recinputs.noncontrolled;
  int numberofus=Recinputs.nusers;
  int numberofim=0;

  V << "Parsing Images from xml...\n";

////////load files
  if ( ncon>0 || nnoncon >0 ) {
    LDABASE=loadLDAXml ( ( char* ) argv[2],ncon,nnoncon );
    numberofim=ncon+nnoncon;
  }


  if ( numberofus> ( int ) LDABASE.size() ) {
    V << "max clients to be selected: " << ( int ) LDABASE.size() << " change max client number and re-run\n";
    return -1;
  }


  int minim;

  for ( int i=0;i< numberofus;i++ ) {
    if ( numberofim > ( int ) LDABASE[i].size() ) {
      minim=LDABASE[i].size();
      V << "user " << i << " have only " << minim << " images, they will be used\n";

    } else
      minim=numberofim;

    for ( int j=0;j< minim;j++ )
      ListofFile.push_back ( LDABASE[i][j] );

    labels.push_back ( minim );
  }

  startimage=0;
  endimage= ( int ) ListofFile.size();
  int step=startimage;
  for ( int i = startimage; i < endimage; ++i )
    ListofFile[i].filename=ChangeFileExt ( ListofFile[i].filename,"pgm" );

  for ( int i = startimage; i < endimage; ++i ) {         /////// loop for all Images found in the Xml file
    V << "\rVerifying " << i+1 << " from " << endimage;
    string imagefilename = ImageDir + "/" + ListofFile[i].filename  ;

    if ( !DoesFileExists ( imagefilename.c_str() ) ) {
      cerr << endl << "Error: " << imagefilename  << " Not found" << endl;
      exit ( -1 );
    }
  }


  V << "\n";
  V << "All images found\n";
  Timer s;
  s.start();

  if ( Recinputs.gabor_method!="NONE" ) {
    DATA.ReSize ( Pfc_Gab[0].GetFeaturesize(),endimage-startimage );
    DATA=0.0;
    V << "Start Gaborating "<< endimage-startimage << " image with " << nthreads << " threads\n";
    Timer s;
    s.start();

    step= ( endimage-startimage ) /nthreads;

    int i,rc;
    for ( i=0;i<nthreads;i++ ) {
      thread_data_array[i].thread_id=i;
      thread_data_array[i].startimage=i*step;
      thread_data_array[i].endimage= ( i+1 ) *step ;
      rc = pthread_create ( &threads[i], NULL, boostcreation, ( void * ) &thread_data_array[i] );
    }

    void *status;

    for ( i=0;i<nthreads;i++ ) {
      rc = pthread_join ( threads[i], &status );
      if ( rc ) {
        cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
        exit ( -1 );
      }
    }


    int rest= ( int ) fmod ( ( double ) endimage-startimage,nthreads );
    if ( rest>0 ) {
      thread_data_array[i].thread_id=i-1;
      thread_data_array[i].startimage=i*step;
      thread_data_array[i].endimage= i*step +rest;
      boostcreation ( ( void * ) &thread_data_array[i] );
    }
  } else {
    DATA.ReSize ( NormMask.getwidth() *NormMask.getheight(),endimage-startimage );
    DATA=0.0;
    V << "Start Loading "<< endimage-startimage << " image with " << nthreads << " threads\n";
    step= ( endimage-startimage ) /nthreads;

    int i,rc;
    for ( i=0;i<nthreads;i++ ) {
      thread_data_array[i].thread_id=i;
      thread_data_array[i].startimage=i*step;
      thread_data_array[i].endimage= ( i+1 ) *step ;
      rc = pthread_create ( &threads[i], NULL, boostloading, ( void * ) &thread_data_array[i] );
    }

    void *status;

    for ( i=0;i<nthreads;i++ ) {
      rc = pthread_join ( threads[i], &status );
      if ( rc ) {
        cerr << "\nERROR; return code from pthread_join() is " << rc << endl;;
        exit ( -1 );
      }
    }


    int rest= ( int ) fmod ( ( double ) endimage-startimage,nthreads );
    if ( rest>0 ) {
      thread_data_array[i].thread_id=i-1;
      thread_data_array[i].startimage=i*step;
      thread_data_array[i].endimage= i*step +rest;
      boostloading ( ( void * ) &thread_data_array[i] );
    }
  }

  V << "\nElapsed time "<< s.Get_Elapsed_s() << "\n";
  V << "All jobs completed join\n";

  V.setmaxlevel(1);
  if ( Recinputs.reduction_method=="PCA" || Recinputs.reduction_method=="LDA" || Recinputs.reduction_method=="DLDA" ) {
    LinearReducer Lin;
    Lin.SetVerbose ( V );
    Lin.SetProblem ( Recinputs.reduction_method );
    Lin.SetData ( DATA );
    DATA.ReSize ( 0,0 );
    DATA.ReleaseAndDelete();

 if ( Recinputs.reduction_method=="PCA" && Recinputs.reduction_param!="")
	Lin.Setmaxvariance(Recinputs.reduction_param);
else
    if ( Recinputs.reduction_method=="LDA" || Recinputs.reduction_method=="DLDA" ) {
      Lin.SetLabels ( labels );
    }

    Lin.ComputeSpace();
    CreateDirForFile ( Recinputs.reduction_space_file,false );
    Lin.SaveSpace ( ( char* ) ( Recinputs.reduction_space_file ).c_str(),Pfc_Gab[0].parameters );
    V << "Space saved and work done!\n";
  } else {

if ( Recinputs.reduction_method=="KFA" || Recinputs.reduction_method=="GDA")
{
    NonLinearReducer Reducer;
    Reducer.SetVerbose ( V );
    Reducer.SetProblem ( Recinputs.reduction_method );
    Reducer.SetkernelPartFromline(Recinputs.reduction_param);
    Reducer.SetDatawithLabels ( DATA,labels );
    DATA.ReSize ( 0,0 );
    DATA.ReleaseAndDelete();
    Reducer.ComputeSpace();
    CreateDirForFile ( Recinputs.reduction_space_file,false );
    Reducer.SaveSpace ( ( char* ) ( Recinputs.reduction_space_file ).c_str(),Pfc_Gab[0].parameters );
    V << "Space saved and work done!\n";
}
// non linear to be implemented

  }

  return 0;
}

