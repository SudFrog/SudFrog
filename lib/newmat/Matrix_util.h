#ifndef MATRIXUTIL_H_
#define MATRIXUTIL_H 1

#include "newmatap.h"
#include <iomanip>
#include "newmatio.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#define pi 3.1416

#ifndef MINFLOAT
#define MINFLOAT -1.0E+20
#endif

#ifndef MAXFLOAT
#define MAXFLOAT 1.0E+20
#endif

using namespace std;

double Min  ( double x, double y );
double Max  ( double x, double y );

float min(Matrix &A);
float max(Matrix &A);
float min(Matrix &A,int &posx,int &posy);
float max(Matrix &A,int &posx,int &posy);
double mean(Matrix &A);
double Std(Matrix &A);
unsigned char * readpgm(const char *filename,int &col,int &row);
ReturnMatrix ReadPGM_Matrix( const char * filename );
ReturnMatrix LoadVectorFromfile( ifstream & fileIn, int size );
ReturnMatrix LoadVectorFrombinfile( ifstream & fileIn, int size );
int minmax(Matrix &A,double &minimum,double& maximum);

ReturnMatrix LoadMatrixFromfile( ifstream & fileIn, int rows, int cols );
bool Save_Projection( ColumnVector & face, string filename );

ReturnMatrix ReadMatrixfromFile(char* filename);
bool GetMatrixSize( string line,int &sizeheight,int &sizewidth);

ReturnMatrix Read_Projection( string filename );
ReturnMatrix Read_Projection_bin( string filename );

ReturnMatrix MatrixRescale(Matrix & Input,float _min, float _max);
//ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max,Mask Masque);
ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max);
ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max,float lamda);
ReturnMatrix ReSize(Matrix &A,int scale,int type);

ReturnMatrix Mat2Vect(Matrix &A);
ReturnMatrix sqrt(Matrix &A);
ReturnMatrix atan(Matrix &A);
//prendre en compte les signes de x/y
ReturnMatrix atan2(Matrix &A,Matrix &B);
ReturnMatrix abs(Matrix &A);
ReturnMatrix div(Matrix &A,Matrix &B);
ReturnMatrix Exp(Matrix &SOURCE);
ReturnMatrix abs(ColumnVector &A);

ReturnMatrix CenterVector(ColumnVector &Vector);
ReturnMatrix CenterMatrix(Matrix &Mat);
ReturnMatrix Discreate(Matrix &Mat,int level);
ReturnMatrix Clean(Matrix &Mat);
bool Save_Code( ColumnVector & face, string filename );
//void putmask(ColumnVector &image,Mask MaskImage);
//void putmask(Matrix &imageout,Mask MaskImage);

float GetSharpness(Matrix &Image);
ReturnMatrix Sharpen(Matrix &Image);
ReturnMatrix Sharpen(Matrix &Image,float sharpness);

ReturnMatrix CreateGaussK(int sizeGauss,float sigma);
ReturnMatrix  Convolve(Matrix &SOURCE,Matrix &KERNEL);
ReturnMatrix Log(Matrix &SOURCE);

ReturnMatrix Quantify(Matrix &SOURCE,int n);
int GetNonNulsAmp(Matrix &SOURCE,Matrix &RES,double seuil);

// double Distance(ColumnVector & face1,ColumnVector & face2,string Method );
double Distance(ColumnVector & face1,ColumnVector & face2,string Method );
double Distance(Matrix & face1,Matrix & face2,string Method );

ReturnMatrix GetProdadap(Matrix &Mat1,Matrix &Mat2);
ReturnMatrix SplitAndHist(Matrix &A,int histsize);
ReturnMatrix GetDHistogram(Matrix &A,int histsize);

void Codify(Matrix &Re,Matrix &Im,double &val,double &mask);
void Codify1(Matrix &Re,Matrix &Im,double &pos,double &neg,double &mask);

ReturnMatrix SplitAndCode(Matrix &RealM,Matrix &ImM);
int CountNan(Matrix &A);
bool isNAN(Matrix &A);
bool OneisNAN(Matrix &A);
void SelectCommonNonNan(ColumnVector &In1,ColumnVector &In2,ColumnVector &out1,ColumnVector &out2);

vector < vector <Matrix > > Centerforgrid(vector < vector <Matrix > > &face1,int rowout,int colout);
ReturnMatrix GetCentered(Matrix &A, int outrows,int outcols);
void GetFFTforgrid(vector < vector <Matrix > > &face1 ,vector < vector <Matrix > > &face1FFTRRef,vector < vector <Matrix > > &face1FFTIRef);
void GetIFFTforgrid(vector < vector <Matrix > > &face1FFTRRef,vector < vector <Matrix > > &face1FFTIRef,vector < vector <Matrix > > &face1);
ReturnMatrix GetMaxCorrfromgrid(vector < vector <Matrix > > &face1);

ReturnMatrix CrossCorrelation(Matrix &A,Matrix&B);
ReturnMatrix CrossCorrelationNAN(Matrix &A,Matrix&B);

// float CrossValidate(Matrix & face1,Matrix & face2,int rows,int cols );
vector < float>  CrossValidate(Matrix & face1,Matrix & face2,int rows,int cols,int &nvref,int &nhref );
ReturnMatrix HausedorfNAN(Matrix &A,Matrix&B);

ReturnMatrix LoadmatrixFrombinfile( ifstream & fileIn, int rows, int cols );
ReturnMatrix LoadmaskFrombinfile( ifstream & fileIn, int rows, int cols );

double Distance(ColumnVector & face1,ColumnVector & face2,Matrix MeanProj );

int ComputeEigens( Matrix & CM, DiagonalMatrix & EigValues,Matrix & EigVectors );
int SPComplexe(Matrix &RealIn1,Matrix &ImIn1,Matrix &RealIn2,Matrix &ImIn2,Matrix &Realout,Matrix &ImOut);
ReturnMatrix ReSizeAndcpy(Matrix &A,int newrows,int newcols);

ReturnMatrix FFTshift(Matrix &Source);
ReturnMatrix GetShiftedFFTAmp(Matrix &A);

  /**
   *
   * @param filename
   * @return
   */
  ReturnMatrix loadSim ( char * filename );
  /**
   * 
   * @param filename 
   * @return 
   */
  ReturnMatrix loadMask ( char * filename );
  /**
   * 
   * @param filename 
   * @param Stype 
   * @return 
   */
 ReturnMatrix loadMatrix ( char * filename, string Stype );

#endif
