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
#include "TMSP_image.h"

#define swap(a,b) { \
        int c = (a);    \
        (a) = (b);      \
        (b) = c;        \
    }


#define MEANREF 128
#define STDREF 80.0

namespace TMSP_Face_Space
{


	Pfc_Image::Pfc_Image()
	{
		Data=NULL;
		allocated=false;
		EnhanceMethod=Im_NoEnhance;
		SetImParam ( 0,0,1 );
	}

	Pfc_Image::Pfc_Image ( const char *filename )
	{
		Data=NULL;
		allocated=false;
		EnhanceMethod=Im_NoEnhance;
		ReadPgm ( filename );
	}

	Pfc_Image::Pfc_Image ( uint8_t *Dataptr,int w,int h,int Bbp )
	{
		Data=NULL;
		allocated=false;
		SetImData ( Dataptr,w,h,Bbp );
	};

	Pfc_Image::Pfc_Image ( uint8_t *Dataptr,int w,int h )
	{
		Data=NULL;
		allocated=false;
		SetImData ( Dataptr,w,h,1 );
	};

	Pfc_Image::Pfc_Image ( int w,int h )
	{
		Data=NULL;
		allocated=false;
		AllocImData ( w,h,Im_8 );
	};

	Pfc_Image::~Pfc_Image()
	{
		if ( allocated )
			delete []Data;
	}

	int Pfc_Image::ReadImage ( const char *filename )
	{
		allocated=true;
		return ReadImage ( filename,DetectExtension ( ( string ) filename ) );

	}

	int Pfc_Image::ReadImage ( const char *filename,Extension ext )
	{
		allocated=true;
		switch ( ext )
		{
			case JPG:
				return ReadJpeg ( filename );
				break;
			case PPM:
				return ReadPpm ( filename );
				break;
			case PGM:
				return ReadPgm ( filename );
				break;
			case unknown:
				cerr << "Extension not supported yet!!" << endl;
				exit ( -1 );
				break;
		}
	}

	int Pfc_Image::ReadPgm ( const char *filename )
	{
		FILE *fp;
//	int r,c;
		char buff[256];
//	int gray;
		int pgmtype;
		int colorLevel;

		if ( ( fp = fopen ( filename,"rb" ) ) == NULL )
		{
			printf ( "File does not exist!\n" );                //bad source file name
			return -1;
//	exit(0);
		}
		else
		{
			if ( fgetc ( fp ) !='P' )                           //check if the source file is in correct format
			{
				printf ( "Not a PGM file!\n" );
				fclose ( fp );
				return -1;
			}
			else
			{
				fgets ( buff,256,fp );                          //inputs the first line in the image (starting from 'P')
				if ( buff[0]=='2' )
				{
					pgmtype=2;
				}
				else
				{
					pgmtype=5;
				}

				while ( 1 )
				{
					fgets ( buff,256,fp );                      //comments are stored in char array comments
					if ( buff[0]!='#' )
					{
						sscanf ( buff,"%d %d",&width,&height );
						break;
					}
				}

				fgets ( buff,256,fp );                          //read max grey level
				sscanf ( buff,"%d",&colorLevel );

				if ( Data )
					delete []Data;

				if ( pgmtype==5 )                               //binary file
				{
					Data = new unsigned char[width*height];               //assign memory to source and target arrays
					fread ( Data,sizeof ( unsigned char ),width*height,fp );
					fclose ( fp );                               //close the file pointer
					return 0;

				}
				else
				{

					fclose ( fp );
					return -1;
				}

			}
		}
	}

	int Pfc_Image::ReadPpm ( const char *filename )
	{
		FILE *fp;
//	int r,c;
		char buff[256];
//	int gray;
		int ppmtype;
		int colorLevel;

		if ( ( fp = fopen ( filename,"rb" ) ) == NULL )
		{
			printf ( "File does not exist!\n" );                //bad source file name
			return -1;
//	exit(0);
		}
		else
		{
			if ( fgetc ( fp ) !='P' )                           //check if the source file is in correct format
			{
				printf ( "Not a PPM file!\n" );
				fclose ( fp );
				return -1;
			}
			else
			{
				fgets ( buff,256,fp );                          //inputs the first line in the image (starting from 'P')
				if ( buff[0]=='2' )
				{
					ppmtype=3;
				}
				else
				{
					ppmtype=6;
				}

				while ( 1 )
				{
					fgets ( buff,256,fp );                      //comments are stored in char array comments
					if ( buff[0]!='#' )
					{
						sscanf ( buff,"%d %d",&width,&height );
						break;
					}
				}

				fgets ( buff,256,fp );                          //read max grey level
				sscanf ( buff,"%d",&colorLevel );

				if ( Data )
					delete []Data;

				Data = new unsigned char[width*height*3];               //assign memory to source and target arrays

				if ( ppmtype==6 )                               //binary file
				{
					fread ( Data,sizeof ( unsigned char ),width*height*3,fp );
					fclose ( fp );                               //close the file pointer
					return 0;

				}
				else
				{

					fclose ( fp );
					return -1;
				}

			}
		}
	}

	int Pfc_Image::ReadJpeg ( const char *filename )
	{
		int orient;
		int res=GetJpegImage ( filename,Data,width,height,Bytebp,orient );

		//printf ( "Orientation %d in the jpeg header of image %s\n",orient,filename );

		if ( orient !=0 && orient !=1 )
		{
			if ( orient==8 )
			{
				//printf ( "Orientation detected in the jpeg header of image %s : %d\n",filename,orient );
				Rotate90();
			}
			else
			{
				printf ( "Orientation detected in the jpeg header of image %s : %d rotation not handled for this orientation\n",filename,orient );
				return -1;
			}
		}

		return res;
	}

	void Pfc_Image::Rotate90 ()
	{

		unsigned char *tmpbuff = new unsigned char[height *width* Bytebp];
		memcpy ( tmpbuff,Data,height *width* Bytebp );

		swap ( width,height );

		for ( int i=0;i<width;i++ )
			for ( int j=0;j<height;j++ )
				for ( int c=0;c<Bytebp;c++ )
					Data[Bytebp* ( ( height-j-1 ) *width+i ) +c] = tmpbuff[Bytebp* ( i*height+j ) +c ];

		delete []tmpbuff;
	}



	void Pfc_Image::SaveImage ( char*  filename )
	{
		FILE *pFile;

		pFile=fopen ( filename, "wb" );
		if ( pFile )
		{
			// Write header
			switch ( GetImBytebp() )
			{
				case 1:
					fprintf ( pFile, "P5\n%d %d\n255\n", GetImWidth(), GetImHeight() );
					break;

				case 3:
					fprintf ( pFile, "P6\n%d %d\n255\n", GetImWidth(), GetImHeight() );
					break;
			}

			fwrite ( GetDataptr(), 1, GetImWidth() *GetImHeight() *GetImBytebp(), pFile );
			// Close file
			fclose ( pFile );
		}
		else
			cerr << "Could not open file"<<endl;

	}

	int Pfc_Image::GetImSize()
	{

		return width*height*Bytebp;
	}

	int Pfc_Image::GetImWidth()
	{
		return width;
	};
	int Pfc_Image::GetImHeight()
	{
		return height;
	};
	void Pfc_Image::SetImWidth ( int w )
	{
		width=w;
	};
	void Pfc_Image::SetImHeight ( int h )
	{
		height=h;
	};
	void Pfc_Image::SetImDim ( int w,int h )
	{
		width=w;
		height=h;
	};
	void Pfc_Image::SetImParam ( int w,int h,int Bb )
	{
		width=w;
		height=h;
		Bytebp=Bb;
	};
	void Pfc_Image::SetImBytebp ( int Bb )
	{
		Bytebp=Bb;
	};

	int Pfc_Image::GetImBytebp()
	{
		return Bytebp;
	}

	void Pfc_Image::SetData ( int pos,uint8_t val )
	{
		Data[pos]=val;
	};

	uint8_t Pfc_Image::GetData ( int pos )
	{
		return Data[pos];
	};

	uint8_t Pfc_Image::maximum()
	{
		uint8_t maxval;
		maxval=0;
		for ( int i=0; i < width*height; ++i )
		{
			if ( maxval<Data[i] )
				maxval=Data[i];
		}
		return maxval;
	}

	uint8_t Pfc_Image::minimum()
	{
		uint8_t minval;
		minval=255;
		for ( int i=0; i < width*height; ++i )
		{
			if ( minval>Data[i] )
				minval=Data[i];
		}
		return minval;
	}


	int  Pfc_Image::AllocImData ( int w,int h,int Bbp )
	{
		SetImParam ( w,h,Bbp );
		if ( allocated )
			delete []Data;

		Data=new uint8_t[Bytebp*width*height];
		if ( !Data )
		{
			cerr << "@Pfc_Image::AllocImData: Error allocating memory";
			exit ( -1 );
		}
		allocated=true;
	}


	uint8_t *Pfc_Image::GetDataptr()
	{
		return Data;
	};

	void Pfc_Image::SetImData ( uint8_t *Dataptr,int w,int h,int Bbp )
	{

		SetImParam ( w,h,Bbp );

		if ( !Dataptr )
		{
			Data=NULL;
			return;
		}

		Data=Dataptr;
	};

	Pfc_Image & Pfc_Image::operator= ( const Pfc_Image &o )
	{
		SetImData ( o.Data,o.width,o.height,o.Bytebp );
		return * ( this );
	}

	void Pfc_Image::copy ( const Pfc_Image & o )
	{
		SetImParam ( o.width,o.height,o.Bytebp );
		if ( !o.Data )
		{
			Data=NULL;
			return;
		}
		if ( !Data )
			delete []Data;
		Data=new uint8_t[Bytebp*width*height];
		//  allocated=true;
		memcpy ( Data,o.Data,o.width*o.height*o.Bytebp );

		//return *(this);
	}


	void Pfc_Image::Stretch ( int method )
	{
		float val;
		float lamda=50.0;
		float meanval=128.0;
		switch ( method )
		{

			case 1:
			{
				//stretch hist (min 0 max 255)
				float minval=1000;
				float maxval=-1000;
				for ( int i=0; i < width*height; ++i )
				{
					if ( minval>Data[i] )
						minval=Data[i];
					else
						if ( maxval<Data[i] )
							maxval=Data[i];
				}

				for ( int i=0; i < width*height; ++i )
				{
					val= 255.0* ( ( float ) GetData ( i )-minval ) / ( maxval-minval );
					//cout << val << endl;
					if ( val <0 )
						SetData ( i,0 );
					else
						if ( val >255 )
							SetData ( i,255 );
						else
							SetData ( i, ( uint8_t ) val );
				}
				//exit(-1);
				break;

			}
			case 2:
			{
//align hist (mean 128 and std=lamda)
				float mean=0;
				float std=0;
				for ( int i=0; i < width*height; ++i )
				{
					mean+=Data[i];
					std+=Data[i]*Data[i];
				}

				mean=mean/ ( width*height );
				std=sqrt ( std/ ( width*height )-mean*mean );


				for ( int i=0; i < width*height; ++i )
				{
					val= ( meanval + lamda* ( Data[i]-mean ) /std );
					if ( val <0 )
						SetData ( i,0 );
					else
						if ( val >255 )
							SetData ( i,255 );
						else
							SetData ( i, ( uint8_t ) val );
				}

				break;
			}
		}
		//return 0;
	}

	void Pfc_Image::Stretch ( int method, Mask &ROI )
	{
		float val;
		float lamda=80.0;
		float meanval=128.0;
		int count=0;
		switch ( method )
		{

			case 1:
			{
				//stretch hist (min 0 max 255)
				float minval=1000;
				float maxval=-1000;

				for ( int i=0; i < width; ++i )
					for ( int j=0;j<height;j++ )
					{
						if ( ROI.Ismask ( j+1,i-2 ) )
						{
							if ( minval>Data[count] )
								minval=Data[count];
							else
								if ( maxval<Data[count] )
									maxval=Data[count];
						}
						count++;
					}

				count=0;
				for ( int i=0; i < width; ++i )
					for ( int j=0;j<height;j++ )
					{
						if ( ROI.Ismask ( j+1,i-2 ) )
						{
							val= 255.0* ( ( float ) GetData ( count )-minval ) / ( maxval-minval );
							//cout << val << endl;
							if ( val <0 )
								SetData ( count,0 );
							else
								if ( val >255 )
									SetData ( count,255 );
								else
									SetData ( count, ( uint8_t ) val );

						}
						else
							SetData ( count,0 );
						count++;
					}
// 				//exit(-1);
				break;

			}
			case 2:
			{
//align hist (mean 128 and std=lamda)
				float mean=0;
				float std=0;
				for ( int i=0; i < width; ++i )
					for ( int j=0;j<height;j++ )
					{
						if ( ROI.Ismask ( j+1,i-2 ) )
						{
							mean+=Data[count];
							std+=Data[count]*Data[count];
						}
						count++;
					}

				mean=mean/count;
				std=sqrt ( ( std/count )-mean*mean );

				count=0;
				for ( int i=0; i < width; ++i )
					for ( int j=0;j<height;j++ )
					{
						if ( ROI.Ismask ( j+1,i-2 ) )
						{
							val= ( meanval + lamda* ( Data[count]-mean ) /std );
							if ( val <0 )
								SetData ( count,0 );
							else
								if ( val >255 )
									SetData ( count,255 );
								else
									SetData ( count, ( uint8_t ) val );

						}
						else
							SetData ( count,0 );
						count++;
					}

				break;
			}
		}
		//return 0;
	}


	void Pfc_Image::Histeq()
	{
		std::vector<int> CumHistIm=GetCumHistogramme();
		int maxgray=CumHistIm.size()-1;
		int total=CumHistIm[maxgray];
		//Pfc_Image EQ(height,width);

		for ( int i=0; i<height*width; i++ )
			SetData ( i, ( uint8_t ) ( maxgray* ( float ) CumHistIm[ ( int ) GetData ( i ) ]/ ( float ) total ) );

	}

	/**
	 *
	 * @return
	 */
	vector<int> Pfc_Image::GetHistogram()
	{
		std::vector < int > Imhistogram;
		int max = maximum();

		Imhistogram.assign ( max+1,0 );

		for ( int i=0; i<height*width; i++ )
			( Imhistogram[ ( int ) GetData ( i ) ] ) ++;

		return Imhistogram;
	}
// //
// //
	/**
	 *
	 * @return
	 */
	vector<int> Pfc_Image::GetCumHistogramme()
	{
		vector <int> CumHist;
		vector <int> Hist=GetHistogram();
		int sizehist=Hist.size();
		CumHist.push_back ( Hist[0] );
		for ( int i=1;i<sizehist;i++ )
			CumHist.push_back ( CumHist[i-1]+Hist[i] );

		return CumHist;
	}

	Pfc_Image *Pfc_Image::LightCorrect ( string methodname,double* param,Mask ROI )
	{
		LightEnhance NormLight;
		strupr ( ( char * ) methodname.c_str() );
		if ( methodname=="NONE" )
			NormLight=Im_NoEnhance;
		else if ( methodname=="HISTOGRAM" )
			NormLight=Im_Histogram;
		else if ( methodname=="GAMMA" )
			NormLight=Im_Gamma;
		else if ( methodname=="LOG" )
			NormLight=Im_Log;
		else if ( methodname=="AS" )
			NormLight=Im_AnisSmooth;
		else if ( methodname=="MRETINEX" )
			NormLight=Im_MultiRetinex;
		else if ( methodname=="PERSO" )
			NormLight=Im_Pers;
		else
		{
			cerr << "unkown Light correctness method use (\"NONE\",\"HISTOGRAM\",\"GAMMA\",\"LOG\"or \"AS\")  " << endl;
			exit ( -1 );
		}

		return LightCorrect ( NormLight,param,ROI );
	}

	Pfc_Image *Pfc_Image::LightCorrect ( string methodname,double* param )
	{
		LightEnhance NormLight;
		strupr ( ( char * ) methodname.c_str() );
		if ( methodname=="NONE" )
			NormLight=Im_NoEnhance;
		else if ( methodname=="HISTOGRAM" )
			NormLight=Im_Histogram;
		else if ( methodname=="GAMMA" )
			NormLight=Im_Gamma;
		else if ( methodname=="LOG" )
			NormLight=Im_Log;
		else if ( methodname=="AS" )
			NormLight=Im_AnisSmooth;
		else if ( methodname=="MRETINEX" )
			NormLight=Im_MultiRetinex;
		else if ( methodname=="PERSO" )
			NormLight=Im_Pers;
		else
		{
			cerr << "Unkown Light correctness method use (\"NONE\",\"HISTOGRAM\",\"GAMMA\",\"LOG\"or \"AS\")  " << endl;
			exit ( -1 );
		}

		return LightCorrect ( NormLight,param );
	}

	Pfc_Image *Pfc_Image::LightCorrect ( LightEnhance NormLight,double* param,Mask ROI )
	{
		Pfc_Image *Norms=NULL;
		int width=GetImWidth();
		int height=GetImHeight();
		int argc=sizeof ( param );
		vector< float > Gam;
		float max=-1000;
		float min=1000;
		float val;
		std::vector<int> CumHistIm;
		int maxgray,total;

		switch ( NormLight )
		{

			case Im_NoEnhance:
				Norms = new Pfc_Image ( width,height );
				Norms = this;
				break;
			case Im_Histogram:
				Norms = new Pfc_Image ( width,height );

				CumHistIm=GetCumHistogramme();
				maxgray=CumHistIm.size()-1;
				total=CumHistIm[maxgray];
				for ( int i=0; i<height*width; i++ )
					Norms->SetData ( i, ( uint8_t ) ( maxgray* ( float ) CumHistIm[ ( int ) GetData ( i ) ]/
					                                  ( float ) total ) );
				Norms->Stretch ( 1 );
				break;

			case Im_Gamma:

				if ( argc<1 || !param )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_Gamma,param#NULL)"<< endl;
					return NULL;
				}
				Norms = new Pfc_Image ( width,height );
				max=pow ( 1.0, ( double ) param[0] );
				for ( int i=0;i<256;i++ )
				{
					val=pow ( ( double ) i/255.0, ( double ) param[0] );
					Gam.push_back ( 255.0*val/max );
				}

				for ( int i=0;i<width*height;i++ )
					Norms->SetData ( i, ( uint8_t ) Gam[GetData ( i ) ] );

				Gam.clear();
				Norms->Stretch ( 2 );
				break;

			case Im_Log:
				Norms = new Pfc_Image ( width,height );
				min=log ( 1.0/256.0 );
				for ( int i=0;i<256;i++ )
					Gam.push_back ( -255.0* ( log ( ( i+1 ) /256.0 )-min ) /min );

				for ( int i=0;i<width*height;i++ )
					Norms->SetData ( i, ( uint8_t ) Gam[GetData ( i ) ] );
				Gam.clear();

				break;

			case Im_AnisSmooth:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				if ( argc<2 )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_AnisSmooth,param#NULL)"<< endl;
					return NULL;
				}
				ASNorm ( *this, ( int ) param[0],param[1],Norms[0],Norms[1],ROI,MEANREF,STDREF );
				break;

			case Im_MultiRetinex:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				cerr << "Sorry not yet implemented!!" << endl;
				return NULL;

				break;

			case Im_Pers:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				if ( argc<2 )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_AnisSmooth,param#NULL)"<< endl;
					return NULL;
				}
				ASNorm ( *this, ( int ) param[0],param[1],Norms[0],Norms[1] );


				break;
		}
		return Norms;
	}

	Pfc_Image *Pfc_Image::LightCorrect ( LightEnhance NormLight,double* param )
	{
		Pfc_Image *Norms=NULL;
		int width=GetImWidth();
		int height=GetImHeight();
		int argc=sizeof ( param );
		vector< float > Gam;
		float max=-1000;
		float min=1000;
		float val;
		std::vector<int> CumHistIm;
		int maxgray,total;

		switch ( NormLight )
		{

			case Im_Histogram:
				Norms = new Pfc_Image ( width,height );

				CumHistIm=GetCumHistogramme();
				maxgray=CumHistIm.size()-1;
				total=CumHistIm[maxgray];
				for ( int i=0; i<height*width; i++ )
					Norms->SetData ( i, ( uint8_t ) ( maxgray* ( float ) CumHistIm[ ( int ) GetData ( i ) ]/
					                                  ( float ) total ) );
				Norms->Stretch ( 1 );
				break;

			case Im_Gamma:

				if ( argc<1 || !param )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_Gamma,param#NULL)"<< endl;
					return NULL;
				}
				Norms = new Pfc_Image ( width,height );
				max=pow ( 1.0, ( double ) param[0] );
				for ( int i=0;i<256;i++ )
				{
					val=pow ( ( double ) i/255.0, ( double ) param[0] );
					Gam.push_back ( 255.0*val/max );
				}

				for ( int i=0;i<width*height;i++ )
					Norms->SetData ( i, ( uint8_t ) Gam[GetData ( i ) ] );

				Gam.clear();
				Norms->Stretch ( 2 );
				break;

			case Im_Log:
				Norms = new Pfc_Image ( width,height );
				min=log ( 1.0/256.0 );
				for ( int i=0;i<256;i++ )
					Gam.push_back ( -255.0* ( log ( ( i+1 ) /256.0 )-min ) /min );

				for ( int i=0;i<width*height;i++ )
					Norms->SetData ( i, ( uint8_t ) Gam[GetData ( i ) ] );
				Gam.clear();

				break;

			case Im_AnisSmooth:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				if ( argc<2 )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_AnisSmooth,param#NULL)"<< endl;
					return NULL;
				}
				ASNorm ( *this, ( int ) param[0],param[1],Norms[0],Norms[1] );
				break;

			case Im_MultiRetinex:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				cerr << "Sorry not yet implemented!!" << endl;
				return NULL;

				break;

			case Im_Pers:
				Norms = new Pfc_Image[2];
				Norms[0].AllocImData ( width,height,Im_8 );
				Norms[1].AllocImData ( width,height,Im_8 );
				if ( argc<2 )
				{
					cerr << "Error: Pfcface::EnhanceLight(Im_AnisSmooth,param#NULL)"<< endl;
					return NULL;
				}
				ASNorm ( *this, ( int ) param[0],param[1],Norms[0],Norms[1] );


				break;
		}
		return Norms;
	}


	float Pfc_Image::sum_square()
	{
		float val=0;
		if ( Bytebp==Im_8 )
		{
			for ( int i=0;i<height*width;i++ )
				val+= ( float ) GetData ( i ) *GetData ( i );

			return val;                       //=sum/(height*width);
		}
		else
			return -1;


	}

	float Pfc_Image::mean()
	{
		float val=0;
		if ( Bytebp==Im_8 )
		{
			for ( int i=0;i<height*width;i++ )
				val+= ( float ) GetData ( i );

			return val/ ( height*width );                   //=sum/(height*width);
		}
		else
			return -1;

	}

	float Pfc_Image::std()
	{
		float meanv=mean();
		return sqrt ( sum_square() / ( height*width ) - meanv*meanv );
	}


	uint8_t Pfc_Image::operator () ( int row,int col )
	{
		return GetData ( row*width+col );
	}

	ReturnMatrix Pfc_Image::MatFromPFCImage ( )
	{
		int width=GetImHeight();
		int height=GetImWidth();
		Matrix Ret ( height,width );

		int count=0;
		for ( int i=1;i<=height;i++ )
			for ( int j=1;j<=width;j++ )
			{
				Ret ( i,j ) = ( float ) GetData ( count );
				count++;
			}

		Ret.Release();
		return Ret;
	}


	Pfc_Image PFCImageFromMat ( Matrix &A )
	{
		int width=A.Ncols();
		int height=A.Nrows();
		Pfc_Image Ret ( height,width );

		int count=0;

		for ( int i=1;i<=height;i++ )
			for ( int j=1;j<=width;j++ )
			{
				Ret.SetData ( count, ( uint8_t ) A ( i,j ) );
				count++;
			}

		return Ret;
	}



}

