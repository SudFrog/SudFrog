#include "Matrix_util.h"

double Min  ( double x, double y )
{
  return x < y ? x : y;
}
double Max  ( double x, double y )
{

  return x > y ? x : y;
}

ReturnMatrix MatrixRescale(Matrix & Input,float _min, float _max)
{
  Matrix A(Input.Nrows(),Input.Ncols());
  Real minim,maxim;
  minim=Input.minimum();
  maxim=Input.maximum();
  if (maxim==0 && minim==0)
    A=0.0;
  else
    A=_min + _max*(Input-minim)/(maxim-minim);

  A.Release();
  return A;
}

  ReturnMatrix loadSim ( char * filename )
  {

    return loadMatrix ( filename,"MF" );
  }

  ReturnMatrix loadMask ( char * filename )
  {

    return loadMatrix ( filename,"MB" );
  }


  ReturnMatrix loadMatrix ( char * filename, string Stype )
  {
    Matrix mask;
    int QueryNum,TargetNum;
    ifstream fileIN ( filename,ios::binary );

    if ( fileIN.good() ) {
      string Targetname,queryname,size,type;

      getline ( fileIN,type );
      getline ( fileIN,Targetname );
      getline ( fileIN,queryname );
      getline ( fileIN,size );
      int pos=size.find ( Stype );
      if ( pos== ( int ) string::npos ) {
        cout << "Size not found for mask" << endl;
        exit ( -1 );
      }
      sscanf ( size.substr ( pos+3 ).c_str(),"%d %d",&QueryNum,&TargetNum );

      if ( Stype=="MB" )
        mask=LoadmaskFrombinfile ( fileIN, QueryNum,TargetNum );
      else
        mask=LoadmatrixFrombinfile ( fileIN, QueryNum,TargetNum );

//			mask.submatrix ( 200,QueryNum,1,TargetNum ) =0.0;
//			mask.submatrix ( 1,QueryNum,200,TargetNum ) =0.0;
      cout << "Matrix Loaded" << endl;
      fileIN.close();
      mask.Release();
      return mask;

    } else {
      cout << "Impossible d'ouvrir " << filename << endl;
      exit ( -1 );
    }
    return 0;
  }



// ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max,Mask Masque)
// {
//     int height=Input.Nrows();
//     int width=Input.Ncols();
//     Matrix A=MatrixRescale(Input,_min, _max);
// // A=
//
// //float sum=A.sum();
//     float meanMat=0;                                        //=sum/(height*width);
//     float stdMat=0;
//     int k=0;
//     for (int i=8;i<=height-1;i++)
//         for (int j=1;j<=width;j++)
//             if (Ismask(j,i,Masque))
//             {
//                 meanMat+=A(i,j);
//                 stdMat+=A(i,j)*A(i,j);
//                 k++;
//             }
//
//     meanMat/=k;
//     stdMat=sqrt(stdMat/k - meanMat*meanMat);
//
// //cout << meanMat << " " << stdMat<< endl;
//     float lamda=50.0;
//     A=128+(lamda*(A-meanMat)/stdMat);
//
//     for (int i=1;i<=height;i++)
//         for (int j=1;j<=width;j++)
//             if (A(i,j)<0)
//                 A(i,j)=0;
//     else
//     if (A(i,j)>255)
//         A(i,j)=255;
// //
//
//     A.Release();
//     return A;
// }
unsigned char * readpgm(const char *filename,int &col,int &row)
{
    FILE *fp;
//	int r,c;
    char buff[256];
//	int gray;
    int pgmtype;
    int colorLevel;
    unsigned char *img;

    if ((fp = fopen(filename,"rb")) == NULL)
    {
        printf("File does not exist!\n");                   //bad source file name
        return NULL;
//	exit(0);
    }
    else
    {
        if(fgetc(fp)!='P')                                  //check if the source file is in correct format
        {
            printf("Not a PGM file!\n");
            fclose(fp);
            return NULL;
        }
        else
        {
            fgets(buff,256,fp);                             //inputs the first line in the image (starting from 'P')
            if(buff[0]=='2')
            {
                pgmtype=2;
            }
            else
            {
                pgmtype=5;
            }

            while(1)
            {
                fgets(buff,256,fp);                         //comments are stored in char array comments
                if(buff[0]!='#')
                {
                    sscanf(buff,"%d %d",&col,&row);
                    break;
                }
            }

            fgets(buff,256,fp);                             //read max grey level
            sscanf(buff,"%d",&colorLevel);

//		if (img)
//			delete []img;

            img = new unsigned char[row*col];               //assign memory to source and target arrays

            if(pgmtype==5)                                  //binary file
            {
                fread(img,sizeof(unsigned char),col*row,fp);
//	fclose(fp);
//	return NULL;

            }
            else
            {
//	fclose(fp);
                return NULL;
            }
            fclose(fp);                                     //close the file pointer
            return img;
        }
    }
}

ReturnMatrix ReadPGM_Matrix( const char * filename )
{
    int origheight, origwidth;
    unsigned char * image;
// int k = 1;
    image = readpgm( filename, origwidth, origheight );
    //image = readpgmmatlab( filename, origwidth, origheight );

    Matrix FACE( origheight, origwidth );
    if ( image )
//{
        for ( int j = 0; j < origwidth; j++ )
            for ( int i = 0; i < origheight; i++ )
                FACE( i+1,j+1 ) = ( int )image[i * origwidth + j];
//}

//FACE=CenterVector(FACE,FACE.Nrows());
    FACE.Release();
    return FACE;
}


ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max)
{

  Matrix A=MatrixSN(Input,_min, _max,60.0);
  A.Release();
  return A;
}


ReturnMatrix MatrixSN(Matrix & Input,float _min, float _max,float lamda)
{
  int height=Input.Nrows();
  int width=Input.Ncols();
  Matrix A=MatrixRescale(Input,_min, _max);
  float sum=A.sum();
  float meanMat=sum/(height*width);                       //=sum/(height*width);

  float stdMat=sqrt(A.sum_square()/(height*width) - meanMat*meanMat);

// float lamda=70.0;
  A=128+(lamda*(A-meanMat)/stdMat);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (A(i,j)<0)
        A(i,j)=0;
      else
        if (A(i,j)>255)
          A(i,j)=255;

  A.Release();
  return A;
}


/**
 *
 *
 * @param fileIn
 * @param size
 *
 * @return
 */
ReturnMatrix LoadVectorFromfile( ifstream & fileIn, int size,string filename )
{
  ColumnVector Vect( size );
  for ( int i = 1; i <= size; i++ )
    {
      if ( fileIn.good() )
        fileIn >> Vect( i );
      else
        {
          cout << "error reading file " <<filename << endl;
          exit(-1);
        }
    }
  Vect.Release();
  return Vect;
}

ReturnMatrix LoadVectorFromfile( ifstream & fileIn, int size )
{
  ColumnVector Vect( size );
  for ( int i = 1; i <= size; i++ )
    {
      if ( fileIn.good() )
        fileIn >> Vect( i );
      else
        {
          cout << "error reading file" << endl;
          exit(-1);
        }
    }
  Vect.Release();
  return Vect;
}


ReturnMatrix LoadVectorFrombinfile( ifstream & fileIn, int size )
{
  ColumnVector Vect( size );
  float data[size];

  fileIn.read( (char *)(&data), sizeof(float)*size );
  if ( fileIn.bad() )
    {
      cerr << "Error reading data" << endl;
      exit( 0 );
    }

  for ( int i = 1; i <= size; i++ )
    Vect(i)=data[i-1];

  Vect.Release();
  return Vect;
}


ReturnMatrix ReadMatrixfromFile(char * file)
{
  string line;
  ifstream filein(file);
  Matrix Mat;
  int sizewidth,sizeheight;
  bool part;
// cout << file << endl;
  if (! filein.good())
    {
      cout << file << " Not found" << endl;
      exit (-1);
    }

  while ( filein.good() )
    {
      getline( filein, line );
      part = GetMatrixSize( line,sizeheight,sizewidth );
      if ( part == true )
        {
          if (sizewidth ==0)
            sizewidth=1;
          Mat=LoadMatrixFromfile( filein,sizeheight,sizewidth);

          filein.close();
          break;
        }
      else
        {
          filein.close();
          cout << "error loading Matrix file" << endl;
          exit (-1);
        }
    }

  return Mat;
}


bool GetMatrixSize( string line,int &sizeheight,int &sizewidth)
{
  int pos1, pos2;
  sizeheight=0;
  sizewidth=0;
  int pos = line.find( "Matrix" );

  if ( pos == ( int )string::npos )
    pos = line.find( "Code" );

  if ( pos != ( int )string::npos )
    {
      pos = line.find( "Size" );
      if ( pos != ( int )string::npos )
        {
          pos1 = line.find( "\"", pos + 1 );
          pos2 = line.find( "\"", pos1 + 1 );
          string tmp = line.substr( pos1 + 1, pos2 - pos1 - 1 );
          sscanf(tmp.c_str(),"%d,%d",&sizeheight,&sizewidth);
// 	   cout << sizeheight << " " << sizewidth << endl;
        }
      return true;
    }
  else
    return false;
}


/**
 *
 *
 * @param fileIn
 * @param rows
 * @param cols
 *
 * @return
 */
ReturnMatrix LoadMatrixFromfile( ifstream & fileIn, int rows, int cols )
{
  Matrix Mat( rows, cols );
  string val;
//     cout << rows << " "<< cols<< endl;
  for ( int i = 1; i <= rows; i++ )
    for ( int j = 1; j <= cols; j++ )
      {
        if ( fileIn.good() )
          {
            fileIn >> val;
            Mat( i, j )=atof((char*)val.c_str());           // to hundel the nan values
//	if (!isnan(Mat( i, j )))
//		cout << Mat( i, j ) << endl;
          }
        else
          {
            cout << "error reading file" << endl;
            exit(-1);
          }
      }
  Mat.Release();
  return Mat;
}


ReturnMatrix LoadmatrixFrombinfile( ifstream & fileIn, int rows, int cols )
{
  Matrix Mat( rows, cols );
  float val;
    // cout << rows << " "<< cols<< endl;
  if ( fileIn.good() )
    {
      for ( int i = 1; i <= rows; i++ )
        for ( int j = 1; j <= cols; j++ )
          {
            fileIn.read( (char *)(&val), sizeof(val) );
            Mat( i, j )=val;                                // to hundel the nan values
          }
    }
  else
    {
      cout << "error reading file" << endl;
      exit(-1);
    }
//  cout << "test" << endl;
  Mat.Release();
  return Mat;
}


ReturnMatrix LoadmaskFrombinfile( ifstream & fileIn, int rows, int cols )
{
  Matrix Mat( rows, cols );
  unsigned char val;
//     cout << rows << " "<< cols<< endl;
  if ( fileIn.good() )
    {
      for ( int i = 1; i <= rows; i++ )
        for ( int j = 1; j <= cols; j++ )
          {
            fileIn.read( (char *)(&val), sizeof(val) );
            Mat( i, j )=val;                                // to hundel the nan values
          }
    }
  else
    {
      cout << "error reading file" << endl;
      exit(-1);
    }
//  cout << "test" << endl;
  Mat.Release();
  return Mat;
}


/**
 *
 *
 * @param face
 * @param filename
 *
 * @return
 */

bool Save_Projection( ColumnVector & face, string filename )
{
  ofstream outfileMat( filename.c_str() );
//outfileMat.open;
  if ( !outfileMat.good() )
    return false;
  outfileMat << "<Code_Size=\"" << face.Nrows() <<  "\">"<< endl;
  outfileMat << face;
  outfileMat.close();
  return true;
}


bool Save_Code( ColumnVector & face, string filename )
{
  ofstream outfileMat( filename.c_str() );
//outfileMat.open;
  if ( !outfileMat.good() )
    return false;

  outfileMat << "<Code_Size=\"" << face.Nrows() <<  "\">"<< endl;
  float val;
  for (int i=1;i<=face.Nrows();i++)
    {
      val=(float)face(i);
      outfileMat.write( ( char * ) &val, sizeof( float ) );
    }
//
//    outfileMat << face;

  outfileMat.close();
  return true;
}


/**
 *
 *
 * @param filename
 *
 * @return
 */
ReturnMatrix Read_Projection( string filename )
{

  string line;
  int size=0,pos1,pos2;
  ifstream outfileMat( filename.c_str() );
//outfileMat.open;
  if ( !outfileMat.good() )
    {
      cout << "Error Reading " <<  filename << endl;
      exit(-1);
    }

  while (outfileMat.good())
    {
      getline(outfileMat,line);
      int pos = line.find( "Code_Size" );

      if ( pos != ( int )string::npos )
        {
          pos1 = line.find( "\"" );
          pos2 = line.find( "\"", pos1 + 1 );
          size = ( int )atof( line.substr( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
          if (size==0)
            {
              cout << "Error Reading " <<  filename << endl;
              exit(-1);
            }
          break;
        }
    }
  if (size==0)
    {
      cout << "Error Reading " <<  filename << " The size of code could not be determined"<< endl;
      exit(-1);
    }
  ColumnVector Projection;
  Projection = LoadVectorFromfile(outfileMat,size,filename);
// cout << Projection << endl;
  outfileMat.close();
  Projection.Release();

  return Projection;

}


ReturnMatrix Read_Projection_bin( string filename )
{

  string line;
  int size=0,pos1,pos2;
  ifstream outfileMat( filename.c_str() );
//outfileMat.open;
  if ( !outfileMat.good() )
    {
      cout << "Error Reading " <<  filename << endl;
      exit(-1);
    }

  while (outfileMat.good())
    {
      getline(outfileMat,line);
      int pos = line.find( "Code_Size" );

      if ( pos != ( int )string::npos )
        {
          pos1 = line.find( "\"" );
          pos2 = line.find( "\"", pos1 + 1 );
          size = ( int )atof( line.substr( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
          if (size==0)
            {
              cout << "Error Reading " <<  filename << endl;
              exit(-1);
            }
          break;
        }
    }
  if (size==0)
    {
      cout << "Error Reading " <<  filename << " The size of code could not be determined"<< endl;
      exit(-1);
    }
  ColumnVector Projection;
  Projection = LoadVectorFrombinfile(outfileMat,size);
// cout << Projection << endl;
  outfileMat.close();
  Projection.Release();

  return Projection;

}

ReturnMatrix ReSizeAndcpy(Matrix &A,int newrows,int newcols)
{
  int height=A.Nrows();
  int width=A.Ncols();

  Matrix TMP(newrows,newcols);
  TMP=0.0;
  TMP.submatrix(1,height,1,width)=A;
  return TMP;

}

ReturnMatrix ReSize(Matrix &A,int scale,int type)
{

  int height=A.Nrows();
  int width=A.Ncols();

  int heightout=(int)height/scale;
  int widthout=(int)width/scale;

  Matrix Res(heightout,widthout);
  float total;
  int k;
  switch (type)
    {
    case 0:                                             // simple resize
      for (int i=1;i<=heightout;i++)
        for (int j=1;j<=widthout;j++)
          Res(i,j)=A(i*scale,j*scale);
      break;
    case 1:                                             //resize with interpolation 4x4
      for (int i=1;i<=heightout;i++)
        for (int j=1;j<=widthout;j++)
          {
//   if (A(i*scale,j*scale)!=0.0)
//   {
            total=0;
            k=0;

            for (int h=-2;h<=2;h++)
              for (int l=-2;l<=2;l++)
                {
                  if (i*scale+h>=1 && i*scale+h<=height &&
                      j*scale+l >=1 && j*scale+l<=width)
                    {
                      total+=A(i*scale+h,j*scale+l);
                      k++;
                    }
                }
            Res(i,j)=total/k;
//   }
//    else
//      Res(i,j)=0.0;
          }

      break;
    }
  Res.Release();
  return Res;
}


ReturnMatrix Mat2Vect(Matrix &A)
{
  int height=A.Nrows();
  int width=A.Ncols();
  int k=1;

  ColumnVector outVect(height*width);

  for (int j=1;j<=width;j++)
    for (int i=1;i<=height;i++)
      {
        outVect(k) = A(i,j);
        k++;
      }
  outVect.Release();
  return outVect;

}


ReturnMatrix sqrt(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();

  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      Res(i,j)=sqrt(A(i,j));

  Res.Release();
  return Res;
}


int minmax(Matrix &A,double &minimum,double& maximum)
{

  int height=A.Nrows();
  int width=A.Ncols();

  minimum=MAXFLOAT;
  maximum=MINFLOAT;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          minimum=Min(minimum,A(i,j));
          maximum=Max(maximum,A(i,j));
        }

  return 0;
}

int CountNan(Matrix &A)
{
  int height=A.Nrows();
  int width=A.Ncols();
//    posx=NAN;
//    posy=NAN;

  int count=0;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (isnan(A(i,j)))
	count++;

return count;
}

float min(Matrix &A,int &posx,int &posy)
{

  int height=A.Nrows();
  int width=A.Ncols();
//    posx=NAN;
//    posy=NAN;

  float minimum=MAXFLOAT;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          if (minimum>A(i,j))
            {
              minimum=A(i,j);
              posx=i;
              posy=j;
            }
        }

  if (minimum==MAXFLOAT)
    return NAN;
  else
    return minimum;
}

float max(Matrix &A,int &posx,int &posy)
{

  int height=A.Nrows();
  int width=A.Ncols();
//    posx=NAN;
//    posy=NAN;

  float maximum=MINFLOAT;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          if (maximum<A(i,j))
            {
              maximum=A(i,j);
              posx=i;
              posy=j;
            }
        }

  if (maximum==MINFLOAT)
    return NAN;
  else
    return maximum;
}

double Std(Matrix &A)
{
  int height=A.Nrows();
  int width=A.Ncols();
  float sum=A.sum();
  float meanMat=sum/(height*width);                       //=sum/(height*width);

  return(sqrt(A.sum_square()/(height*width) - meanMat*meanMat));

}

double mean(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();
  int count=0;
  double meanv=0.0;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          meanv+=A(i,j);
          count++;
        }
  if (count > 0)
    return meanv/count;
  else
    return NAN;                                         //infinity
}


float min(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();

  float minimum=MAXFLOAT;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          minimum=Min(minimum,A(i,j));
        }

  if (minimum==MAXFLOAT)
    return NAN;
  else
    return minimum;
}


float max(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();

  float maximum=MINFLOAT;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          maximum=Max(maximum,A(i,j));
        }

  if (maximum==MINFLOAT)
    return NAN;
  else
    return maximum;
}


ReturnMatrix atan(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();

  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      Res(i,j)=atan(A(i,j));

  Res.Release();
  return Res;
}


ReturnMatrix atan2(Matrix &A,Matrix &B)
{

  int height=A.Nrows();
  int width=A.Ncols();

  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      Res(i,j)=atan2(A(i,j),B(i,j));

  Res.Release();
  return Res;
}


ReturnMatrix abs(Matrix &A)
{

  int height=A.Nrows();
  int width=A.Ncols();

  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      Res(i,j)=fabs(A(i,j));

  Res.Release();
  return Res;
}


ReturnMatrix abs(ColumnVector &A)
{

  int height=A.Nrows();
//int width=A.Ncols();

  ColumnVector Res(height);

  for (int i=1;i<=height;i++)
    Res(i)=fabs(A(i));

  Res.Release();
  return Res;
}


ReturnMatrix Log(Matrix &SOURCE)
{
  int height=SOURCE.Nrows();
  int width=SOURCE.Ncols();
  Matrix LOG(height,width);
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      LOG(i,j)=log(SOURCE(i,j));

  LOG.Release();
  return LOG;
}


ReturnMatrix Exp(Matrix &SOURCE)
{
  int height=SOURCE.Nrows();
  int width=SOURCE.Ncols();
  Matrix EXP(height,width);
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      EXP(i,j)=exp(SOURCE(i,j));

  EXP.Release();
  return EXP;
}


ReturnMatrix div(Matrix &A,Matrix &B)
{

  int height=A.Nrows();
  int width=A.Ncols();

  if ((height !=B.Nrows()) || (width!=B.Ncols()))
    {
      cout << "Dimension mismatch for elements division" << endl;
      exit (-1);
    }
  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (B(i,j)!=0)
        Res(i,j)=A(i,j)/B(i,j);
      else
        Res(i,j)=NAN;

  Res.Release();
  return Res;
}

/*
void putmask(ColumnVector &image,Mask MaskImage)
{
//int imageRows=imageout.Nrows();
//int imageCols=imageout.Ncols();

    for(int i=1; i<=MaskImage.NORMALIZED_ROW; ++i)
        for(int j=1; j<=MaskImage.NORMALIZED_COL; ++j)
            if(!Ismask(j,i,MaskImage))
                image((i-1)*MaskImage.NORMALIZED_COL+j)=0.0;
}*/

/*
void putmask(Matrix &imageout,Mask MaskImage)
{
    int imageRows=imageout.Nrows();
    int imageCols=imageout.Ncols();

    for(int i=1; i<=imageRows; ++i)
        for(int j=1; j<=imageCols; ++j)
            if(!Ismask(j,i,MaskImage))
                imageout(i,j)=0.0;
}*/


float GetSharpness(Matrix &Image)
{
  float sharpness=0;
  int imageRows=Image.Nrows();
  int imageCols=Image.Ncols();

  float flou;
  for (int i=2;i<imageRows;i++)
    for (int j=2;j<imageCols;j++)
      {
        flou=(Image(i,j)+
              Image(i-1,j-1)+
              Image(i-1,j)+
              Image(i-1,j+1)+
              Image(i,j-1) +
              Image(i,j+1)+
              Image(i+1,j-1) +
              Image(i+1,j)+
              Image(i+1,j+1))/9;

        sharpness+=(float)fabs(flou - Image(i,j));

      }

  return sharpness/(imageCols*imageRows);
}


ReturnMatrix Sharpen(Matrix &Image)
{
  int imageRows=Image.Nrows();
  int imageCols=Image.Ncols();
  Matrix Out(imageRows,imageCols);
  Matrix Laplaced(imageRows,imageCols);
//float laplacecooef;
  Laplaced=0.0;                                           // FOR THE BORDERS

  Matrix LapKernel(3,3);
  int a[] = { -1,-1,-1,-1,8,-1,-1,-1,-1};
  LapKernel << a;

  Laplaced=Convolve(Image,LapKernel);
  Out=Image + MatrixRescale(Laplaced,Image.Minimum(),Image.Maximum());
  Out=MatrixSN(Out,0, 255,60);
  Laplaced.ReleaseAndDelete();
  Out.Release();
  return Out;
}


ReturnMatrix blur(Matrix &Image)
{
  int imageRows=Image.Nrows();
  int imageCols=Image.Ncols();
  Matrix blured(imageRows,imageCols);
  blured=0.0;                                             // FOR THE BORDERS

  Matrix GaussK=CreateGaussK(1,1.2);
  blured=Convolve(Image,GaussK);
  blured=MatrixSN(blured,0, 255,60);

  blured.Release();
  return blured;
}


ReturnMatrix Sharpen(Matrix &Image,float sharpness)
{
  int imageRows=Image.Nrows();
  int imageCols=Image.Ncols();
  Matrix Out(imageRows,imageCols);
  Out=Image;
  float flou=GetSharpness(Out);
  int count=0;
  while (flou < sharpness && count < 10)
    {
      Out=Sharpen(Out);
      flou=GetSharpness(Out);
      count++;
//      while (flou > sharpness+sharpness*0.1)
// {
// 	Out=blur(Out);
//         flou=GetSharpness(Out);
// }
//cout << flou << endl;
    }
//   cout << GetSharpness(Out) << endl;
  Out.Release();
  return Out;
}


ReturnMatrix Clean(Matrix &Mat)
{
  Matrix outvector(Mat.Nrows(),Mat.Ncols());
  outvector=0.0;

  for ( int i = 1; i <= Mat.Nrows(); i++ )
    for ( int j = 1; j <= Mat.Ncols(); j++ )
      if (!isnan(Mat(i,j)))
        outvector(i,j)=(Mat(i,j)+2.0*pi);

//outvector=(Vector-mean);

  outvector.Release();
  return outvector;
}

ReturnMatrix Discreate(Matrix &Mat,int level)
{
  Matrix outvector(Mat.Nrows(),Mat.Ncols());
  outvector=0.0;

  float maxl=Mat.maximum();
  float minl=Mat.minimum();
  float seg=(maxl-minl)/level;
  for ( int i = 1; i <= Mat.Nrows(); i++ )
    for ( int j = 1; j <= Mat.Ncols(); j++ )
      if (!isnan(Mat(i,j)))
        outvector(i,j)=ceil((Mat(i,j)-minl)/seg);

//outvector=(Vector-mean);

  outvector.Release();
  return outvector;
}


ReturnMatrix CenterMatrix(Matrix &Mat)
{
  Matrix outvector(Mat.Nrows(),Mat.Ncols());
  outvector=0.0;
  Real size=0;
//cout << size<< endl;
  Real meanv=mean(Mat);

  Real sigma=0;
  for ( int i = 1; i <= Mat.Nrows(); i++ )
    for ( int j = 1; j <= Mat.Ncols(); j++ )
      if (!isnan(Mat(i,j)))
        {
          sigma+=(Mat(i,j)-meanv)*(Mat(i,j)-meanv);
          size++;
        }

  sigma=sqrt(sigma/(size-1));

  for ( int i = 1; i <= Mat.Nrows(); i++ )
    for ( int j = 1; j <= Mat.Ncols(); j++ )
      if (!isnan(Mat(i,j)))
        outvector(i,j)=(Mat(i,j)-meanv)/sigma;

//outvector=(Vector-mean);

  outvector.Release();
  return outvector;

}


ReturnMatrix CenterVector(ColumnVector &Vector)
{
  ColumnVector outvector;
  Real size=Vector.Nrows();
//cout << size<< endl;
  Real mean=Vector.sum()/(size);
//cout << mean << endl;
  Real sigma=0;
  for ( int j = 1; j <= size; j++ )
    sigma+=(Vector(j)-mean)*(Vector(j)-mean);

  sigma=sqrt(sigma/(size-1));
  outvector=(Vector-mean)/sigma;
//outvector=(Vector-mean);

  outvector.Release();
  return outvector;

}


ReturnMatrix CreateGaussK(int sizeGauss,float sigma)
{
  Matrix Filtre(2*sizeGauss+1,2*sizeGauss+1);

  for (int i=-sizeGauss;i<=sizeGauss;i++)
    for (int j=-sizeGauss;j<=i;j++)
      {
        Filtre(i+sizeGauss+1,j+sizeGauss+1)=exp(-(i*i+j*j)/(2*sigma*sigma));
        Filtre(j+sizeGauss+1,i+sizeGauss+1)=Filtre(i+sizeGauss+1,j+sizeGauss+1);
      }

  Filtre/=Filtre.sum();

  Filtre.Release();
  return Filtre;
}


ReturnMatrix  Convolve(Matrix &SOURCE,Matrix &KERNEL)
{
  int height=SOURCE.Nrows();
  int width=SOURCE.Ncols();
  Matrix RESULT(height,width);
  RESULT=0.0;
  int Kernelsize=(KERNEL.Nrows()-1)/2;
//cout << Kernelsize << endl;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      for (int k=-Kernelsize;k<=Kernelsize;k++)
        for (int l=-Kernelsize;l<=Kernelsize;l++)
          if ((i+k)>=1 && (j+l)>=1 && (i+k)<=height && (j+l) <=width)
            RESULT(i,j)+=SOURCE(i+k,j+l)*KERNEL(k+Kernelsize+1,l+Kernelsize+1);

  RESULT.Release();
  return RESULT;
}


ReturnMatrix Quantify(Matrix &SOURCE,int n)
{
  int height=SOURCE.Nrows();
  int width=SOURCE.Ncols();
  Matrix RESULT(height,width);

  ColumnVector list(n);
  for (int i=1;i<=n;i++)
    list(i)=i*2*pi/n;

//	cout << list << endl;
//exit(-1);
  int k;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        k=1;
        while (SOURCE(i,j)>list(k))
          k++;
        RESULT(i,j)=list(k);
      }
  RESULT.Release();
  return RESULT;
}


double Distance(ColumnVector & face1,ColumnVector & face2,Matrix MeanProj )
{

  double distance=0;
  ColumnVector vect1,vect2;
  float dist1[MeanProj.Ncols()];
  float dist2[MeanProj.Ncols()];
  string method="cos";

  for (int i=1;i<=MeanProj.Ncols();i++)
    {
      vect1=MeanProj.Column(i);
      dist1[i-1]=Distance(face1,vect1,method);
      dist2[i-1]=Distance(face2,vect1,method);
    }
//int class1,class2;

  int maxc1=0;
  int maxc2=0;
  float val1=dist1[0];
  float val2=dist2[0];
  for (int i=1;i<MeanProj.Ncols();i++)
    {
      if (val1 > dist1[i])
        {
          val1=dist1[i];
          maxc1=i;
        }

      if (val2 > dist2[i])
        {
          val2=dist2[i];
          maxc2=i;
        }
    }

// cout<< maxc1 << endl;
// cout<< maxc2 << endl;

  float dclass=0;
  vect1=MeanProj.Column(maxc1+1);
  vect2=MeanProj.Column(maxc2+1);

  dclass=Distance(vect1,vect2,method);
  distance=Distance(face1,face2,method);
  distance=distance*(1.0-fabs(dclass));

  return distance;
}


double Distance(ColumnVector & face1,ColumnVector & face2,string Method )
{

  double dist=0;
  ColumnVector vect1,vect2;
  if (Method=="normdist")
    {
      vect1<<face1/face1.norm_Frobenius();
      vect2<<face2/face2.norm_Frobenius();
      dist=(vect1-vect2).norm_Frobenius();
      vect1.ReleaseAndDelete();
      vect2.ReleaseAndDelete();
    }
  if (Method=="cos")
    {
      dist=-dotproduct(face1,face2)/(sqrt(face1.sum_square()*face2.sum_square()));
    }
  else
    if (Method=="L1")
      {
        dist=(face1-face2).sum_absolute_value();
      }
    else
      if (Method=="L2")
        {
          dist=(face1-face2).norm_Frobenius();
        }
  if (Method=="LogL")
    {
      int size=face1.Nrows();
      vect1=face1/face1.sum();
      vect2=face2/face2.sum();

//        double val;
      for (int i=1;i<=size;i++)
        {
          if (vect1(i)==0)
            vect1(i)=0.000001;
          if (vect2(i)==0)
            vect2(i)=0.000001;

          vect1(i)=log(vect1(i));
          vect2(i)=-vect2(i);
        }
      dist=SP(vect2,vect1).sum();

    }
  if (Method=="chi2")
    {
      int size=face1.Nrows();
      dist=0;
      vect1=face1/face1.sum();
      vect2=face2/face2.sum();
      for (int i=1;i<=size;i++)
        if (vect2(i) !=0 )
          dist+=((vect1(i)-vect2(i))*(vect1(i)-vect2(i)))/vect2(i);
    }
  if (Method=="HistInter")
    {
      int size=face1.Nrows();
      int count=0;
      dist=0;
      vect1=face1;
      vect2=face2;
      for (int i=1;i<=size;i++)
        if (max(vect1(i),vect2(i)) > 0)
          {
            dist+=min(vect1(i),vect2(i))/max(vect1(i),vect2(i));
            count++;
          }
      dist/=count;
    }
  if (Method=="L1NAN")
    {
      dist=0;
      for (int i=1;i<=face1.Nrows();i++)
        if (!isnan(face1(i)) && ! isnan(face2(i)))
          dist+=fabs(face1(i)-face2(i));
    }
  if (Method=="L2NAN")
    {
      dist=0;
      for (int i=1;i<=face1.Nrows();i++)
        if (!isnan(face1(i)) && ! isnan(face2(i)))
          dist+=(face1(i)-face2(i))*(face1(i)-face2(i));
      dist=sqrt(dist);
    }
  if (Method=="CosNAN")
    {

      int height=face1.Nrows();
      int c=0;
      for (int i=1;i<=height;i++)
        if (!isnan(face1(i)) && !isnan(face2(i)))
          c++;

      vect1.ReSize(c);
      vect2.ReSize(c);
//cout << facevect1.Nrows() << endl;
      int k=1;
      for (int i=1;i<=height;i++)
        if (!isnan(face1(i)) && !isnan(face2(i)))
          {
            vect1(k)=face1(i);
            vect2(k)=face2(i);
            k++;
          }
      dist=Distance(vect1,vect2,"cos");

    }

  vect1.ReleaseAndDelete();
  vect2.ReleaseAndDelete();
  return dist;
}


int GetNonNulsAmp(Matrix &SOURCE,Matrix &RES,double seuil)
{

  int height=SOURCE.Nrows();
  int width=SOURCE.Ncols();

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      if (SOURCE(i,j)<seuil)
        RES(i,j)=NAN;

  return 0;
}


ReturnMatrix GetProdadap(Matrix &Mat1,Matrix &Mat2)
{

  int height=Mat1.Nrows();
  int width=Mat1.Ncols();
//    double x,y;
  Matrix Res(height,width);

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      Res(i,j)=atan2(Mat1(i,j),Mat2(i,j));

  Res.Release();
  return Res;

}


ReturnMatrix GetDHistogram(Matrix &A,int histsize)
{
  Matrix Hist;
  int height=A.Nrows();
  int width=A.Ncols();
  int pos;
  double minimum,maximum;
// int histsize=int(ceil((A.maximum()-A.minimum())/step));
  minmax(A,minimum,maximum);
//cout << minimum << " " << maximum << endl;
  float step=(maximum-minimum)/(histsize-1);

  Hist.ReSize(histsize,2);
  Hist=0.0;
  if ( step==0.0)
    {
//cout << "test" << endl;
      Hist.Release();
      return Hist;
    }

  for (int i=1;i<=histsize;i++)
    Hist(i,2)=minimum+(i-1)*step;

  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (!isnan(A(i,j)))
          {
            pos=(int)floor((A(i,j)-minimum)/step)+1;
            Hist(pos,1)++;
          }
      }

//cout << A.maximum() << " " << A.minimum() <<" "<< step << endl;

  Hist.Release();
  return Hist;
}


ReturnMatrix SplitAndHist(Matrix &A,int histsize)
{
// acontinuer
  Matrix tmp(histsize,2),hist(histsize,2);

//Matrix Norm;
  int width=A.Ncols();
  int height=A.Nrows();
  int steph=(int)floor(height/10.0);
  int stepw=(int)floor(width/10.0);
  Matrix S(steph,stepw);
//Norm=MatrixRescale(A,0.0, 1.0);
//
//cout << "test" << endl;
  int k=0;
  for (int i=1;i<=height-steph+1;i=i+steph)
    {
//cout << i << " " << i+steph << endl;
      for (int j=1;j<=width-stepw+1;j=j+stepw)
        {
          S=A.submatrix(i,i+steph-1,j,j+stepw-1);
//S=MatrixRescale(S,0.0, 1.0);
          hist=GetDHistogram(S,histsize);
//cout << "hist" << endl;
          if (k==0)
            {
              tmp = hist;
//cout << S << endl;
//cout << tmp << endl;
//exit(-1);
            }
          else
            tmp = tmp & hist;

          k++;
        }
    }

//cout << k << endl;
  tmp.Release();
  return tmp;
}


void Codify(Matrix &Re,Matrix &Im,double &val,double &mask)
{
  int pos=1,neg=1;
  int height=Re.Nrows();
  int width=Im.Ncols();
//	Cod=0;
  mask=0;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (sqrt(Re(i,j)*Re(i,j)+Im(i,j)*Im(i,j)) > 1.5)
          {

            if (Re(i,j)>0)
              pos++;
            else
              neg++;

            if (Im(i,j)>0)
              pos++;
            else
              neg++;

            mask++;
          }
      }

// 	if (neg>pos)
// 		val=1;
// 	else
// 		val=0;
  val=(double)pos/(double)neg;

  mask/=height*width;
}


void Codify1(Matrix &Re,Matrix &Im,double &pos,double &neg,double &mask)
{
  pos=0;
  neg=0;
  int height=Re.Nrows();
  int width=Im.Ncols();
//	Cod=0;
  mask=0;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (sqrt(Re(i,j)*Re(i,j)+Im(i,j)*Im(i,j)) > 1.5)
          {

            if (Re(i,j)>0)
              pos++;
            else
              neg++;

            if (Im(i,j)>0)
              pos++;
            else
              neg++;

            mask++;
          }
      }

//     if (neg>pos)
//         val=1;
//     else
//         val=0;

  mask/=height*width;
}


void Codify2(Matrix &Re,Matrix &Im,double &pos1,double &pos2,double &pos3,double &pos4,double &mask)
{
  pos1=0;
  pos2=0;
  pos3=0;
  pos4=0;
  int height=Re.Nrows();
  int width=Im.Ncols();
//	Cod=0;
  mask=0;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (sqrt(Re(i,j)*Re(i,j)+Im(i,j)*Im(i,j)) > 1.5)
          {

            if (Re(i,j)>0 && Im(i,j) >0)
              pos1++;
            if (Re(i,j)<0 && Im(i,j) <0)
              pos2++;
            if (Re(i,j)<0 && Im(i,j) >0)
              pos3++;
            if (Re(i,j)>0 && Im(i,j) <0)
              pos4++;

            mask++;
          }
      }
}


void Codify3(Matrix &Re,Matrix &Im,double &pos1,double &pos2,double &mask)
{
  pos1=0;
  pos2=0;
  int height=Re.Nrows();
  int width=Im.Ncols();
//	Cod=0;
  mask=0;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (sqrt(Re(i,j)*Re(i,j)+Im(i,j)*Im(i,j)) > 1.5)
          {

            if (Re(i,j)*Im(i,j) >0)
              pos1++;
            else
              pos2++;

            mask++;
          }
      }
  mask/=height*width;
}


void Codify4(Matrix &Re,Matrix &Im,double &meanRe,double &meanIm,double &mask)
{
  meanRe=0.0;
  meanIm=0.0;
  int height=Re.Nrows();
  int width=Im.Ncols();
//	Cod=0;
  mask=0;
  for (int i=1;i<=height;i++)
    for (int j=1;j<=width;j++)
      {
        if (sqrt(Re(i,j)*Re(i,j)+Im(i,j)*Im(i,j)) > 1.5)
          {
            meanRe+=sqrt(Re(i,j)*Re(i,j));
            meanIm+=sqrt(Im(i,j)*Im(i,j));
            mask++;
          }
      }
  meanRe/=mask;
  meanIm/=mask;
  mask/=height*width;
}


ReturnMatrix SplitAndCode(Matrix &RealM,Matrix &ImM)
{
// acontinuer
  Matrix code(100,3);

//Matrix Norm;
  int width=RealM.Ncols();
  int height=RealM.Nrows();
  int steph=(int)floor(height/10.0);
  int stepw=(int)floor(width/10.0);
  Matrix R(steph,stepw);
  Matrix I(steph,stepw);
  int k=1;
  for (int i=1;i<=height-steph+1;i=i+steph)
    {
//cout << i << " " << i+steph << endl;
      for (int j=1;j<=width-stepw+1;j=j+stepw)
        {
          R=RealM.submatrix(i,i+steph-1,j,j+stepw-1);
          I=ImM.submatrix(i,i+steph-1,j,j+stepw-1);
          Codify4(R,I,code(k,1),code(k,2),code(k,3));
//Codify2(R,I,code(k,1),code(k,2),code(k,3),code(k,4),code(k,5));
//Codify3(R,I,code(k,1),code(k,2),code(k,3));
//Codify1(R,I,code(k,1),code(k,2),code(k,3));
          k++;
        }
    }

//cout << k << endl;
  code.Release();
  return code;
}


void SelectCommonNonNan(ColumnVector &In1,ColumnVector &In2,ColumnVector &out1,ColumnVector &out2)
{

  if (In1.Nrows() != In2.Nrows())
    {
      cout << "Dimension vectors mismatch" << endl;
      return;
    }
  else
    {
      int r=In1.Nrows();
      out1=In1;
      out2=In2;
      for (int i=1;i<=r;i++)
        if (isnan(In1(r)) || isnan(In2(r)))
          {
            out1(r)=NAN;
            out2(r)=NAN;
          }
    }

}


vector < vector < Matrix > > Split(Matrix &A,int steph,int stepw)
{
  vector < vector < Matrix > > Outsplit;
  vector < Matrix > tmpsplit;
  int width=A.Ncols();
  int height=A.Nrows();

  for (int i=1;i<=height-steph+1;i=i+steph)
    {
      for (int j=1;j<=width-stepw+1;j=j+stepw)
        tmpsplit.push_back(A.submatrix(i,i+steph-1,j,j+stepw-1));

      Outsplit.push_back(tmpsplit);
      tmpsplit.clear();
    }

  return Outsplit;
}


bool isNAN(Matrix &A)                                       // tous les element sans Nan
{

  int height=A.Nrows();
  int width=A.Ncols();
  int count=0;
  for (int i=1;i<=height;i++)
    for (int j=1; j<=width;j++)
      if (isnan(A(i,j)))
        count++;

  if (count==height*width)
    return true;
  else
    return false;
}


bool OneisNAN(Matrix &A)                                    // un seul element est NAN
{

  int height=A.Nrows();
  int width=A.Ncols();
  for (int i=1;i<=height;i++)
    for (int j=1; j<=width;j++)
      if (isnan(A(i,j)))
        return true;

  return false;
}


ReturnMatrix mindist(Matrix &faceref,Matrix &facetest)
{
//    double dist=MAXFLOAT;
  int height=faceref.Nrows();
  int width=faceref.Ncols();
  int steph=facetest.Nrows();
  int stepw=facetest.Ncols();
  Matrix S(steph,stepw);
  double ab;
  int count;

  Matrix Res(height-steph,width-stepw);

  Res=NAN;
  for (int i=1;i<=height-steph;i++)
    for (int j=1; j<=width-stepw;j++)
      {
        S=faceref.submatrix(i,i+steph-1,j,j+stepw-1);
//cout << S.Nrows()<< "  "<<S.Ncols() << endl;
        ab=0.0;
        count=0;

        if (!isNAN(S) && !isNAN(facetest))
          {
            for (int k=1;k<=steph;k++)
              for (int l=1;l<=stepw;l++)
                if (!isnan(S(k,l)) && !isnan(facetest(k,l)))
                  {
                    ab+=sqrt((S(k,l)-facetest(k,l))*(S(k,l)-facetest(k,l)));
                    count++;
                  }
            if (count>=(steph*stepw)/4.0)
              Res(i,j)=ab/count;
          }

      }

  Res.Release();
  return Res;

}


double Distance(Matrix & face1,Matrix & face2,string Method )
{
  double dist1=0.0,dist2=0.0,dist=0.0;
//cout << Method << endl;
  if (Method=="min" || Method=="max" || Method=="mean" )
    {
//	cout << "test" << endl;
      vector < vector < Matrix> > face1divRef,face2divRef;
      vector < vector < Matrix> > face1divTest,face2divTest;
      int posx,posy;
      face1divRef=Split(face1,21,21);
      face2divRef=Split(face2,21,21);
// face1divTest=Split(face1,5,5);
// face2divTest=Split(face2,5,5);
      int miniheight=5, miniwidth=5;

      int nvref=face1divRef.size();
      int nhref=face1divRef[0].size();
      int nvtest,nhtest;
      float displacement1=0,displacement2=0;
//face1 vs face2
      Matrix RES;
//    int c=0;
//  cout << "testisnan start" << endl;
      for (int i=0;i< nvref;i++)
        for ( int j=0;j<nhref;j++)
          {
            face2divTest=Split(face2divRef[i][j],miniheight,miniwidth);
            nvtest=face2divTest.size();
            nhtest=face2divTest[0].size();
            Matrix distances(nvtest,nhtest);
            Matrix diplace(nvtest,nhtest);
            Matrix diplacex(nvtest,nhtest);
            Matrix diplacey(nvtest,nhtest);
//         diplace=1.0 / 0.0;
//         diplacex=1.0 / 0.0;
//         diplacey=1.0 / 0.0;
            diplace=NAN;
            diplacex=NAN;
            diplacey=NAN;
            distances=NAN;
            for (int k=0;k< nvtest;k++)
              for ( int l=0;l<nhtest;l++)
                {
                  if (!isNAN(face2divTest[k][l]))
                    {
                      RES=mindist(face1divRef[i][j],face2divTest[k][l]);
                      if (!isNAN(RES))
                        {
                          distances(k+1,l+1)=min(RES,posx,posy);
                          diplacex(k+1,l+1)=(posx-1)-k*miniheight;
                          diplacey(k+1,l+1)=(posy-1)-l*miniwidth;
                          diplace(k+1,l+1)=sqrt(diplacex(k+1,l+1)*diplacex(k+1,l+1)+diplacey(k+1,l+1)*diplacey(k+1,l+1));
                        }

                    }
                }

            dist=mean(distances);
            if (!isnan(dist))
              dist1+=dist;
            displacement1+= mean(diplace);
            face2divTest.clear();
          }

      for (int i=0;i< nvref;i++)
        for ( int j=0;j<nhref;j++)
          {
            face1divTest=Split(face1divRef[i][j],miniheight,miniwidth);
            nvtest=face1divTest.size();
            nhtest=face1divTest[0].size();
            Matrix distances(nvtest,nhtest);
            Matrix diplace(nvtest,nhtest);
            Matrix diplacex(nvtest,nhtest);
            Matrix diplacey(nvtest,nhtest);
            for (int k=0;k< nvtest;k++)
              for ( int l=0;l<nhtest;l++)
                {
                  if (!isNAN(face1divTest[k][l]))
                    {
                      RES=mindist(face2divRef[i][j],face1divTest[k][l]);
                      if (!isNAN(RES))
                        {
                          distances(k+1,l+1)=min(RES,posx,posy);
                          diplacex(k+1,l+1)=(posx-1)-k*miniheight;
                          diplacey(k+1,l+1)=(posy-1)-l*miniwidth;
                          diplace(k+1,l+1)=sqrt(diplacex(k+1,l+1)*diplacex(k+1,l+1)+diplacey(k+1,l+1)*diplacey(k+1,l+1));
                        }
// 		else
// 			cout <<  "nan" << endl;
                    }
                }

            dist=mean(distances);
            if (!isnan(dist))
              dist2+=mean(distances);

            displacement2+= mean(diplace);
            face1divTest.clear();
          }

// cout << dist2 << endl;
//cout << displacement2 << endl;
      face1divRef.clear();
      face2divRef.clear();

      if (Method=="min")
        dist=min(dist1,dist2);

      if (Method=="mean")
        dist=(dist1+dist2)/2;

      if (Method=="max")
        dist=max(dist1,dist2);

      face1divRef.clear();
      face2divRef.clear();
      face1divTest.clear();
      face2divTest.clear();
    }

  if (Method=="MatL2NAN")
    {
      dist=0;
      int height=face1.Nrows();
      int width=face1.Ncols();

      for (int i=1;i<=height;i++)
        for (int j=1;i<=width;i++)
          if (!isnan(face1(i,j)) && ! isnan(face2(i,j)))
            dist+=(face1(i,j)-face2(i,j))*(face1(i,j)-face2(i,j));
      dist=sqrt(dist);

    }

  if (Method=="MatCosNAN")
    {

//    int height=face1.Nrows();
//    int width=face1.Ncols();
      ColumnVector vect1=Mat2Vect(face1);
      ColumnVector vect2=Mat2Vect(face2);
      dist=Distance(vect1,vect2,"CosNAN");
    }

  if (Method=="NewSplit" )
    {
//int posx,posy;

      vector < vector < Matrix> > face1divRef,face2divRef;
      vector < vector < Matrix> > face1divFFTRRef,face1divFFTIRef,face2divFFTRRef,face2divFFTIRef;
      vector < vector < Matrix> > face1divTest,face2divTest;
      vector < vector < Matrix> > face1divFFTRTest,face1divFFTITest,face2divFFTRTest,face2divFFTITest;

      face1divRef=Split(face1,21,21);
      face2divRef=Split(face2,21,21);
      Matrix FacConRes(21,21);
      int nvref=face1divRef.size();
//cout << nvref << endl;
      int nhref=face1divRef[0].size();
      int miniheight=5, miniwidth=5;
      int nvtest,nhtest;

//cout << "point0" << endl;
      GetFFTforgrid(face1divRef ,face1divFFTRRef,face1divFFTIRef);
//GetIFFTforgrid(face1divFFTRRef,face1divFFTIRef,face1divRef);
//exit (-1);
      GetFFTforgrid(face2divRef ,face2divFFTRRef,face2divFFTIRef);

//cout << "point1" << endl;
      for (int i=0;i< nvref;i++)
        for ( int j=0;j<nhref;j++)
          {
            face2divTest=Split(face2divRef[i][j],miniheight,miniwidth);
            face2divTest=Centerforgrid(face2divTest,21,21);
            GetFFTforgrid(face2divTest ,face2divFFTRTest,face2divFFTITest);

            cout << face1divRef[0][0] << endl;
            cout << face2divTest[0][0] << endl;
            exit(-1);
// GetIFFTforgrid(face2divFFTRTest,face2divFFTITest,face2divTest);
// exit(-1);
            nvtest=face2divTest.size();
            nhtest=face2divTest[0].size();

            for (int k=0;k< nvtest;k++)
              for ( int l=0;l<nhtest;l++)
                {
                  face2divFFTRTest[k][l]=SP(face2divFFTRTest[k][l],face1divFFTRRef[i][j]);
                  face2divFFTITest[k][l]=SP(face2divFFTITest[k][l],face1divFFTIRef[i][j]);
                }

            GetIFFTforgrid(face2divFFTRTest,face2divFFTITest,face2divTest);
            exit(-1);
            ColumnVector Res=GetMaxCorrfromgrid(face2divTest);
            dist+=Res.sum();
// 	    cout << Res << endl;
            Res.ReleaseAndDelete();
            face2divTest.clear();
            face2divFFTRTest.clear();
            face2divFFTITest.clear();
          }

    }

  if (Method=="CrossCorr" )
    {
//int posx,posy;

      vector < vector < Matrix> > face1divRef,face2divRef;
      vector < vector < Matrix> > face1divTest,face2divTest;
      Matrix Res;

//         int height=face1.Nrows();
//         int width=face1.Ncols();
      int rows=31;
      int cols=31;

      face1divRef=Split(face1,rows,cols);
      face2divRef=Split(face2,rows,cols);

      int nvref=face1divRef.size();
      int nhref=face1divRef[0].size();

      int count=0;
      if (1)
        {
          for (int i=0;i< nvref;i++)
            for ( int j=0;j<nhref;j++)
              {
//                // cout << face2divRef[i][j] << endl;
                Res=CrossCorrelationNAN(face1divRef[i][j],face2divRef[i][j]);
//cout << Res << endl;
                Res=abs(Res);
                dist1+=max(Res);
//    cout << max(Res) << endl;
                Res.ReleaseAndDelete();
                count++;
              }
        }
      else
        {
          int nvtest,nhtest;
          int miniheight=5, miniwidth=5;
          for (int i=0;i< nvref;i++)
            for ( int j=0;j<nhref;j++)
              {
                face2divTest=Split(face2divRef[i][j],miniheight,miniwidth);

                nvtest=face2divTest.size();
                nhtest=face2divTest[0].size();
                for (int k=0;k< nvtest;k++)
                  for ( int l=0;l<nhtest;l++)
                    {
                      Res=CrossCorrelationNAN(face1divRef[i][j],face2divTest[k][l]);
                      Res=abs(Res);
                      dist1+=max(Res);
                      count++;
                    }

                face2divTest.clear();

              }
        }
      dist1/=count;

      dist=dist1;
      face1divRef.clear();
      face2divRef.clear();

    }

  return dist;
}


// CrossValidate(Matrix & face1,Matrix & face2,int rows,int cols,int &nvref,int &nhref )
// {
// vector < float> tmp;
//
// }

vector < float>  CrossValidate(Matrix & face1,Matrix & face2,int rows,int cols,int &nvref,int &nhref )
{
  double dist1=0.0;                                       //,dist=0.0;
//int posx,posy;
  vector < float> tmp;
  vector < vector < Matrix> > face1divRef,face2divRef;
  vector < vector < Matrix> > face1divTest,face2divTest;
  Matrix Res;

  ColumnVector F1,F2;
  int count=0;
  if (0)
    {

      face1divRef=Split(face1,rows,cols);
      nvref=face1divRef.size();
      nhref=face1divRef[0].size();
      for (int i=0;i< nvref;i++)
        for ( int j=0;j<nhref;j++)
          {
//Res=
// 	    Res=CrossCorrelationNAN(face1divRef[i][j],face2divRef[i][j]);
//             Res=abs(Res);
//             dist1+=max(Res);
// 	    Res.ReleaseAndDelete();
            dist1=Distance(face1divRef[i][j],face2divRef[i][j],"MatCosNAN");
            tmp.push_back(dist1);
            count++;
          }
      face1divRef.clear();
      face2divRef.clear();
    }
  else
    {
      int height=face1.Nrows();
      int width=face1.Ncols();
//        int nvtest,nhtest;
      int miniheight=15, miniwidth=15;
      face2divRef=Split(face2,miniheight,miniwidth);
      nvref=face2divRef.size();
      nhref=face2divRef[0].size();
      int testheight=(int)ceil(miniheight/2);
      int testwidtht=(int)ceil(miniwidth/2);

//   rows=(int)ceil(rows/2);
//    cols=(int)ceil(cols/2);

//         cout << nvref << " " << nhref << endl;
//         cout << testheight << " " <<testwidtht << endl;
//         cout << rows << " " <<cols << endl;
      Matrix face1test;
      int up,down,left,right;
      for (int i=0;i< nvref;i++)
        {
          for ( int j=0;j<nhref;j++)
            {
              int posx=i*miniheight+testheight+1;
              int posy=j*miniwidth+testwidtht+1;

//  cout << posx << "x" <<posy << "\t";

              up=max(posx-miniheight-rows,1);
              down=min(posx+miniheight+rows,height);
              left=max(posy-miniwidth-cols,1);
              right=min(posy+miniwidth+cols,width);

              face1test=face1.submatrix(up,down,left,right);
//Res=CrossCorrelationNAN(face1test,face2divRef[i][j]);
              Res=HausedorfNAN(face1test,face2divRef[i][j]);

//                 cout << face1test << endl;
//                 cout << face2divRef[i][j] << endl;
//                 cout << Res << endl;
//
//                 exit(-1);
//Res=abs(Res);
              tmp.push_back(min(Res));
              count++;
              Res.ReleaseAndDelete();
              face1test.ReleaseAndDelete();
            }
//cout << endl;
        }
//exit(-1);
//         for (int i=0;i< nvref;i++)
//             for ( int j=0;j<nhref;j++)
//         {
//             face2divTest=Split(face2divRef[i][j],miniheight,miniwidth);
//             nvtest=face2divTest.size();
//             nhtest=face2divTest[0].size();
// //cout << nvtest <<" "<< nhtest << endl;
//             for (int k=0;k< nvtest;k++)
//                 for ( int l=0;l<nhtest;l++)
//             {
//                 Res=CrossCorrelationNAN(face1divRef[i][j],face2divTest[k][l]);
//                 Res=abs(Res);
//                 tmp.push_back(max(Res));
//                 count++;
//                 Res.ReleaseAndDelete();
//             }

//             face2divTest.clear();
      face2divRef.clear();
    }
// //}

//dist1/=count;

// dist=dist1;
//

//   cout << dist1 << endl;
  return tmp;
}


ReturnMatrix GetCentered(Matrix &A, int outrows,int outcols)
{

  Matrix CentredF;
  int height=(int)ceil(A.Nrows()/2);
  int width=(int)ceil(A.Ncols()/2);
  CentredF.ReSize(outrows,outcols);
  outrows=(int)ceil(outrows/2+0.5);
  outcols=(int)ceil(outcols/2+0.5);
  CentredF=0.0;
  for (int i=outrows-height;i<=outrows+height;i++)
    for (int j=outcols-width;j<=outcols+width;j++)
      CentredF(i,j)=A(i-(outrows-height)+1,j-(outcols-width)+1);

//cout << CentredF << endl;
//exit(-1);
  CentredF.Release();
  return CentredF;
}


ReturnMatrix GetMaxCorrfromgrid(vector < vector <Matrix > > &face1)
{

  int nvref=(int)face1.size();
  int nhref=(int)face1[0].size();
  ColumnVector Res(nvref*nhref);
  int k=1;
  for (int i=0;i< nvref;i++)
    for ( int j=0;j<nhref;j++)
      {
        Res(k)=max(face1[i][j]);
        k++;
      }

  Res.Release();
  return Res;
}


vector < vector <Matrix > > Centerforgrid(vector < vector <Matrix > > &face1,int rowout,int colout)
{
  vector < vector <Matrix > >  faceout;
  vector <Matrix > tmpface;
  Matrix Centred;
  int nvref=(int)face1.size();
  int nhref=(int)face1[0].size();
  for (int i=0;i< nvref;i++)
    {
      for ( int j=0;j<nhref;j++)
        {
          Centred=GetCentered(face1[i][j],rowout,colout);
//Centred=face1[i][j];
          tmpface.push_back(Centred);
          Centred.ReleaseAndDelete();
        }
      faceout.push_back(tmpface);
      tmpface.clear();
    }
  return faceout;
}


void GetFFTforgrid(vector < vector <Matrix > > &face1 ,vector < vector <Matrix > > &face1FFTRRef,vector < vector <Matrix > > &face1FFTIRef)
{
  int nvref=face1.size();
  int nhref=face1[0].size();
  int height=face1[0][0].Nrows();
  int width=face1[0][0].Ncols();
  vector < Matrix> FFTR,FFTI;
  Matrix Noface(height,width),FFTFaceR(height,width),FFTFaceI(height,width);
  Noface=0.0;
  face1FFTRRef.clear();
  face1FFTIRef.clear();
  for (int i=0;i< nvref;i++)
    {
      for ( int j=0;j<nhref;j++)
        {
          FFT2(face1[i][j],Noface,FFTFaceR,FFTFaceI);
          FFTR.push_back(FFTFaceR);
          FFTI.push_back(FFTFaceI);
          FFTFaceR.ReleaseAndDelete();
          FFTFaceI.ReleaseAndDelete();
        }
      face1FFTRRef.push_back(FFTR);
      face1FFTIRef.push_back(FFTI);
      FFTR.clear();
      FFTI.clear();
    }
}


void GetIFFTforgrid(vector < vector <Matrix > > &face1FFTRRef,vector < vector <Matrix > > &face1FFTIRef,vector < vector <Matrix > > &face1)
{
  face1.clear();
  int nvref=face1FFTRRef.size();
  int nhref=face1FFTRRef[0].size();
  int height=face1FFTRRef[0][0].Nrows();
  int width=face1FFTRRef[0][0].Ncols();
  vector < Matrix> FFTR;
  Matrix Noface(height,width),FFTFaceR(height,width);

  for (int i=0;i< nvref;i++)
    {
      for ( int j=0;j<nhref;j++)
        {
          FFT2I(face1FFTRRef[i][j],face1FFTIRef[i][j],FFTFaceR,Noface);
          FFTR.push_back(FFTFaceR);
          FFTFaceR.ReleaseAndDelete();
          cout << Noface << endl;
        }
      face1.push_back(FFTR);
      FFTR.clear();
    }
}


float norm_Frobenius(Matrix &A)
{
  int height=A.Nrows();
  int width=A.Ncols();
  float res=0;
  int count=0;
  for (int i=1;i<= height;i++)
    for ( int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        {
          res+=A(i,j)*A(i,j);
          count++;
        }

  if (count >0)
    return sqrt(res)/count;
  else
    return NAN;
}


float Sum(Matrix &A)
{
//cout << "test" << endl;
  int height=A.Nrows();
  int width=A.Ncols();
  float res=0;
  for (int i=1;i<= height;i++)
    for ( int j=1;j<=width;j++)
      if (!isnan(A(i,j)))
        res+=A(i,j);

//cout << res << endl;
  return res;
}


void GetCommonNonNan(Matrix &A,Matrix &B,Matrix &Aout,Matrix &Bout)
{
  int height=A.Nrows();
  int width=A.Ncols();
  Matrix Btmp,Atmp;
  Btmp=B;
  Atmp=A;
  for (int i=1;i<= height;i++)
    for ( int j=1;j<=width;j++)
      if (isnan(A(i,j)) || isnan(B(i,j)) )
        {
          Atmp(i,j)=NAN;
          Btmp(i,j)=NAN;
        }

//cout << res << endl;
  Bout=Btmp;
  Aout=Atmp;
  Btmp.ReleaseAndDelete();
  Atmp.ReleaseAndDelete();
}


ReturnMatrix CrossCorrelationNAN(Matrix &A,Matrix&B)
{
  int height=A.Nrows();
  int width=A.Ncols();
  int rows=B.Nrows();
  int cols=B.Ncols();
  int maskrows=(int)ceil(rows/2);
  int maskcols=(int)ceil(cols/2);
  Matrix CentredA,C;                                      //=A-Abar;

  Matrix CentredB=B-mean(B);
  Matrix CentredBbis;
  float smask=norm_Frobenius(CentredB);
  Matrix Res(height,width);
  Res=0.0;

  if (smask>0)
    {
      float simage;
      for (int i=1;i<= height;i++)
        for ( int j=1;j<=width;j++)
          {
            if ((i-maskrows)>0 && (j-maskcols)>0 && (i+maskrows)<=height && (j+maskcols)<=width  )
              {

                CentredA=A.submatrix(i-maskrows,i+maskrows,j-maskcols,j+maskcols);
// for evry loop verify the common NAN
                GetCommonNonNan(CentredA,B,CentredA,CentredBbis);
                CentredBbis-=mean(CentredBbis);
                smask=norm_Frobenius(CentredBbis);
                if (smask>0)
                  {
                    CentredA-=mean(CentredA);
                    simage=norm_Frobenius(CentredA);

                    if (simage>0)
                      {
                        C=SP(CentredA,CentredBbis);
                        Res(i,j)=Sum(C)/(simage*smask);
                        C.ReleaseAndDelete();
                      }
                    else
                      Res(i,j)=0.0;
                    CentredA.ReleaseAndDelete();
                  }
                else
                  Res(i,j)=0.0;
                CentredBbis.ReleaseAndDelete();
              }
          }
    }

//cout << Res << endl;
  Res.Release();
  return Res;

}


ReturnMatrix CrossCorrelation(Matrix &A,Matrix&B)
{
  int height=A.Nrows();
  int width=A.Ncols();
  int rows=B.Nrows();
  int cols=B.Ncols();
  int maskrows=(int)ceil(rows/2);
  int maskcols=(int)ceil(cols/2);
  Matrix CentredA,C;                                      //=A-Abar;

  Matrix CentredB=B-mean(B);
  float smask=norm_Frobenius(CentredB);
  Matrix Res(height,width);
  Res=0.0;

  if (smask>0)
    {
      float simage;
      for (int i=1;i<= height;i++)
        for ( int j=1;j<=width;j++)
          {
            if ((i-maskrows)>0 && (j-maskcols)>0 && (i+maskrows)<=height && (j+maskcols)<=width  )
              {

                CentredA=A.submatrix(i-maskrows,i+maskrows,j-maskcols,j+maskcols);
                CentredA-=mean(CentredA);
                simage=norm_Frobenius(CentredA);

                if (simage>0)
                  {
                    C=SP(CentredA,CentredB);
                    Res(i,j)=Sum(C)/(simage*smask);
                    C.ReleaseAndDelete();
                  }
                else
                  Res(i,j)=0.0;
                CentredA.ReleaseAndDelete();
              }
          }
    }

  Res.Release();
  return Res;

}


ReturnMatrix HausedorfNAN(Matrix &A,Matrix&B)
{
  int height=A.Nrows();
  int width=A.Ncols();
  int rows=B.Nrows();
  int cols=B.Ncols();
  int maskrows=(int)ceil(rows/2);
  int maskcols=(int)ceil(cols/2);
  Matrix CentredA,C;                                      //=A-Abar;
  Matrix CentredBbis;
// float smask=norm_Frobenius(CentredB);
  Matrix Res(height,width);
  Res=NAN;

  for (int i=1;i<= height;i++)
    for ( int j=1;j<=width;j++)
      {
        if ((i-maskrows)>0 && (j-maskcols)>0 && (i+maskrows)<=height && (j+maskcols)<=width  )
          {

            CentredA=A.submatrix(i-maskrows,i+maskrows,j-maskcols,j+maskcols);
            GetCommonNonNan(CentredA,B,CentredA,CentredBbis);
            C=CentredA-CentredBbis;
            Res(i,j)=norm_Frobenius(C);
            CentredA.ReleaseAndDelete();
            CentredBbis.ReleaseAndDelete();
          }
      }

  Res.Release();
  return Res;

}


int ComputeEigens( Matrix & CM, DiagonalMatrix & EigValues,Matrix & EigVectors )
{

//cout << CM << endl;
  SVD(CM,EigValues,EigVectors);
  SortSV(EigValues,EigVectors, false );
  return 0;
}


int SPComplexe(Matrix &RealIn1,Matrix &ImIn1,Matrix &RealIn2,Matrix &ImIn2,Matrix &Realout,Matrix &ImOut)
{
  int height=RealIn1.Nrows();
  int width=RealIn1.Ncols();
  if (height!=ImIn1.Nrows() || height!=RealIn2.Nrows() || height!=ImIn2.Nrows() || width!=ImIn1.Ncols() || width!=RealIn2.Ncols()|| width!=ImIn2.Ncols())
    {
      cout << "Matrix dimensions mismatch for Complexe product" << endl;
      exit(-1);
    }
  else
    {
      Realout.ReSize(height,width);
      ImOut.ReSize(height,width);
      Realout=SP(RealIn1,RealIn2)-SP(ImIn1,ImIn2);
      ImOut=SP(RealIn1,ImIn2)+SP(RealIn2,ImIn1);
    }

  return 0;
}

ReturnMatrix FFTshift(Matrix &Source)
{
  int height=Source.Nrows();
  int width=Source.Ncols();
  int hheight=(int)ceil(height/2.0);
  int hwidth=(int)ceil(width/2.0);
  //  cout << hheight << "x" << hwidth<< endl;
  Matrix Res(height,width);
  Res=0.0;
  Res.submatrix(1, hheight,1,hwidth)=Source.submatrix(hheight, height,hwidth,width);
  Res.submatrix(hheight+1, height,1,hwidth)=Source.submatrix(1, hheight-1,hwidth,width);

  Res.submatrix(1, hheight,hwidth+1,width)=Source.submatrix(hheight, height,1,hwidth-1);
  Res.submatrix(hheight+1, height,hwidth+1,width)=Source.submatrix(1, hheight-1,1,hwidth-1);
  /*
       cout << "test4" << endl;*/
  Res.Release();
  return Res;

}

ReturnMatrix GetShiftedFFTAmp(Matrix &A)
{
//filter size=2*sizeGAUSS+1
  Matrix AMP;
  int height=A.Nrows()+1;
  int width=A.Ncols()+1;
  Matrix ImpairA(height,width);
  ImpairA=0;
  ImpairA.submatrix(1, height-1,1,width-1)=A;
///compute FFT(A)
  Matrix FFTFaceReal,FFTFaceIM,FFTPRODR,FFTPRODI;
  Matrix Noface(height,width);
  Noface=0.0;
  FFT2(ImpairA, Noface, FFTFaceReal,FFTFaceIM);
  ImpairA=SP(FFTFaceReal,FFTFaceReal)+SP(FFTFaceIM,FFTFaceIM);
  ImpairA=sqrt(ImpairA);

  ImpairA=FFTshift(ImpairA);
//     Matrix ShiftedAmp(height-1,width-1);
//     ShiftedAmp=ImpairA.submatrix(2, height,2,width);
  ImpairA.Release();

  return ImpairA;

}
