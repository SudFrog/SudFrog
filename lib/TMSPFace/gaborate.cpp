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
#include "gaborate.h"

#ifndef pi
#define pi 3.14159265
#endif
namespace TMSP_Face_Space
{


 

  void Gaborate::init()
  {

//Original=NULL;
    parameters.height=128;
    parameters.width=128;
    parameters.nscale=5 ;
    parameters.norient=8 ;
    parameters.minWaveLength=8;
    parameters.mult=2  ;
    parameters.sigmaOnf=0.65 ;
    parameters.dThetaOnSigma=1.5;
    mutexout=NULL;
    usePhase=useReal=useImag=applymask=FirstFace=false;
    SetReduction ( 8 );
    useMag=true;
    Filters.clear();
    SetProblem ( "MAGN" );
   
  }

  Gaborate::Gaborate()
  {
    init();
    
  }

  Gaborate::Gaborate ( GaborParams params )
  {
    init();
    SetGaborParams ( params );
  }

  Gaborate::Gaborate ( GaborParams params,const char *  problem )
  {
    init();
    SetGaborParams ( params );
    SetProblem ( string ( problem ) );
  }

  Gaborate::~Gaborate()
  {

    if (!Filters.empty())
       for ( int o=0;o<parameters.norient;o++ )
         for ( int s=0;s<parameters.nscale;s++ )
          if (Filters[o][s]) 
           delete []Filters[o][s];

    Filters.clear();

    if ( FirstFace ) {
      fftw_destroy_plan ( plan_forward );
      fftw_destroy_plan ( plan_backward );
      fftw_free ( Original );
      fftw_free ( FFTFace );
      fftw_free ( IFFTFace );
      fftw_free ( Reconst );
    }

  }


  void Gaborate::SetProblem ( string P )
  {
    useMag=usePhase=useReal=useImag=false;
    strupr ( ( char * ) P.c_str() );
    parameters.methodname="";
    if ( P=="NONE" ) {
      useMag=usePhase=useReal=useImag=false;
      parameters.methodname="NONE";
      return;
    } else if ( P=="ALL" ) {
      useMag=usePhase=useReal=useImag=true;
      parameters.methodname="ALL";
    } else {
      if ( P.find ( "REAL" ) !=string::npos ) {
        parameters.methodname+="REAL";
        useReal=true;
      }

      if ( P.find ( "IMAG" ) !=string::npos ) {
        if ( parameters.methodname!="" )
          parameters.methodname+="-IMAG";
        else
          parameters.methodname+="IMAG";

        useImag=true;
      }
      if ( P.find ( "MAGN" ) !=string::npos ) {

        if ( parameters.methodname!="" )
          parameters.methodname+="-MAGN";
        else
          parameters.methodname+="MAGN";

        useMag=true;
      }
      if ( P.find ( "PHASE" ) !=string::npos ) {
        if ( parameters.methodname!="" )
          parameters.methodname+="-PHASE";
        else
          parameters.methodname+="PHASE";
        usePhase=true;
      }

    }
    if ( !useReal && !useImag && !useMag && !usePhase ) {
      cerr << "Error: Unknown Gabor mixing method: " << P << endl;
      cerr << "Use a single or combination of the following methods :" << endl << "  REAL, IMAG, MAGN, PHASE" << endl;
      exit ( -1 );
    }
  }

  void Gaborate::SetGaborParams ( int height, int width,int nscale,int norient,int minWaveLength, float mult,float  sigmaOnf,float dThetaOnSigma )
  {
    parameters.height=height;
    parameters.width=width;
    parameters.nscale=nscale ;
    parameters.norient=norient ;
    parameters.minWaveLength=minWaveLength;
    parameters.mult=mult  ;
    parameters.sigmaOnf=sigmaOnf ;
    parameters.dThetaOnSigma=dThetaOnSigma;
// don't need to recreate filters once you initiliaze the class
    CreateFFTGaborFilters();

  }

  void Gaborate::SetGaborParams ( GaborParams params )
  {
    SetGaborParams ( params.height, params.width,
                     params.nscale,params.norient,
                     params.minWaveLength, params.mult,params.
                     sigmaOnf,params.dThetaOnSigma );
  }

  void Gaborate::SetMutex ( pthread_mutex_t *m )
  {
    mutexout=m;
  }


  int Gaborate::SetReduction ( int val )
  {
    parameters.reductcoef=val;
    Features.clear();
  }

  void Gaborate::CreateFFTGaborFilters ( GaborParams params )
  {
    SetGaborParams ( params );
  }

  void Gaborate::CreateFFTGaborFilters ()
  {
    vector < double *> filtreor;

    double thetaSigma;
    float wavelength;
    double *x,*y,*radius,*theta,*sintheta,*costheta,*lowp;
    int size=parameters.height*parameters.width;
    x=new double[ parameters.height*parameters.width];
    y=new double[ parameters.height*parameters.width];
    radius=new double[ parameters.height*parameters.width];
    theta=new double[ parameters.height*parameters.width];
    sintheta=new double[ parameters.height*parameters.width];
    costheta=new double[ parameters.height*parameters.width];
    lowp=new double[ parameters.height*parameters.width];

    Filters.clear();

    verb << "Creation of Gabor Filters with :\n";
    verb << "\t width:" << parameters.width << "\n";
    verb << "\t height:" << parameters.height << "\n";
    verb << "\t scales:" << parameters.nscale << "\n";
    verb << "\t orientations:" << parameters.norient << "\n";
    verb << "\t minwave length:" << parameters.minWaveLength << "\n";
    verb << "\t mult:" << parameters.mult << "\n";
    verb << "\t sigmaOnf:" << parameters.sigmaOnf << "\n";
    verb << "\t dThetaOnSigma:" << parameters.dThetaOnSigma << "\n";


    int halfw,width,halfh,height;;

    if ( fmod ( ( double ) parameters.width,2.0 ) ==0.0 ) {
      halfw = ( int ) ( parameters.width-1 ) /2;
      width=parameters.width-1;

    } else {
      halfw = ( int ) parameters.width /2;
      width=parameters.width;
    }

    if ( fmod ( ( double ) parameters.height,2.0 ) ) {
      halfh = ( int ) ( parameters.height-1 ) /2;
      height=parameters.height-1;

    } else {
      halfh = ( int ) parameters.height /2;
      height=parameters.height;
    }

    for ( int l=0;l<parameters.width;l++ ) {
      float val= ( float ) ( -width+halfw+l ) /width;

      for ( int h=0;h<parameters.height;h++ )
        x[l+h*parameters.width]=val;
    }

    for ( int h=0;h<parameters.height;h++ ) {
      float val= ( float ) ( -height+halfh+h ) /height;

      for ( int l=0;l<parameters.width;l++ )
        y[l+h*parameters.width]=val;

    }

    double cutoff=0.45;

    double shapness=15;

    for ( int l=0;l<size;l++ ) {
      radius[l]=sqrt ( x[l]*x[l]+y[l]*y[l] );
      theta[l] = atan2 ( -y[l],x[l] );
      sintheta[l] = sin ( theta[l] );
      costheta[l] = cos ( theta[l] );
      lowp[l]= 1.0 / ( 1.0 + pow ( ( radius[l]/cutoff ),2*shapness ) );
    }

    delete []x;

    delete []theta;
    delete []y;

    radius[halfw*parameters.width+halfh]=1.0;
    thetaSigma = ( pi/parameters.norient ) /parameters.dThetaOnSigma;

    double logsigmaOnf=log ( parameters.sigmaOnf ) *log ( parameters.sigmaOnf );

    for ( int o=0;o<parameters.norient;o++ ) {

      float angl = o*pi/parameters.norient;
      //cout << angl << endl;
      wavelength = parameters.minWaveLength;
      double *ds=new double[size];
      double *dc=new double[size];
      double *dtheta=new double[size];
      double *spread=new double[size];

      for ( int l=0;l<size;l++ ) {
        ds[l] = sintheta[l] * cos ( angl ) - costheta[l] * sin ( angl );     // Difference in sine.
        dc[l] = costheta[l] * cos ( angl ) + sintheta[l] * sin ( angl );     // Difference in cosine.
        dtheta[l] = fabs ( atan2 ( ds[l],dc[l] ) );                     // Absolute angular distance.
        spread[l] = exp ( ( -dtheta[l]*dtheta[l] ) / ( 2.0 * thetaSigma*thetaSigma ) );      // Calculate the angular filter component.
      }

      for ( int s=0;s<parameters.nscale;s++ ) {
        double *filtre=NULL;
        filtre=new double[ parameters.height*parameters.width];

        double fo = 1.0/wavelength;

        for ( int l=0;l<size;l++ )
          filtre[l]=exp ( ( - ( log ( radius[l]/fo ) ) * ( log ( radius[l]/fo ) ) ) / ( 2.0 * logsigmaOnf ) ) *spread[l]*lowp[l];

        filtre[halfw*parameters.width+halfh]=0.0;

        fftshift2D ( filtre,filtre,parameters.height,parameters.width );

        filtreor.push_back ( filtre );
        //  delete []filtre;
        wavelength *= parameters.mult;
      }

      Filters.push_back ( filtreor );

      filtreor.clear();
      delete[]ds;
      delete[]dc;
      delete[]dtheta;
      delete[]spread;

    }

    delete []radius;
    delete []lowp;
    delete []sintheta,
    delete []costheta;
  }

  void Gaborate::SetFace ( uint8_t *data,int rows,int cols )
  {
    if ( rows!=parameters.height || cols!=parameters.width ) {
      cerr << "*Error dimension mismatch" << endl;
      exit ( -1 );
    }

    int size=rows*cols;

    if ( !FirstFace ) {

      FirstFace=true;
      Original= ( fftw_complex* ) fftw_malloc ( size*sizeof ( fftw_complex ) );
      FFTFace= ( fftw_complex * ) malloc ( size*sizeof ( fftw_complex ) );
      IFFTFace= ( fftw_complex * ) malloc ( size*sizeof ( fftw_complex ) );
      Reconst= ( fftw_complex * ) malloc ( size*sizeof ( fftw_complex ) );

      if ( mutexout )
        pthread_mutex_lock ( & ( *mutexout ) );

      plan_forward=fftw_plan_dft_2d ( parameters.height,parameters.width,
                                      Original,FFTFace, FFTW_FORWARD,FFTW_ESTIMATE );
      plan_backward=fftw_plan_dft_2d ( parameters.height,parameters.width,
                                       Reconst, IFFTFace,FFTW_BACKWARD,FFTW_ESTIMATE );
      if ( mutexout )
        pthread_mutex_unlock ( & ( *mutexout ) );
    }

    for ( int i=0;i<size;i++ ) {
      Original[i][0]= ( double ) data[i];
      Original[i][1]= 0.0;
    }

    fftw_execute ( plan_forward );
  }

  void Gaborate::SetFace ( Pfc_Image &im )
  {
    SetFace ( im.GetDataptr(),im.GetImHeight(),im.GetImWidth() );
  }

  vector <double>  Gaborate::Gaborating ( Pfc_Image &im )
  {
    vector <double> tmp;
    SetFace ( im );
    tmp=Gaborating();
    //return Gaborating();
    return tmp;
  }

  void Gaborate::SetVerbose ( verbose &v )
  {
    verb=v;
  }

  vector <double>  Gaborate::Gaborating ( uint8_t *data,int rows,int cols )
  {
    SetFace ( data,rows,cols );
    return Gaborating();
  }

  vector <double> Gaborate::Gaborating()
  {
    Features.clear();

    if ( !FirstFace ) {
      verb << "@Gaborator::Gaborate(): Original face not yet defined\n";
      return Features;
    }

    int size=parameters.height*parameters.width;

    vector <double *> realvect,imvect,magnitude;
    double *Real,*Imag;
    double *RealRed,*ImagRed;
    Real=Imag=RealRed=ImagRed=NULL;
    int heightout= ( int ) parameters.height/parameters.reductcoef;
    int widthout= ( int ) parameters.width/parameters.reductcoef;
    int redsize=heightout*widthout;

    for ( int o=0;o<parameters.norient;o++ )
      for ( int s=0;s<parameters.nscale;s++ ) {
        Real=new double[size];
        Imag=new double[size];

        if ( parameters.reductcoef>1 ) {
          RealRed=new double[heightout*widthout ];
          ImagRed=new double[heightout*widthout ];
        }

        for ( int l=0;l<size;l++ ) {
          Reconst[l][0]=Filters[o][s][l]*FFTFace[l][0];
          Reconst[l][1]=Filters[o][s][l]*FFTFace[l][1];
        }

        fftw_execute ( plan_backward );

        int count=0;
        double meanR,meanI,meanM,stdR,stdI,stdM;
        meanR=meanI=stdR=stdI=0.0;

        for ( int h=0;h<parameters.height;h++ )
          for ( int l=0;l<parameters.width;l++ ) {
            if ( applymask && !GabMask.Ismask ( l+1,h-2 ) ) {
              count++;
              continue;
            } else {
              Real[count]= ( IFFTFace[count][0] ) /size;
              meanR+=Real[count];
              stdR+=Real[count]*Real[count];
              Imag[count]= ( IFFTFace[count][1] ) /size;
              meanI+=Imag[count];
              stdI+=Imag[count]*Imag[count];
            }
            count++;
          }

        meanR/=count;
        stdR=sqrt ( stdR/count - meanR*meanR );
        meanI/=count;
        stdI=sqrt ( stdI/count - meanI*meanI );

//center features and apply mask
        count=0;
        for ( int h=0;h<parameters.height;h++ )
          for ( int l=0;l<parameters.width;l++ ) {
            if ( applymask && !GabMask.Ismask ( l+1,h-2 ) ) {
              Real[count]=0.0;
              Imag[count]=0.0;
            } else {
              Real[count]= ( Real[count]-meanR ) /stdR;
              Imag[count]= ( Imag[count]-meanI ) /stdI;
            }
            count++;
          }

        int dscale=0;
        if ( parameters.reductcoef>1 ) {
          for ( int h=0;h<heightout;h++ )
            for ( int l=0;l<widthout;l++ )
              RealRed [ h*widthout+l ] =interpData ( Real,parameters.width,parameters.height, ( h*parameters.reductcoef )- dscale,
                                                     ( h*parameters.reductcoef ) + dscale, ( l*parameters.reductcoef )- dscale,
                                                     ( l*parameters.reductcoef ) + dscale );

          delete []Real;
          realvect.push_back ( RealRed );

          for ( int h=0;h<heightout;h++ )
            for ( int l=0;l<widthout;l++ )
              ImagRed [ h*widthout+l ] =interpData ( Imag,parameters.width,parameters.height, ( h*parameters.reductcoef )- dscale,
                                                     ( h*parameters.reductcoef ) + dscale, ( l*parameters.reductcoef )- dscale,
                                                     ( l*parameters.reductcoef ) + dscale );
          delete []Imag;
          imvect.push_back ( ImagRed );

        } else {
          realvect.push_back ( Real );
          imvect.push_back ( Imag );
        }
      }

    if ( useReal )
      for ( int i=0;i<realvect.size();i++ )
        for ( int l=0;l<redsize;l++ )
          Features.push_back ( fabs ( realvect[i][l] ) );

    if ( useImag )
      for ( int i=0;i<imvect.size();i++ )
        for ( int l=0;l<redsize;l++ )
          Features.push_back ( fabs ( imvect[i][l] ) );

    if ( useMag )
      for ( int i=0;i<realvect.size();i++ )
        for ( int l=0;l<redsize;l++ )
          Features.push_back ( sqrt ( realvect[i][l]*realvect[i][l] + imvect[i][l]*imvect[i][l] ) );

    if ( usePhase )
      for ( int i=0;i<realvect.size();i++ )
        for ( int l=0;l<redsize;l++ )
          if ( fabs ( imvect[i][l] ) *fabs ( realvect[i][l] ) >0 )
            Features.push_back ( atan2 ( fabs ( realvect[i][l] ),fabs ( imvect[i][l] ) ) );
          else
            Features.push_back ( 0 );


    for ( int i=0;i<realvect.size();i++ )
      delete []realvect[i];

    for ( int i=0;i<imvect.size();i++ )
      delete []imvect[i];

    realvect.clear();
    imvect.clear();

    return Features;
  }


  int Gaborate::GetFeaturesize()
  {

    int featuresize=0;
    int fsize=parameters.nscale*parameters.norient* ( ( int ) parameters.width/parameters.reductcoef ) * ( ( int ) parameters.height/parameters.reductcoef );

    if ( useReal )
      featuresize+=fsize;

    if ( useImag )
      featuresize+=fsize;

    if ( useMag )
      featuresize+=fsize;

    if ( usePhase )
      featuresize+=fsize;

    return featuresize;
  }

  string Gaborate::GetMethod()
  {
    return parameters.methodname;
  }

  vector <double> Gaborate::GetGaborFeatures()
  {
    return Features;
  }

  void Gaborate::SetMask ( int disteye )
  {
    applymask=true;
    GabMask.Init ( disteye );
  }

  void Gaborate::UnSetMask()
  {
    applymask=false;
  }


  int  Gaborate::SaveFeatures ( const char* filename )
  {
    ofstream output ( filename );

    if ( !output.good() ) {
      cerr << "*Error opening file for saving Gabor features"<<endl;
      return -1;
    }
    // int fsize=parameters.nscale*parameters.norient* ( ( int ) parameters.width/parameters.reductcoef ) * ( ( int ) parameters.height/parameters.reductcoef );
    int h= ( int ) parameters.height/parameters.reductcoef;
    int w= ( int ) parameters.width/parameters.reductcoef;
    int k=0;
    if ( useReal ) {
      output << "<REAL>" << endl;
      for ( int o=0;o<parameters.norient;o++ )
        for ( int s=0;s<parameters.nscale;s++ ) {
          output << "o=" << o << " x s=" << s << endl;
          for ( int i=0;i<h;i++ ) {
            for ( int j=0;j<w;j++ ) {
              output << Features[k] << "\t";
              k++;
            }
            output << endl;
          }
          output << endl;
        }
    }

    if ( useImag ) {
      output << "<IMAG>" << endl;
      for ( int o=0;o<parameters.norient;o++ )
        for ( int s=0;s<parameters.nscale;s++ ) {
          output << "o=" << o << " x s=" << s << endl;
          for ( int i=0;i<h;i++ ) {
            for ( int j=0;j<w;j++ ) {
              output << Features[k] << "\t";
              k++;
            }
            output << endl;
          }
          output << endl;
        }
    }

    if ( useMag ) {
      output << "<MAGN>" << endl;
      for ( int o=0;o<parameters.norient;o++ )
        for ( int s=0;s<parameters.nscale;s++ ) {
          output << "o=" << o << " x s=" << s << endl;
          for ( int i=0;i<h;i++ ) {
            for ( int j=0;j<w;j++ ) {
              output << Features[k] << "\t";
              k++;
            }
            output << endl;
          }
          output << endl;
        }
    }

    if ( usePhase ) {
      output << "<PHASE>" << endl;
      for ( int o=0;o<parameters.norient;o++ )
        for ( int s=0;s<parameters.nscale;s++ ) {
          output << "o=" << o << " x s=" << s << endl;
          for ( int i=0;i<h;i++ ) {
            for ( int j=0;j<w;j++ ) {
              output << Features[k] << "\t";
              k++;
            }
            output << endl;
          }
          output << endl;
        }
    }

    output.close();
    return 0;
  }



  int  Gaborate::SaveFilters ( const char* filename )
  {



    for ( int i=0;i<parameters.norient;i++ )
      for ( int j=0;j<parameters.nscale;j++ ) {
        ostringstream name;
        name << filename << i << "x" << j << ".txt";
        ofstream output ( name.str().c_str() );

        if ( !output.good() ) {
          cerr << "*Error opening file for saving Gabor filters"<<i << j<< endl;
          return -1;
        }

        verb << name.str() << "\n";

        int count=0;

        for ( int h=0;h<parameters.height;h++ ) {
          for ( int l=0;l<parameters.width;l++ ) {
            output << Filters[i][j][count] << "\t";
            count++;
          }

          output << endl;
        }

        output.close();
      }

    return 0;
  }

 

}
