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
#include "nonlinearreducer.h"

namespace TMSP_Face_Space
{
  NonLinearReducer::NonLinearReducer()
  {
    ker.settype ( LINEAR );
    feedback=true;
    eps=reg;
    SetProblem ( KFA );
  }

  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L )
  {
    labels=L;
    ker.settype ( LINEAR );
    ker.SetData ( A );
    feedback=true;
    SetProblem ( KFA );
    eps=reg;
  }

  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L,kernel &k )
  {
    labels=L;
    ker=k;
    feedback=true;
    ker.SetData ( A );
    SetProblem ( KFA );
    eps=reg;
  }

  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L,kerlin &k )
  {
    SetProblem ( KFA );
    Set ( A,L,k );
    eps=reg;
  }


  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L,kerpoly &k )
  {
    SetProblem ( KFA );
    Set ( A,L,k );
    eps=reg;
  }

  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L,kerrbf &k )
  {
    SetProblem ( KFA );
    Set ( A,L,k );
    eps=reg;
  }

  NonLinearReducer::NonLinearReducer ( Matrix &A,vector<int>& L,kersigmoid &k )
  {
    SetProblem ( KFA );
    Set ( A,L,k );
    eps=reg;
  }

  NonLinearReducer::~NonLinearReducer()
  {
    EigenVectors.ReleaseAndDelete();
    labels.clear();
    bias.ReleaseAndDelete();
  }



  void NonLinearReducer::SetProblem ( NProblem w )
  {
    which=w;

    switch ( which ) {
      case GDA:
        methodname="GDA";
        break;

      case KFA:
        methodname="KFA";
        break;

      default:
        cerr << "Not supported method\n" ;
        break;

    }
  }

  void NonLinearReducer::SetProblem ( string w )
  {


    if ( w=="GDA" ) {
      which=GDA;
      methodname="GDA";
      return;
    }

    if ( w=="KFA" ) {
      which=KFA;
      methodname="KFA";
      return;
    }

    cerr << "Not supported method\n" ;
    exit ( -1 );

  }

  NProblem NonLinearReducer::GetProblem ( )
  {

    return which;
  }
  string NonLinearReducer::GetProblemName ( )
  {
    return methodname;

  }


  void NonLinearReducer::SetKernel ( kernel &k )
  {
    ker=k;
  }

  void NonLinearReducer::SetDatawithLabels ( Matrix &A,vector<int>& L)
{
labels=L;
ker.SetData ( A );
feedback=true;
}

  void  NonLinearReducer::Set ( Matrix &A,vector<int>& L,kerlin &k )
  {
    labels=L;
    ker.setker ( k );
    ker.SetData ( A );
    feedback=true;
  }
  void  NonLinearReducer::Set ( Matrix &A,vector<int>& L,kerpoly &k )
  {
    labels=L;
    ker.setker ( k );
    ker.SetData ( A );
    feedback=true;
  }
  void  NonLinearReducer::Set ( Matrix &A,vector<int>& L,kerrbf &k )
  {
    labels=L;
    ker.setker ( k );
    ker.SetData ( A );
    feedback=true;
  }
  void  NonLinearReducer::Set ( Matrix &A,vector<int>& L,kersigmoid &k )
  {
    labels=L;
    ker.setker ( k );
    ker.SetData ( A );
    feedback=true;
  }

  int NonLinearReducer::ComputeSpace ()
  {

    switch ( which ) {
      case KFA:
        GetKFACompound();
        break;

      case GDA:
        GetGDACompound();
        break;
    }

    return 0;
  }

  int NonLinearReducer::GetGDACompound()
  {
    if ( feedback )
      verb << "Computing kernel\n" ;
    ker.Computekernel();
    if ( feedback )
      verb << "kernel Computed\n" ;

//verb << ker.type<< "\n";
    int num_data=ker.DonneesA.Ncols();

// center Data in the nonlinear space
    if ( feedback )
      verb << "Centring Data in the Non linear Space\n" ;

    Matrix J ( num_data,num_data ),JK,KC;
    J=1.0/num_data;
    JK = J*ker.noyau;
    KC = ker.noyau - JK.t() - JK + JK*J;

    if ( OneisNAN ( KC ) ) {
      verb << "Centred Data containing NAN, \nVerification of kernel parameters must be done, \nComputation stopped.. \n" ;
      exit ( -1 );
    }

//  KC decomposition; Kc = P*Gamma*P'
    DiagonalMatrix Gamma;
    Matrix FirstEigV;
// verb << KC ;
    if ( feedback )
      verb << "Computing Centred Data decomposition\n" ;

    ComputeEigens ( KC,Gamma,FirstEigV );

    if ( feedback )
      verb << "Centred Data decomposition computed\n" ;

//clean FirstP by deleting the low EigenValues;
    int rankKC=0;
    double minEigV = Gamma ( 1,1 ) /1000.0;
    for ( int i=Gamma.Ncols();i>=1;i-- )
      if ( Gamma ( i,i ) >=minEigV ) {
        rankKC=i;
        break;
      }

    FirstEigV<<FirstEigV.Columns ( 1,rankKC );
    Gamma<< Gamma.SubMatrix ( 1,rankKC,1,rankKC );
    Matrix EigVin ( rankKC,rankKC );
    EigVin=0.0;
    for ( int i=1;i<=rankKC;i++ )
      EigVin ( i,i ) =1.0/Gamma ( i,i );

    if ( feedback )
      verb << "Rank of KC: " <<rankKC  <<"\n";

//verb << FirstEigV.Nrows()<< "x"<<FirstEigV.Ncols()  << "\n";

    KC<<FirstEigV*Gamma*FirstEigV.t();

    if ( feedback )
      verb << "Computing diagonal block matrix " <<"\n";
// make diagonal block matrix W
    int classnumber=labels.size();
    Matrix W ( num_data,num_data );
    W=0.0;
    int start,end=0;
    for ( int i=0;i< classnumber;i++ ) {
      start=end+1;                                        //start=end+1
      end=start + labels[i]-1;
      W.SubMatrix ( start,end,start,end ) =1.0/labels[i];
    }

    SpaceDim=min ( rankKC,classnumber-1 );

    DiagonalMatrix Gamma2;
    Matrix SecondEigV;
    Matrix NonLinearS;
    NonLinearS=FirstEigV.t() *W*FirstEigV;
// verb << KC ;
    verb << "Computing second decomposition  " <<"\n";
    ComputeEigens ( NonLinearS,Gamma2,SecondEigV );
    verb << "Second decomposition computed" <<"\n";
    SecondEigV << SecondEigV.Columns ( 1,SpaceDim );

// P*diag(1./Gamma)*Beta;
    EigenVectors=FirstEigV*EigVin*SecondEigV;

    double val;
    for ( int i=1;i<= SpaceDim;i++ )
      EigenVectors.Column ( i ) =EigenVectors.Column ( i ) /
                                 ( sqrt ( ( EigenVectors.Column ( i ).t() *KC*EigenVectors.Column ( i ) ).as_scalar() ) );

// % centering EigenVectors and computing Bias
    if ( feedback )
      verb << "Centring EigenVectors and computing Bias " <<"\n";

    ColumnVector sumK ( num_data );
    for ( int i=1;i<=num_data;i++ )
      sumK ( i ) =ker.noyau.Column ( i ).sum();

    bias.ReSize ( SpaceDim );
    for ( int i=1;i<= SpaceDim;i++ ) {
      val=- ( sumK.t() *EigenVectors.Column ( i ) ).as_scalar() /num_data + EigenVectors.Column ( i ).sum() *sumK.sum() / ( num_data*num_data );
      bias ( i ) =val;
      EigenVectors.Column ( i )-=EigenVectors.Column ( i ).sum() /num_data;
    }
    return ( 0 );
  }



  int NonLinearReducer::GetKFACompound()
  {
    if ( feedback )
      verb << "Computing kernel\n" ;
    ker.Computekernel();
    if ( feedback )
      verb << "kernel Computed\n" ;

//verb << ker.type<< "\n";
    int num_data=ker.DonneesA.Ncols();

// center Data in the nonlinear space
    if ( feedback )
      verb << "Centring Data in the Non linear Space\n" ;

    Matrix J ( num_data,num_data ),JK,KC;
    J=1.0/num_data;
    JK = J*ker.noyau;
    KC = ker.noyau - JK.t() - JK + JK*J;

    if ( OneisNAN ( KC ) ) {
      verb << "Centred Data containing NAN, \nVerification of kernel parameters must be done, \nComputation stopped.. \n" ;
      exit ( -1 );
    }

    if ( feedback )
      verb << "Computing diagonal block matrix " <<"\n";
// make diagonal block matrix W
    int classnumber=labels.size();
    Matrix W ( num_data,num_data );
    W=0.0;
    int start,end=0;
    for ( int i=0;i< classnumber;i++ ) {
      start=end+1;                                        //start=end+1
      end=start + labels[i]-1;
      W.SubMatrix ( start,end,start,end ) =1.0/labels[i];

    }

    Matrix I ( num_data,num_data );
    for ( int i=1;i<=I.Ncols();i++ )
      I ( i,i ) =1;

    Matrix FirstEigV;
// verb << "test1\n" ;
    FirstEigV<< ( KC*KC+eps*I ).i();
// verb << "test2\n" ;
    FirstEigV<<FirstEigV*KC*W*KC;
// verb << "test3\n" ;
    DiagonalMatrix Gamma;
    ComputeEigens ( FirstEigV,Gamma,EigenVectors );
// verb << "test4\n" ;

    int rankKC;
    double minEigV = Gamma ( 1,1 ) /1000.0;
    for ( int i=Gamma.Ncols();i>=1;i-- )
      if ( Gamma ( i,i ) >=minEigV ) {
        rankKC=i;
        break;
      }
    EigenVectors<<EigenVectors.Columns ( 1,rankKC );
    Gamma<< Gamma.SubMatrix ( 1,rankKC,1,rankKC );
    SpaceDim=min ( rankKC,classnumber-1 );

    if ( feedback )
      verb << "Rank of KFA Space: " <<rankKC  <<"\n";

    double val;
    for ( int i=1;i<= SpaceDim;i++ )
      EigenVectors.Column ( i ) =EigenVectors.Column ( i ) /
                                 ( sqrt ( ( EigenVectors.Column ( i ).t() *KC*EigenVectors.Column ( i ) ).as_scalar() ) );

// % centering EigenVectors and computing Bias
    if ( feedback )
      verb << "Centring EigenVectors and computing Bias " <<"\n";

    ColumnVector sumK ( num_data );
    for ( int i=1;i<=num_data;i++ )
      sumK ( i ) =ker.noyau.Column ( i ).sum();

    bias.ReSize ( SpaceDim );
    for ( int i=1;i<= SpaceDim;i++ ) {
      val=- ( sumK.t() *EigenVectors.Column ( i ) ).as_scalar() /num_data + EigenVectors.Column ( i ).sum() *sumK.sum() / ( num_data*num_data );
      bias ( i ) =val;
      EigenVectors.Column ( i )-=EigenVectors.Column ( i ).sum() /num_data;
    }
    return ( 0 );
  }


  ReturnMatrix NonLinearReducer::Projection ( GeneralMatrix &Datain )
  {
    kernel kerp;

//kerp.setparam(Datain,Data,k);
    kerp=ker;
//verb << "done\n" ;
    kerp.SetData ( ker.DonneesA,Datain );
    Matrix Proj;

// Y(:,i) = Alpha' * kernel(sv.X, X(:,i), ker, arg) + b
    kerp.Computekernel();

//cout << "Eigens " << EigenVectors.Nrows() << "\t" << EigenVectors.Ncols << endl;
//cout << "Noyau " << kerp.noyau.Nrows() << "\t" << kerp.noyau.Ncols() << endl;

    Proj << (kerp.noyau.t()*EigenVectors).t();

    for ( int j = 1; j <= SpaceDim; j++ )
      Proj.row ( j ) +=bias ( j );                        //correct for the centering

    Proj.Release();
    return Proj;
  }

  void NonLinearReducer::SaveSpace ( char * filename ,GaborParams &params )
  {
    ofstream outfileMat;
    outfileMat.open ( filename );
    if ( outfileMat.good() ) {

      outfileMat << "<Method=\""<< methodname << "\" Gabor=\""<< params.methodname<<"\" Gwidth=\""<<params.width<<"\" Gheight=\""<<params.height<<"\" Gres=\""<<params.nscale<<"\" Gorient=\""<<params.norient <<"\" Gminwave=\""<<params.minWaveLength <<"\" Gmult=\""<<params.mult <<"\" Gsigf=\""<< params.sigmaOnf<<"\" GThetasig=\""<< params.dThetaOnSigma<<"\" Greduction=\""<< params.reductcoef<<"\" >"<< "\n";

      switch ( ker.type ) {
          /* linear kernel */
        case LINEAR:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\""<< ker.linear.alpha<<","<< ker.linear.decal<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">\n" ;
          break;
          /* polynomial kernel */
        case POLY:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.poly.power<<","<< ker.poly.decal<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">\n" ;
          break;
          /* radial basis functions kernel*/
        case RBF:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.rbf.power<< ",0\""<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">"<< "\n";
          break;
          /* sigmoid */
        case SIGMOID:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.sigmoid.gamma<<","<< ker.sigmoid.decal<<  "\" rescale=\""<< ker.minR << ","<< ker.maxR<< "\">\n" ;
          break;

      }
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"Data\" Size=\"" << ker.DonneesA.Nrows() << ","<<ker.DonneesA.Ncols() << "\">\n" ;
      outfileMat << ker.DonneesA;
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"Bias\" Size=\"" << bias.Nrows() << "\">\n" ;
      outfileMat << bias;
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"EigenVectors\" Size=\"" << EigenVectors.Nrows() << ","
      <<  EigenVectors.Ncols() << "\">\n" ;
      outfileMat << EigenVectors;
      outfileMat.close();
    } else {
      verb << "Error Saving NonLinear Space\n" ;
      exit ( -1 );
    }
  }


  void NonLinearReducer::SaveSpace ( char * filename )
  {
    ofstream outfileMat;
    outfileMat.open ( filename );
    if ( outfileMat.good() ) {

      switch ( ker.type ) {
          /* linear kernel */
        case LINEAR:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\""<< ker.linear.alpha<<","<< ker.linear.decal<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">\n" ;
          break;
          /* polynomial kernel */
        case POLY:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.poly.power<<","<< ker.poly.decal<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">\n" ;
          break;
          /* radial basis functions kernel*/
        case RBF:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.rbf.power<< ",0\""<< "\" rescale=\""<< ker.minR<<","<< ker.maxR<< "\">"<< "\n";
          break;
          /* sigmoid */
        case SIGMOID:
          outfileMat << "<Space_Part=\"kernel\" type=\"" << ker.type << "\" params=\"" << ker.sigmoid.gamma<<","<< ker.sigmoid.decal<<  "\" rescale=\""<< ker.minR << ","<< ker.maxR<< "\">\n" ;
          break;

      }
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"Data\" Size=\"" << ker.DonneesA.Nrows() << ","<<ker.DonneesA.Ncols() << "\">\n" ;
      outfileMat << ker.DonneesA;
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"Bias\" Size=\"" << bias.Nrows() << "\">\n" ;
      outfileMat << bias;
      outfileMat << "\n";

      outfileMat << "<Space_Part=\"EigenVectors\" Size=\"" << EigenVectors.Nrows() << ","
      <<  EigenVectors.Ncols() << "\">\n" ;
      outfileMat << EigenVectors;
      outfileMat.close();
    } else {
      verb << "Error Saving NonLinear Space\n" ;
      exit ( -1 );
    }
  }


  void NonLinearReducer::LoadSpace ( const char * filename )
  {

    string line;
    int size1, size2;
    string part;
    ifstream outfileMat;
    outfileMat.open ( filename );
//Data.ReleaseAndDelete();
    bias.ReleaseAndDelete();
    EigenVectors.ReleaseAndDelete();
    if ( outfileMat.good() ) {
      while ( outfileMat.good() ) {
	
        getline ( outfileMat, line );
        part = GetSpacePart ( line, size1, size2 );
        if ( part != "none" && part != "unknown" ) {
          if ( part == "kernel" )
            GetkernelPart ( line );
          else if ( part == "Data" )
            ker.DonneesA << LoadMatrixFromfile ( outfileMat, size1, size2 );
          else if ( part == "Bias" )
            bias << LoadVectorFromfile ( outfileMat, size1 );
          else if ( part == "EigenVectors" )
            EigenVectors << LoadMatrixFromfile ( outfileMat, size1, size2 );

	//cout << part << "\t" << size1 << "\t" << size2 << endl;

        }
//
      }


      SpaceDim=EigenVectors.Ncols();
// 	verb << SpaceDim << "\n";
      outfileMat.close();

    } else {
      verb << filename << " not found\n" ;
      exit ( -1 );
    }

  }

  void NonLinearReducer::SetkernelPartFromline ( string line )
  {
    int type=0;
    float param1 = NAN;
    float param2 = NAN;
    int pos1, pos2;
    int pos = line.find ( "type" );
    if ( pos != ( int ) string::npos ) {
      pos1 = line.find ( "=", pos + 1 );
      pos2 = line.find ( ";", pos1 + 1 );
      type = ( int ) atoi ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
    }

    pos = line.find ( "params" );
    if ( pos != ( int ) string::npos ) {
      pos1 = line.find ( "=", pos + 1 );
      pos2 = line.find ( ":", pos1 + 1 );
      string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
      pos = tmp.find ( "," );
      param1 = atof ( tmp.substr ( 0, pos ).c_str() );
      param2 = atof ( tmp.substr ( pos + 1 ).c_str() );

    }

    ker.type=type;
    switch ( ker.type ) {
      case LINEAR:
        ker.linear.alpha=param1;
        ker.linear.decal=param2;
        verb << "Linear kernel alpha:" << ker.linear.alpha << " decal:"<< ker.linear.decal<<  "\n";
        break;
      case POLY:
        ker.poly.power=param1;
        ker.poly.decal=param2;
        verb << "Polynomial kernel power:" << ker.poly.power << " decal:"<< ker.poly.decal<<  "\n";
        break;
      case RBF:
        ker.rbf.power=param1;
        verb << "Radial kernel power:" << ker.rbf.power <<  "\n";
        break;
      case SIGMOID:
        ker.sigmoid.gamma=param1;
        ker.sigmoid.decal=param2;
        verb << "Sigmoid kernel gamma:" << ker.sigmoid.gamma << " decal:"<< ker.sigmoid.decal <<  "\n";
        break;

    }
  }

  void NonLinearReducer::GetkernelPart ( string line )
  {
    int type=0;
    float param1 = NAN;
    float param2 = NAN;
    int pos1, pos2;
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
        ker.type=type;
        switch ( ker.type ) {
          case LINEAR:
            ker.linear.alpha=param1;
            ker.linear.decal=param2;
            verb << "Linear kernel alpha:" << ker.linear.alpha << " decal:"<< ker.linear.decal<<  "\n";
            break;
          case POLY:
            ker.poly.power=param1;
            ker.poly.decal=param2;
            verb << "Polynomial kernel power:" << ker.poly.power << " decal:"<< ker.poly.decal<<  "\n";
            break;
          case RBF:
            ker.rbf.power=param1;
            verb << "Radial kernel power:" << ker.rbf.power <<  "\n";
            break;
          case SIGMOID:
            ker.sigmoid.gamma=param1;
            ker.sigmoid.decal=param2;
            verb << "Sigmoid kernel gamma:" << ker.sigmoid.gamma << " decal:"<< ker.sigmoid.decal <<  "\n";
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
          ker.minR=param1;
          ker.maxR=param2;
          verb << "using (" << ker.minR << " , " << ker.maxR << ") for rescaling\n" ;
        }
      } else {
        verb << "bad format kernel\n" ;
        exit ( -1 );
      }

    } else {
      verb << "bad format kernel\n" ;
      exit ( -1 );
    }

  }


    string NonLinearReducer::GetSpacePart ( string line, int & size1, int & size2 )
  {
    size1 = 0;
    size2 = 0;
    int pos1, pos2;
    int pos = line.find ( "Space_Part" );

    if ( pos != ( int ) string::npos ) {
      pos1 = line.find ( "\"" );
      pos2 = line.find ( "\"", pos1 + 1 );
      string occ = line.substr ( pos1 + 1, pos2 - pos1 - 1 );

      if ( occ == "kernel" ) {
        
        return occ;

      } else if ( occ == "Data" ) {
        pos = line.find ( "Size" );

        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          pos = tmp.find ( "," );
          size1 = ( int ) atof ( tmp.substr ( 0, pos ).c_str() );
          size2 = ( int ) atof ( tmp.substr ( pos + 1 ).c_str() );
        }

        return occ;

      } else if ( occ == "Bias" ) {
        pos = line.find ( "Size" );

        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          size1 = ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
        }
        return occ;

      } else if ( occ == "EigenVectors" ) {
        pos = line.find ( "Size" );

        if ( pos != ( int ) string::npos ) {
          pos1 = line.find ( "\"", pos + 1 );
          pos2 = line.find ( "\"", pos1 + 1 );
          string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
          pos = tmp.find ( "," );
          size1 = ( int ) atof ( tmp.substr ( 0, pos ).c_str() );
          size2 = ( int ) atof ( tmp.substr ( pos + 1 ).c_str() );
        }

        return occ;

      } else
        return "unknown";

    } else
      return "none";

  }

  void NonLinearReducer::SetVerbose ( verbose &v )
  {
    verb=v;
  }


}
