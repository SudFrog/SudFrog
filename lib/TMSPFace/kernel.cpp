#include "kernel.h"
// #include "Matrix_util.h"
/* -------------------------------------------------------------------
 Computes dot product of a-th and b-th vector.
 c = (a)'*(b)
------------------------------------------------------------------- */
namespace TMSP_Face_Space
{
  kernel::kernel()
  {
    settype ( LINEAR );
    linear.decal=0;
    linear.alpha=1;

    poly.decal=NAN;
    poly.power=NAN;
    rbf.power=NAN;

    sigmoid.decal=NAN;
    sigmoid.gamma=NAN;
    Setmaxmin ( NAN,NAN );
// 	    cout << maxR << " " << minR << endl;
  }


  kernel::kernel ( GeneralMatrix &Data )                      //linear
  {
    SetData ( Data );
    settype ( LINEAR );
    linear.decal=0;
    linear.alpha=1;
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data,kerlin k )             //poly
  {
    SetData ( Data );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data,kerpoly k )            //poly
  {
    SetData ( Data );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data,kerrbf k )             //rbf
  {
    SetData ( Data );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data,kersigmoid k )         //sigmoid
  {
    SetData ( Data );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2 ) //linear
  {
    SetData ( Data1,Data2 );
    settype ( LINEAR );
    linear.decal=0;
    linear.alpha=1;
    Setmaxmin ( NAN,NAN );
  }

  kernel::kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerlin k )
  {
    SetData ( Data1,Data2 );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }
//poly
  kernel::kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerpoly k )
  {
    SetData ( Data1,Data2 );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

//rbf
  kernel::kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerrbf k )
  {
    SetData ( Data1,Data2 );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

//sigmoid
  kernel::kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kersigmoid k )
  {
    SetData ( Data1,Data2 );
    setker ( k );
    Setmaxmin ( NAN,NAN );
  }

  kernel::~kernel()
  {
    noyau.ReleaseAndDelete();
    DonneesA.ReleaseAndDelete();
    DonneesB.ReleaseAndDelete();
  }

  void kernel::SetData ( const Matrix &Data )
  {
    prob=ONE;
    DonneesA.Release();
    DonneesB.Release();
    DonneesA<<Data;
  }

  void kernel::SetData ( const Matrix &Data1,const Matrix &Data2 )
  {
    SetFData ( Data1 );
    SetSData ( Data2 );
  }

  void kernel::SetFData ( const Matrix &Data1 )
  {
    prob=TWO;
    DonneesA.Release();
    DonneesA<<Data1;
  }

  void kernel::SetSData ( const Matrix &Data1 )               // save time for multiple projection
  {
    prob=TWO;
    DonneesB.Release();
    DonneesB<<Data1;
  }

  void kernel::settype ( int T )
  {
    type=T;
  }

  void kernel::setker ( kerlin k )
  {
    settype ( LINEAR );
    linear=k;
// 	    cout << poly.power << endl;
  }

  void kernel::setker ( kerpoly k )
  {
    settype ( POLY );
    poly=k;
// 	    cout << poly.power << endl;
  }

  void kernel::setker ( kerrbf k )
  {
    settype ( RBF );
    rbf=k;
  };

  void kernel::setker ( kersigmoid k )
  {
    settype ( SIGMOID );
    sigmoid=k;
  }


  double kernel::dot_prod ( long a, long b )
  {
    double c = 0;
    switch ( prob ) {
      case ONE:
        c=SP ( DonneesA.Column ( a ),DonneesA.Column ( b ) ).sum();
        break;
      case TWO:
        c=SP ( DonneesA.Column ( a ),DonneesB.Column ( b ) ).sum();
        break;
    }
    return ( c );
  }


  /* -------------------------------------------------------------------
   Computes dot product of subtraction of a-th and b-th vector.
   c = (a-b)'*(a-b)
  ------------------------------------------------------------------- */
  double kernel::sub_dot_prod ( long a, long b )
  {
    double c = 0;
    ColumnVector SV;
    switch ( prob ) {
      case ONE:
        SV=DonneesA.Column ( a )-DonneesA.Column ( b );
        break;
      case TWO:
        SV=DonneesA.Column ( a )-DonneesB.Column ( b );
        break;
    }

    c=SP ( SV,SV ).sum();
    return ( c );
  }


  double kernel::ProdMat ( long a, long b )
  {
    double c = 0.0;

    switch ( type ) {
        /* linear kernel */
      case LINEAR:
        c =dot_prod ( a, b );
        break;
        /* polynomial kernel */
      case POLY:
        c = dot_prod ( a, b );
        break;
        /* radial basis functions kernel*/
      case RBF:
        c = sub_dot_prod ( a, b ) ;
        break;
        /* sigmoid */
      case SIGMOID:
        c = dot_prod ( a,b );
//  	    cout << dot_prod(a,b) << "   " << c << endl;
        break;
      default:
        c = 0.0;
    }
    return ( c );
  }


  double kernel::Ckernel ( double a )
  {
    double c = 0.0;

    switch ( type ) {
        /* linear kernel */
      case LINEAR:
        c = linear.alpha*a + linear.decal;
        break;
        /* polynomial kernel */
      case POLY:
        if ( isnan ( poly.decal ) || isnan ( poly.power ) ) {
          cout << "ploynomial kernel bad formed" << endl;
          exit ( -1 );
        }
        c = pow ( ( a + poly.decal ), poly.power );
        break;
        /* radial basis functions kernel*/
      case RBF:
        if ( isnan ( rbf.power ) ) {
          cout << "rbf kernel bad formed" << endl;
          exit ( -1 );
        }
        c = exp ( -rbf.power*a ) ;

        break;
        /* sigmoid */
      case SIGMOID:
        if ( isnan ( sigmoid.gamma ) || isnan ( sigmoid.decal ) ) {
          cout << "sigmoid kernel bad formed" << endl;
          exit ( -1 );
        }

        c = tanh ( sigmoid.gamma*a + sigmoid.decal );
//  	    cout << dot_prod(a,b) << "   " << c << endl;
        break;
      default:
        c = 0.0;
    }
    return ( c );
  }


  /*#define LINEAR 0
  #define POLY 1
  #define RBF 2
  #define SIGMOID 3*/

  double kernel::Ckernel ( long a, long b )
  {
    double c = 0.0;

    switch ( type ) {
        /* linear kernel */
      case LINEAR:
        c = linear.alpha*dot_prod ( a, b ) + linear.decal;
        break;
        /* polynomial kernel */
      case POLY:
        if ( isnan ( poly.decal ) || isnan ( poly.power ) ) {
          cout << "ploynomial kernel bad formed" << endl;
          exit ( -1 );
        }
        c = pow ( ( dot_prod ( a, b ) + poly.decal ), poly.power );
        break;
        /* radial basis functions kernel*/
      case RBF:
        if ( isnan ( rbf.power ) ) {
          cout << "rbf kernel bad formed" << endl;
          exit ( -1 );
        }
        c = exp ( -rbf.power*sub_dot_prod ( a, b ) ) ;

        break;
        /* sigmoid */
      case SIGMOID:
        if ( isnan ( sigmoid.gamma ) || isnan ( sigmoid.decal ) ) {
          cout << "sigmoid kernel bad formed" << endl;
          exit ( -1 );
        }

        c = tanh ( sigmoid.gamma*dot_prod ( a,b ) + sigmoid.decal );
//  	    cout << dot_prod(a,b) << "   " << c << endl;
        break;
      default:
        c = 0.0;
    }
    return ( c );
  }


  void kernel::Computekernel()
  {
    noyau.Release();
    int n1=DonneesA.Ncols();

    /* computes kernel matrix. */
// ofstream filout;
//Matrix Q;
    int step;
    switch ( prob ) {
      case ONE:

        noyau.ReSize ( n1,n1 );
// Q=noyau.ReSize(n1,n1);
// 	    Q.ReSize(n1,n1);
        cout << "Computing Non linear matrix" << endl;
        step=0;
        for ( int i = 1; i <= n1; i++ ) {
          if ( i >= step )
            if ( step+1000>n1 ) {
              cout << "rows between " << step << " and " << n1 <<" from " << n1<< endl;
              step=n1+1;
            } else {
              cout << "rows between " << step << " and " << step +1000 << " from " << n1<<endl;
              step=i+1000;
            }

          for ( int j = i; j <= n1; j++ )
            noyau ( i,j ) =ProdMat ( i,j );

        }
        cout << "rescaling the Nonlinear matrix" << endl;
// cout << "reduction coefficient: " << maxR << endl;
        RescaleN ( 0.0,1.0 );
        cout << "reduction coefficient: " << maxR << endl;

        for ( int i = 1; i <= 1; i++ )
          for ( int j = i; j < 10; j++ ) {
            switch ( type ) {
              case RBF:
                cout << sub_dot_prod ( i,j ) << " ->  " << noyau ( i,j ) << "  ->  " << Ckernel ( noyau ( i,j ) ) << endl;
                break;
              default:
                cout << dot_prod ( i,j ) << " ->  " << noyau ( i,j ) <<  " ->  " << Ckernel ( noyau ( i,j ) ) << endl;
                break;
            }
          }

// 	    cout << "test3" << endl;
        for ( int i = 1; i <= n1; i++ )
          for ( int j = i; j <= n1; j++ ) {
            noyau ( i,j ) = Ckernel ( noyau ( i,j ) );
            noyau ( j,i ) = noyau ( i,j );              /* kernel is symetric */
          }
// 	    cout << "test4" << endl;
// 	    filout.open("Kermat.txt");
// 	    filout << Q;
// 	    filout.close();
// 	    Q.ReleaseAndDelete();
        break;

      case TWO:
//cout << n1 << endl;
        int n2=DonneesB.Ncols();
        noyau.ReSize ( n1,n2 );

// 	    cout << minR << endl;
// 	    cout << maxR << endl;
        for ( int i = 1; i <= n1; i++ )
          for ( int j = 1; j <= n2; j++ )
            noyau ( i,j ) =ProdMat ( i,j );

        RescaleN ( 0.0,1.0 );
// 	   if (maxR!=0 || minR!=0)
//       		noyau=minR + maxR*(noyau-minR)/(maxR-minR);

        for ( int i = 1; i <= n1; i++ )
          for ( int j = 1; j <= n2; j++ )
            noyau ( i,j ) =Ckernel ( noyau ( i,j ) );

        break;
    }

  }


// #define LINEAR 0
// #define POLY 1
// #define RBF 2
// #define SIGMOID 3
  kernel & kernel::operator = ( const kernel & k )
  {
//Setmaxmin(k.minR,k.maxR);
//     cout << maxR << endl;
    minR=k.minR;
    maxR=k.maxR;
    type=k.type;
    prob=k.prob;
    switch ( k.prob ) {
      case ONE:
        SetData ( k.DonneesA );
        break;
      case TWO:
        SetData ( k.DonneesA,k.DonneesB );
        break;
    }

    switch ( k.type ) {
      case LINEAR:
        break;
      case POLY:
        poly=k.poly;
        break;
      case RBF:
        rbf=k.rbf;
        break;
      case SIGMOID:
        sigmoid=k.sigmoid;
        break;

    }
    return *this;
  }

  void  kernel::Setmaxmin ( float _min,float _max )
  {
    minR=_min;
    maxR=_max;
  }

  void kernel::RescaleN ( float _min, float _max )
  {
//    Matrix A(Input.Nrows(),Input.Ncols());
//     Real minim,maxim;

    if ( isnan ( maxR ) && isnan ( minR ) ) {
      minR=noyau.minimum();
      maxR=pow ( 10,floor ( log10 ( noyau.maximum() ) ) +1 );

    }
//noyau=_min + _max*(noyau-minR)/(maxR-minR);
    noyau=noyau/maxR;
  }
}
