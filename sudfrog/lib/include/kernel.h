#ifndef KERNEL_H
#define KERNEL_H

#include "newmatap.h"
#include <iomanip>
#include "newmatio.h"

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

using namespace std;

namespace TMSP_Face_Space
{
#define ONE 1
#define TWO 2

#define LINEAR 0
#define POLY 1
#define RBF 2
#define SIGMOID 3

/** \brief structure that stores a Linear kernel parameters Y = X*alpha + decal; */
  struct kerlin {
    float alpha,  decal;
  };

/** \brief structure that stores a Polynomial kernel parameters Y = pow ( ( X + decal ),power )*/
  struct kerpoly {
    float decal, power;
  };

/** \brief structure that stores a Radial Basis Function kernel parameters Y= exp ( -X*power ) ;*/
  struct kerrbf {
    float power;
  };

/** \brief structure that stores a Sigmoid kernel parameters  Y=tanh ( X*gamma + decal );
*/
  struct kersigmoid { 
    float decal, gamma;
  };

/** \brief Class that create kernels and implement kernel methods*/
  class kernel
  {

    public:
      Matrix DonneesA;
      Matrix DonneesB;
      Matrix noyau;
      int type;
      float maxR,minR;

      struct kerlin linear;
      struct kerpoly poly;
      struct kerrbf rbf;
      struct kersigmoid sigmoid;

      /**
       *
       */
      kernel();
      /**
       *
       * @param Data
       */
      kernel ( GeneralMatrix &Data );                      //linear
      /**
       *
       * @param Data
       * @param k
       */
      /**
       *
       * @param Data
       * @param k
       */
      kernel ( GeneralMatrix &Data,kerlin k );             //linear
      /**
       *
       * @param Data
       * @param k
       */
      kernel ( GeneralMatrix &Data,kerpoly k );             //poly
      /**
       *
       * @param Data
       * @param k
       */
      kernel ( GeneralMatrix &Data,kerrbf k );             //rbf
      /**
       *
       * @param Data
       * @param k
       */
      kernel ( GeneralMatrix &Data,kersigmoid k );         //sigmoid
      /**
       *
       * @param Data1
       * @param Data2
       */
      kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2 ); //linear
      /**
       *
       * @param Data1
       * @param Data2
       * @param k
       */
      kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerlin k );
      /**
       *
       * @param Data1
       * @param Data2
       * @param k
       */
      kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerpoly k );
      /**
       *
       * @param Data1
       * @param Data2
       * @param k
       */
      kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kerrbf k );
      /**
       *
       * @param Data1
       * @param Data2
       * @param k
       */
      kernel ( GeneralMatrix &Data1,GeneralMatrix &Data2,kersigmoid k );
      /**
       *
       */
      ~kernel();

      /**
       *
       * @param Data
       */
      void SetData ( const Matrix &Data );
      /**
       *
       * @param Data1
       * @param Data2
       */
      void SetData ( const Matrix &Data1,const Matrix &Data2 );
      /**
       *
       * @param Data1
       */
      void SetFData ( const Matrix &Data1 );
      /**
       *
       * @param Data1
       */
      void SetSData ( const Matrix &Data1 )  ;             // save time for multiple projection

      /**
       *
       * @param T
       */
      void settype ( int T );
      /**
       *
       * @param k
       */
      void setker ( kerlin k );
      /**
       *
       * @param k
       */
      void setker ( kerpoly k );
      /**
       *
       * @param k
       */
      void setker ( kerrbf k );
      /**
       *
       * @param k
       */
      void setker ( kersigmoid k );
      /**
       *
       */
      void Computekernel();
      /**
       *
       * @param k
       * @return
       */
      kernel & operator = ( const kernel &  k);
      /**
       *
       * @param _min
       * @param _max
       */
      void Setmaxmin ( float _min,float _max );


    private:
      int prob;
      double dot_prod ( long a, long b );
      double sub_dot_prod ( long a, long b );
      double Ckernel ( long a, long b );
      double Ckernel ( double a );
      void RescaleN ( float _min, float _max );
      double ProdMat ( long a, long b );
  };

}
#endif
