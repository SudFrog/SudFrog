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


#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "Matrix_util.h"


#define INTRA 255
#define INTER 127

using namespace std;

void GetConfidence ( double *Far,double*Frr,int histsize,int numimp,int numclient,double *&FarC,double*&FrrC,double confid );
float min ( float a,float b );
float max ( float a,float b );
int Getproblemtype ( vector<float> &inter,vector<float> &intra );
int norm_dev ( double *Fr,int histsize,double *&ndf );

class inputarg
{
  public:
    string Simfile,maskfile,outintraHist,outinterHist,FarFrr,type;
    bool feed;
    float histstep;
    bool outscores;
    string outscoreintra,outscoreinter;

    inputarg() {
      Simfile="";
      maskfile="";
      outintraHist="/dev/null";
      outinterHist="/dev/null";
      FarFrr="/dev/null";
      type="Auto";
      histstep=0.0;
      feed=false;
      outscores=false;
      outscoreintra="/dev/null";
      outscoreinter="/dev/null";
    }

    ~inputarg() {}

    bool Getinputs ( int ac,char * av[] ) {
      for ( int i=1;i<ac;i++ ) {
        if ( av[i][0]=='-' ) {
          switch ( av[i][1] ) {

            case 'S':
              Simfile=av[i+1];
              break;
            case 'M':
              maskfile=av[i+1];
              break;
            case 'R':
              FarFrr=av[i+1];
              break;
            case 'h':
              histstep=atof ( av[i+1] );
              break;
            case 'a':
              outintraHist=av[i+1];
              break;
            case 'b':
              outinterHist= av[i+1];
              break;
            case 't':
              type= av[i+1];
              break;
            case 'f':
              feed= true;
              break;
            case 'i':
              outscores=true;
              outscoreintra= av[i+1];
              break;
            case 'e':
              outscores=true;
              outscoreinter= av[i+1];
              break;
          }

        }
      }

      if ( Simfile=="" ) {
        cout << "need Sim file" << endl;
        printout ( av[0] );
      }

      if ( maskfile=="" ) {
        cout << "need mask file" << endl;
        printout ( av[0] );
      }

      if ( histstep==0.0 ) {
        cout << "need  histogram step discretisation"<< endl;
        printout ( av[0] );
      }

      printNonnullinputs();

      return true;

    }

    void printNonnullinputs() {
      if ( feed ) {
        cout << "list of arguments:" << endl;

        if ( Simfile!="" )
          cout << "Sim file: " << Simfile << endl;

        if ( maskfile!="" )
          cout << "mask file: " << maskfile << endl;

        if ( outintraHist!="" )
          cout << "output intra-results histogram file: " << outintraHist << endl;

        if ( outinterHist!="" )
          cout << "output inter-results histogram file: " << outinterHist << endl;

        if ( FarFrr!="" )
          cout << "output FarFrr file: " << FarFrr << endl;

        cout << "Histogram step: " << histstep << endl;

        cout << "problem type: " << type << endl;

        if ( outscores ) {
          cout << "score intra will be saved: " << outscoreintra << endl;
          cout << "score inter will be saved: " << outscoreinter << endl;
        }

      }
    }

    void printout ( char * prog ) {
      cout << "usage " << prog << " -S Simfile -M maskfile -h Hist-step [-a ouputintraHist [/dev/null]] [-b outputinterHist[/dev/null]] [-R FarFrr[/dev/null]] [-t type (distance/similarity) [distance]] -[f feedback] [-i intrascore] [-e interscores]" << endl;
      exit ( 1 );
    }

};

int IfFileExists ( char * file )
{
  ifstream testfile ( file );

  if ( !testfile.good() ) {
    printf ( "Error opening %s\n",file );
    return false;
  }

  testfile.close();

  return true;
}

struct point {
  int x;
  int y;
};


/**
 * 
 * \page GetPermformance GetPermformance
 *
 * \section H1 How I'am??
 *   I'am the module that computes the performance of the algorithms when you give me the similarity matrix, and the corresponding mask. 
 *   I can output many things, so read the following, but I give at the minimum the EER and the VR at 0.1% of FAR.
 * \section Usage Usage: 
 *   GetPerformance -S Simfile -M maskfile -h Hist-step [-a ouputintraHist [/dev/null]] [-b outputinterHist[/dev/null]] [-R FarFrr[/dev/null]] [-t type (distance/similarity) [distance]] -[f feedback] [-i intrascore] [-e interscores]
 *
 * @param Simfile
 *   The similarity matrix created by \link FRGC_RUN frgc_run \endlink or \link MBGC_RUN mbgc_run \endlink
 * @param maskfile
 *   Mask file containing the intra/interclass tests (see MBGC mask format)
 * @param Hist-step
 *   histogramme sampling steps generally (maxscore - minscore)/10000 will be enough.
 * @param ouputintraHist
 *   it's an optional output file, if set, the distribution of intraclass (genuine) comparisons tests will be saved
 * @param outputinterHist
*   it'salso  an optional output file, if set, the distribution of interclass (impostor) comparisons tests will be saved
 * @param FarFrr
 *   it's  an optional output file, if set, the score FAR FRR the confidences will be saved
 * @param type
 *   it's  optional value [distance/similarity], by default the system will detect if the used measure is similarity or a distance
 * @param feedback
 *   it's allow verbosing
 * @param intrascore
 *   it's  an optional output file, if set, the intraclass scores will be saved
 * @param interscores
 *   it's  an optional output file, if set, the interclass scores will be saved
 *
 * @return 0 when succeed
 *
 */


int main ( int argc, char* argv[] )
{
  inputarg inputs;
  inputs.Getinputs ( argc,argv );

  if ( IfFileExists ( ( char* ) inputs.Simfile.c_str() ) ==-1 ) {
    cout << inputs.Simfile << " not found" << endl;
    return ( -1 );
  }

  if ( IfFileExists ( ( char* ) inputs.maskfile.c_str() ) ==-1 ) {
    cout << inputs.maskfile << " not found" << endl;
    return ( -1 );
  }

  double step=inputs.histstep;

  int SimQueryNum, SimTargetNum;
  int MaskQueryNum, MaskTargetNum;
  int type=0,i,j;
  double * Far,* Frr;

  cout << "Mask ";
  cout.flush();
  //cout << "Loading Mask file..." <<endl;
  Matrix Smask=loadMask ( ( char* ) inputs.maskfile.c_str() );
  MaskQueryNum=Smask.Nrows();
  MaskTargetNum=Smask.Ncols();
  int intra=0,inter=0;
////////////////
  Matrix Similarity=loadSim ( ( char* ) inputs.Simfile.c_str() );

//just for testing
  for ( i = 1;i <= Smask.Nrows();i++ ) {
    for ( j = 1;j <= Smask.Ncols();j++ ) {
      if ( isnan ( Similarity ( i,j ) ) ){
        Smask ( i,j )=0;
	cout << "problem at comparison Video:"<< i << " vs Still:" << j << endl;
	}
    }
  }

  vector <point> intrat, intert;

  for ( i = 1;i <= Smask.Nrows();i++ ) {
    for ( j = 1;j <= Smask.Ncols();j++ ) {
      point coord;

      if ( Smask ( i, j ) == INTRA ) {
        coord.x = i;
        coord.y = j;
        intrat.push_back ( coord );

      } else

        if ( Smask ( i, j ) == INTER ) {
          coord.x = i;
          coord.y = j;
          intert.push_back ( coord );
        }
    }
  }

  intra = ( int ) intrat.size();
  inter = ( int ) intert.size();

  if ( intra==0 || inter==0 ) {
    cout << "No between or within class score found" << endl;
    Smask.ReSize ( 0,0 );
    Smask.ReleaseAndDelete();
    return ( -1 );
  }

  Smask.ReSize ( 0,0 );
  Smask.ReleaseAndDelete();

  cout << intra << " : Client tests" << endl << inter << " : Impostor tests" << endl;
/////////////////////
  vector < float > intrascores,interscores;
  double minintra=MAXFLOAT,maxintra=MINFLOAT,mininter=MAXFLOAT,maxinter=MINFLOAT;

  cout << "Similarity ";
  cout.flush();

  SimQueryNum=Similarity.Nrows();
  SimTargetNum=Similarity.Ncols();

  if ( SimQueryNum!=MaskQueryNum || SimTargetNum!=MaskTargetNum ) {
    cout << "Sim dimension and mask dimension mismatch" << endl;
    return -1;

  } else {
    cout << "Loading intra/inter results..." <<endl;

    for ( i = 0; i < intra; i++ ) {                     /////// loop for all Images found in the Xml file
      intrascores.push_back ( Similarity ( intrat[i].x, intrat[i].y ) );

      if ( minintra > Similarity ( intrat[i].x, intrat[i].y ) )
        minintra=Similarity ( intrat[i].x, intrat[i].y );
      else
        if ( maxintra < Similarity ( intrat[i].x, intrat[i].y ) )
          maxintra=Similarity ( intrat[i].x, intrat[i].y );
    }

    for ( i = 0; i < inter; i++ ) {                     /////// loop for all Images found in the Xml file
      interscores.push_back ( Similarity ( intert[i].x, intert[i].y ) );

      if ( mininter >Similarity ( intert[i].x, intert[i].y ) )
        mininter=Similarity ( intert[i].x, intert[i].y );
      else
        if ( maxinter < Similarity ( intert[i].x, intert[i].y ) )
          maxinter=Similarity ( intert[i].x, intert[i].y );

      //cout << SimResult[ intrat[i].x*endimageTarget+ intrat[i].y ] << endl;
    }

    Similarity.ReleaseAndDelete();

    if ( inputs.type=="Distance" )
      type=0;
    else
      if ( inputs.type=="Similarity" )
        type=1;
      else
        if ( inputs.type=="Auto" ) {
          type=Getproblemtype ( interscores,intrascores );

          switch ( type ) {
            case 0:
              cout << "problem detected: Distance" << endl;
              break;
            case 1:
              cout << "problem detected: Similarity" << endl;
              break;
          }
        }

    if ( inputs.outscores ) {
      cout << "Saving scores" << endl;
      ofstream outintra ( ( char * ) inputs.outscoreintra.c_str() );

      for ( int i=0;i<intra;i++ )
        outintra<<intrascores[i] << endl;

      outintra.close();

      ofstream outinter ( ( char * ) inputs.outscoreinter.c_str() );

      for ( int i=0;i<inter;i++ )
        outinter<<interscores[i] << endl;

      outinter.close();
    }

    cout << fixed;

    cout.precision ( 3 );
    cout << "number match/mismatch " << intra << "/" << inter << endl;
    cout << "minimum/maximum intraclass tests " << minintra << "/" << maxintra << endl;
    cout << "minimum/maximum interclass tests " << mininter << "/" << maxinter << endl;

    double Cummin=min ( minintra,mininter );
    double Cummax=max ( maxintra,maxinter );
    int histsize=int ( ceil ( ( Cummax-Cummin ) /step ) );

    int *histintra,*histinter;
    histintra=new int [histsize];
    histinter=new int [histsize];

    int pos;

    for ( int i=0;i<histsize;i++ )
      histintra[i]=0;

    for ( int i=0;i<histsize;i++ )
      histinter[i]=0;

// cout << "Constructing intratests histogramme " << endl;
    for ( int i=0;i<intra;i++ ) {
      pos= ( int ) floor ( ( intrascores[i]-Cummin ) /step );
      histintra[pos]++;
    }

// cout << "Constructing intertests histogramme " << endl;

    for ( int i=0;i<inter;i++ ) {
      pos= ( int ) floor ( ( interscores[i]-Cummin ) /step );
      histinter[pos]++;
    }

    ofstream outintra ( ( char* ) inputs.outintraHist.c_str() );

    ofstream outinter ( ( char* ) inputs.outinterHist.c_str() );

    for ( int i=0;i<histsize;i++ )
      outintra << Cummin+i*step << "\t" << histintra[i] << endl;

    outintra.close();

    for ( int i=0;i<histsize;i++ )
      outinter << Cummin+i*step << "\t" << histinter[i] << endl;

    outinter.close();

    intrascores.clear();

    interscores.clear();

    int *Cumhistintra,*Cumhistinter;

    Cumhistintra=new int [histsize];

    Cumhistinter=new int [histsize];

    Cumhistintra[0]=histintra[0];

    for ( int i=1;i<histsize;i++ )
      Cumhistintra[i]=Cumhistintra[i-1]+histintra[i];

    delete []histintra;

    Cumhistinter[0]=histinter[0];

    for ( int i=1;i<histsize;i++ )
      Cumhistinter[i]=Cumhistinter[i-1]+histinter[i];

    delete []histinter;



    Far=new double [histsize];

    Frr=new double [histsize];

    switch ( type ) {
      case 0:

        for ( int i=0;i<histsize;i++ ) {
          Frr[i]=double ( intra-Cumhistintra[i] ) /intra;
          Far[i]=double ( Cumhistinter[i] ) /inter;
        }

        break;

      case 1:

        for ( int i=0;i<histsize;i++ ) {
          Frr[i]=double ( Cumhistintra[i] ) /intra;
          Far[i]=double ( inter-Cumhistinter[i] ) /inter;
        }

        break;

    }

    double *FarC,*FrrC;
    double *ndivFar,*ndivFrr;

    GetConfidence ( Far,Frr,histsize,inter,intra,FarC,FrrC,1.96 );
    // norm_dev ( Far,histsize,ndivFar );
    // norm_dev ( Frr,histsize,ndivFrr );

    delete []Cumhistintra;
    delete []Cumhistinter;
// 	diff=new double [histsize];
    pos=0;
    float val=0.0;
    float minimum=MAXFLOAT;

    for ( int i=0;i<histsize;i++ ) {
      val=fabs ( Frr[i]-Far[i] );

      if ( minimum > val ) {
        pos=i;
        minimum=val;
      }
    }

    float FrrEER=Frr[pos],FarEER=Far[pos];

    double EER= ( Far[pos]+Frr[pos] ) /2.0;
    double FarCV=FarC[pos]*100.0;
    double FrrCV=FrrC[pos]*100.0;

//cout << "EER: " << EER*100.0 << "%" << endl;
    pos=0;

    switch ( type ) {
      case 0:

        for ( int i=0;i<histsize;i++ ) {
          if ( Far[i]>=0.001 ) {
            pos=i;
            break;
          }
        }

        break;

      case 1:

        for ( int i=0;i<histsize;i++ ) {
          if ( Far[i]<=0.001 ) {
            pos=i;
            break;
          }
        }

        break;
    }

    double VR=1-Frr[pos];

//  cout << "VR: " << VR*100 << "% @0.1% Far"<< endl;
    ofstream outFarFrr ( inputs.FarFrr.c_str() );
    outFarFrr << "scores\tFrr\tFar\t1-Frr\t1-Far\tFrrC\tFarC" << endl;

    // cout << "test" << endl;
    for ( int i=0;i<histsize;i++ )
      // outFarFrr << Cummin+i*step << "\t" << Frr[i] << "\t" << Far[i] << "\t" << 1.0-Frr[i] << "\t" << 1.0-Far[i] <<"\t"   << FrrC[i] << "\t" << FarC[i] << "\t"   << ndivFar[i] << "\t" << ndivFrr[i] << endl;
      outFarFrr << Cummin+i*step << "\t" << Frr[i] << "\t" << Far[i] << "\t" << 1.0-Frr[i] << "\t" << 1.0-Far[i] <<"\t"   << FrrC[i] << "\t" << FarC[i] << endl;


    outFarFrr << "number match/mismatch " << intra << "/" << inter << endl;

    outFarFrr << "EER[Far/Frr]: " << EER*100.0 << "["<< FarEER*100.0 <<"/" << FrrEER*100.0 << "] confidence on [Far: "<< FarCV << ", Frr: "<<FrrCV << "]"<< endl;

    outFarFrr<< "VR: " << VR*100.0 << " @0.1% Far, confidences on VR: "<< FrrC[pos]*100.0<< endl;

    outFarFrr.close();

    cout << "EER[Far/Frr]: " << EER*100.0 << "["<< FarEER*100.0 <<"/" << FrrEER*100.0 << "] confidences on [Far: "<< FarCV << ", Frr: "<<FrrCV << "]"<< endl;

    cout<< "VR: " << VR*100.0 << " @0.1% Far, confidences on VR: "<< FrrC[pos]*100.0<< endl;

    delete []Frr;

    delete []Far;

    delete []FrrC;

    delete []FarC;

    return 0;

  }
}


float min ( float a,float b )
{
  if ( a<=b )
    return a;
  else
    return b;
}


float max ( float a,float b )
{
  if ( a>=b )
    return a;
  else
    return b;
}


vector < float > loadscores ( char *filename )
{

  string stat;
  char *query,*target;
  query=new char[255];
  target=new char[255];
  float val;
  vector < float > scores;
  ifstream filein ( filename );

  while ( filein.good() ) {
    getline ( filein,stat );

    if ( stat!="" ) {
      sscanf ( stat.c_str(),"%s\t%s\t%f",query,target,&val );
      scores.push_back ( val );
    }
  }

  filein.close();

  delete []query;
  delete []target;
  return scores;
}


// function [Farconf Frrconf]=Inter_Conf(Far,Frr,num_imposteurs_tests,num_clients_tests)
void GetConfidence ( double *Far,double*Frr,int histsize,int numimp,int numclient,double *&FarC,double*&FrrC,double confid )
{

  FarC=new double[histsize];
  FrrC=new double[histsize];

  for ( int i=0;i<histsize;i++ ) {
    FarC[i]=confid*sqrt ( Far[i]* ( 1.0-Far[i] ) /numimp );    // go to zero when num of impostor is high
    FrrC[i]=confid*sqrt ( Frr[i]* ( 1.0-Frr[i] ) /numclient );
  }

}


/*
Compute if mean Far > Frr => Distance mesure
else
   Similarity measure
*/
int Getproblemtype ( vector<float> &inter,vector<float> &intra )
{
  double meaninter=0.0;
  double meanintra=0.0;
  int numimp=inter.size();
  int numclient=intra.size();

  for ( int i=0;i<numimp;i++ )
    meaninter+=inter[i];

  for ( int i=0;i<numclient;i++ )
    meanintra+=intra[i];

  if ( meaninter/numimp > meanintra/numclient )
    return 0;
  else
    return 1;

}

/*

compute the norm deviation
*/
int norm_dev ( double *Fr,int histsize,double *&ndf )
{
//Fr=[0.0--0.1]
  ndf=new double[histsize];
  double *adj_prob=new double[histsize];
  double *R=new double[histsize];
  double SPLIT =  0.42;
  double A0 =   2.5066282388, A1 = -18.6150006252, A2 =  41.3911977353, A3 = -25.4410604963, B1 =  -8.4735109309, B2 =  23.0833674374, B3 = -21.0622410182,
                                   B4 =   3.1308290983, C0 =  -2.7871893113, C1 =  -2.2979647913, C2 =   4.8501412713, C3 =   2.3212127685, D1 =   3.5438892476, D2 =   1.6370678189;


  for ( int i=0;i<histsize;i++ ) {
    if ( Fr[i]<=0.0 )
      Fr[i]= 0.00001;
    else if ( Fr[i]>=1.0 )
      Fr[i]= 0.99999;

    adj_prob[i]=Fr[i]-0.5;

    if ( fabs ( adj_prob[i] ) <= SPLIT ) {
      R[i]=adj_prob[i]*adj_prob[i];
      ndf[i] = adj_prob [ i ] * ( ( ( A3 * R [ i ] + A2 ) * R [ i ] + A1 ) * R [ i ] + A0 );
      ndf[i]  = ndf[i]/ ( ( ( ( B4 * R [ i ] + B3 ) * R [ i ] + B2 ) * R [ i ] + B1 ) * R [ i ] + 1.0 );
    } else {
      R[i]=Fr[i];
      if ( Fr[i]> 0.5 ) {
        R [i] = 1.0 - Fr[i];
        R[i] = sqrt ( ( -1.0 ) * log ( R[i] ) );
        ndf[i] = ( ( ( C3* R [ i ] + C2 ) * R [ i ] + C1 ) * R [ i ] + C0 );
        ndf[i] = ndf[i] / ( ( D2* R [ i ] + D1 ) * R [ i ] + 1.0 );
      } else
        R[i] = sqrt ( ( -1.0 ) * log ( R[i] ) );
      ndf[i] = ( ( ( C3* R [ i ] + C2 ) * R [ i ] + C1 ) * R [ i ] + C0 );
      ndf[i] = ndf[i] / ( ( D2* R [ i ] + D1 ) * R [ i ] + 1.0 );
      ndf[i] = ndf[i] * -1.0;
    }
//cout << Fr[i] << "\t" << ndf[i] << endl;
  }

  delete []adj_prob;
  delete [] R;

  return 0;

}

