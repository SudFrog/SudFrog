/***************************************************************************
 *   Copyright (C) 2009 by Télécom SudParis. Major Developer: Anouar Mellakh   *
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
#include "templator.h"

namespace TMSP_Face_Space
{

  Templator::Templator()
  {
    Problem=loadedGabor=loadedspace=nospace=false;
  }

  Templator::Templator ( const char* SpaceRed )
  {
    Problem=loadedGabor=loadedspace=nospace=false;
    SetSpacefile ( SpaceRed );
  }

  Templator::~Templator()
  {
  }

  void Templator::SetVerbose ( verbose &v )
  {

    Pfc_Gab.SetVerbose ( v );
    Lin.SetVerbose ( v );
    NonLin.SetVerbose ( v );
    verb=v;
  }

  void Templator::SetSpacefile ( const char* SpaceRed )
  {
    string line;
    int size=0,pos1,pos2,pos;

    ifstream outfileMat ( SpaceRed );

    if ( !outfileMat.good() ) {
      cout << "Error Reading " <<  SpaceRed << endl;
      exit ( -1 );
    }
    bool found=false;

    int res,orient,width,height,redcoef,minwave;
    float sigmaOnf,dThetaOnSigma,mult;

    string meth;
    while ( outfileMat.good() ) {
      getline ( outfileMat,line );

      pos = line.find ( "Method" );
      if ( pos != ( int ) string::npos ) {
        outfileMat.close();
        pos1 = line.find ( "\"" );
        pos2 = line.find ( "\"", pos1 + 1 );
        meth = line.substr ( pos1 + 1, pos2 - pos1 - 1 );

        if ( meth=="PCA" || meth=="LDA" || meth=="DLDA" ) {

          Lin.LoadSpace ( ( char* ) SpaceRed );
          Lin.SetProblem ( meth );
          Problem=false;
          found=true;

        } else if ( meth=="GDA"|| meth=="KFA" ) {
          NonLin.LoadSpace ( ( char* ) SpaceRed );
		//cout << "test" << endl;
          NonLin.SetProblem ( meth );
          found=Problem=true;
        } else {
          cerr << "Not known reducing method!" << endl;
          exit ( -1 );
        }

        pos = line.find ( "Gabor" );
        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"",pos+1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          meth= line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          //cout << meth << endl;

          pos = line.find ( "Gwidth" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            width= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor width: " << width<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gheight" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            height= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor height: " << height<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gres" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            res= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor resolution: " << res<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gorient" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            orient= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor orientations: " << orient<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gminwave" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            minwave= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor orientations: " << orient<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gmult" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            mult= atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor orientations: " << orient<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Gsigf" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            sigmaOnf= atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor orientations: " << orient<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "GThetasig" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            dThetaOnSigma= atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor orientations: " << orient<< endl;
            // SetGaborReduction(redcoef);
          }

          pos = line.find ( "Greduction" );
          if ( pos != ( int ) string::npos ) {
            pos1 = line.find ( "\"",pos+1 );
            pos2 = line.find ( "\"", pos1 + 1 );
            redcoef= ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
            //cout << "Gabor Coeff reduction: " << redcoef<< endl;
            //
          }
          strupr ( ( char * ) meth.c_str() );
          if ( meth!="NONE" );
          {
            Pfc_Gab.SetProblem ( meth );
            Pfc_Gab.SetGaborParams ( height, width, res, orient, minwave,  mult, sigmaOnf,dThetaOnSigma );
            SetGaborReduction ( redcoef );
          }
        }

        break;
      } else
        continue;
    }

    if ( found )
      loadedspace=true;
    else {
      cerr << "Unable to determine; if it's linear or not linear problem !" << endl;
      exit ( -1 );
    }
  }

  void Templator::SetMutex ( pthread_mutex_t* t )
  {
    Pfc_Gab.SetMutex ( t );
  }

  void Templator::SetNoSpace()
  {
    nospace=true;
  }


  vector <double> Templator::LoadTemplate ( const char* templ )
  {

    vector <double> LoadedTemplate;
    string line;
    int size=0,pos1,pos2,pos;
    if ( !nospace && !loadedspace ) {
      cerr << "Load space before loading template or use Templator::SetNoSpace() to bypass the parameters verification!" << endl;
      exit ( -1 );
    }

    ifstream outfileMat ( templ );

    if ( !outfileMat.good() ) {
      cout << "Error Reading " <<  templ << endl;
      exit ( -1 );
    }

    while ( outfileMat.good() ) {
      getline ( outfileMat,line );
      if ( !nospace ) {
        pos = line.find ( "Method" );
        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"" );
          pos2 = line.find ( "\"", pos1 + 1 );
          string meth = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          if ( meth=="PCA" )
            assertlinear();
          else
            if ( meth=="LDA" )
              assertlinear();
            else
              if ( meth=="DLDA" )
                assertlinear();
              else
                if ( meth=="GDA" ) {
                  assertnonlinear();
                  GetkernelParam ( line );
                } else
                  if ( meth=="KFA" ) {
                    assertnonlinear();
                    GetkernelParam ( line );
                  }
        }

        else
          continue;
      }

      getline ( outfileMat,line );
      pos = line.find ( "Code_Size" );

      if ( pos != ( int ) string::npos ) {
        pos1 = line.find ( "\"" );
        pos2 = line.find ( "\"", pos1 + 1 );
        size = ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
        if ( size==0 ) {
          cout << "Error Reading " <<  templ << endl;
          exit ( -1 );
        }
        break;
      }
    }

    if ( size==0 ) {
      cout << "Error Reading " <<  templ << " The size of code could not be determined"<< endl;
      exit ( -1 );
    }
    ColumnVector Projection = LoadVectorFromfile ( outfileMat,size );

    for ( int i=1;i<=Projection.Nrows();i++ )
      LoadedTemplate.push_back ( Projection ( i ) );

    Projection.release_and_delete();

    return LoadedTemplate;
  }


// 	vector <double> Templator::Template()
// 	{
// 	}

  void Templator::CreateFilters ( int height, int width,int nscale,int norient,int minWaveLength, float mult,float  sigmaOnf,float dThetaOnSigma )
  {
    Pfc_Gab.SetGaborParams ( height, width, nscale, norient, minWaveLength,  mult, sigmaOnf,dThetaOnSigma );
    loadedGabor=true;
  }


  void Templator::SetMask ( int eyedist )
  {
    Pfc_Gab.SetMask ( eyedist );
  }

  void Templator::SetGaborReduction ( int val )
  {
    Pfc_Gab.SetReduction ( val );
  }

  ReturnMatrix Templator::GetTemplate ()
  {
    return TmpLate;
  };


  void Templator::SaveTemplate ( const char* templ )
  {
    ofstream outfileMat ( templ );
    if ( !Problem )
      outfileMat << "<Method=\"" << Lin.GetProblemName() << "\">" << endl;
    else
      outfileMat << "<Method=\"" << NonLin.GetProblemName() << "\">" << endl;

    outfileMat << "<Code_Size=\"" << TmpLate.size() << "\">" << endl;
    outfileMat << TmpLate << endl;
    outfileMat.close();
  }

  vector <double> Templator::Template ( Pfc_Image &F )
  {

    vector <double> tmplate;
    ColumnVector V ;
    if ( Pfc_Gab.GetMethod() !="NONE" ) {
      vector <double> Gabs;
      Gabs=Pfc_Gab.Gaborating ( F );
      V.ReSize ( Gabs.size() );
      for ( int i=0;i<Gabs.size();i++ )
        V ( i+1 ) =Gabs[i];
      Gabs.clear();
    } else {
      int s=F.GetImSize();
      V.ReSize ( s );
      for ( int i=0;i<s;i++ )
        V ( i+1 ) =F.GetData ( i );
    }

    TmpLate.Release();

    if ( !Problem )
      TmpLate=Lin.Projection ( V );
    else
      TmpLate=NonLin.Projection ( V );

    for ( int i=1;i<=TmpLate.Nrows();i++ )
      tmplate.push_back ( TmpLate ( i ) );

    V.ReleaseAndDelete();
    return tmplate;
  }


  bool Templator::GetProblem()
  {
    if ( Problem )
      verb << "Non-linear problem\n";
    else
      verb << "Linear problem\n";

    return Problem;
  }

  void Templator::SetGaborMethod ( string meth )
  {
    Pfc_Gab.SetProblem ( meth );
  }

  string Templator::GetGaborMethod()
  {
    return Pfc_Gab.GetMethod();
  }

  void Templator::assertlinear()
  {
    if ( Problem ) {
      cerr << "Templator configured for linear projection and template is created from nonlinear projection" << endl;
      exit ( -1 );
    }
  }

  void Templator::assertnonlinear()
  {
    if ( !Problem ) {
      cerr << "Templator configured for nonlinear projection and template is created from linear projection" << endl;
      exit ( -1 );

    }
  }

  void Templator::GetkernelParam ( string line )
  {

    int type=0;
    float param1 = NAN;
    float param2 = NAN;
    int pos1, pos2;
    assertnonlinear();

    int pos = line.find ( "Space_Part" );

    if ( pos != ( int ) string::npos ) {
      pos1 = line.find ( "\"" );
      pos2 = line.find ( "\"", pos1 + 1 );
      string occ = line.substr ( pos1 + 1, pos2 - pos1 - 1 );

      if ( occ == "kernel" ) {
        pos = line.find ( "type" );
        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          type = ( int ) atoi ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
          if ( NonLin.ker.type!=type ) {
            cerr << "Error in kernel type" << endl;
            exit ( -1 );
          }
        }

        pos = line.find ( "params" );
        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          pos = tmp.find ( "," );
          param1 = atof ( tmp.substr ( 0, pos ).c_str() );
          param2 = atof ( tmp.substr ( pos + 1 ).c_str() );

        }
//             //ker.type=type;
        switch ( NonLin.ker.type ) {
          case LINEAR:
            if ( NonLin.ker.linear.alpha!=param1 || NonLin.ker.linear.decal!=param2 ) {
              cerr << "Kernel parameters are different!" << endl;
              exit ( -1 );
            }

            break;
          case POLY:
            if ( NonLin.ker.poly.power!=param1 || NonLin.ker.poly.decal!=param2 ) {
              cerr << "Kernel parameters are different!" << endl;
              exit ( -1 );
            }
            break;
          case RBF:
            if ( NonLin.ker.rbf.power!=param1 ) {
              cerr << "Kernel parameters are different!" << endl;
              exit ( -1 );
            }
            break;
          case SIGMOID:
            if ( NonLin.ker.sigmoid.gamma!=param1 || NonLin.ker.sigmoid.decal!=param2 ) {
              cerr << "Kernel parameters are different!" << endl;
              exit ( -1 );
            }
            break;

        }
        pos = line.find ( "rescale" );
        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          pos = tmp.find ( "," );
          param1 = atof ( tmp.substr ( 0, pos ).c_str() );
          param2 = atof ( tmp.substr ( pos + 1 ).c_str() );
          if ( NonLin.ker.minR!=param1 || NonLin.ker.maxR!=param2 ) {
            cerr << "Kernel parameters are different!" << endl;
            exit ( -1 );
          }
        }
      } else {
        cout << "bad format kernel" << endl;
        exit ( -1 );
      }

    } else {
      cout << "bad format kernel" << endl;
      exit ( -1 );
    }

  }


}
