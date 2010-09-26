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
#include "miscellaneous.h"

using namespace std;
namespace TMSP_Face_Space
{

	string applicationname ( const char *arg )
	{

		size_t found;
		string newfile,str=arg;
		found=str.find_last_of ( "/" );
		if ( found!=string::npos )
			newfile=str.substr ( found +1 );
		else
			newfile=str;

		return newfile;
	}


	string applicationpath ( const char *arg )
	{

		size_t found;
		string newfile="./",str=arg;
		found=str.find_last_of ( "/" );
		if ( found!=string::npos )
			newfile=str.substr ( 0,found+1 );

		return newfile;
	}



	string ChangeFileExt ( string str,string newext )
	{

		size_t found;
		string newfile;
		found=str.find_last_of ( "." );
		if ( found!=string::npos )
			newfile=str.substr ( 0,found +1 ) +newext;
		else
			newfile=str+ "."+newext;

		return newfile;
	}

	int GetComparisonList ( const char * filename, vector <string> &ListRef,vector <string> &ListTest )
	{
		ListRef.clear();
		ListTest.clear();
		string test;
		char refim[256],testim[256];
		int count=0;
		ifstream fileIN ( filename );
		while ( fileIN.good() )
		{
			getline ( fileIN,test );
			sscanf ( test.c_str(),"%s %s",& refim ,& testim );
			count++;
			if ( refim!="" && testim!="" )
			{
				ListRef.push_back ( ( string ) refim );
				ListTest.push_back ( ( string ) testim );
			}
		}
		fileIN.close();
		return ( int ) ListTest.size();
	}

	int InitialSimMatrix ( ofstream & Sim, char M, char * Queryfilename, char * Targetfilename, int QueryNum, int TargetNum )
	{
		Sim.write ( "%c2\n", M );
		Sim.write ( Targetfilename, strlen ( Targetfilename ) );
		Sim.put ( '\n' );
		Sim.write ( Queryfilename, strlen ( Queryfilename ) );
		Sim.put ( '\n' );
		Sim.write ( "MF ", 3 );

		char tmp[255];
		memset ( tmp, '\0', 255 );
		sprintf ( tmp, "%d %d ", QueryNum,TargetNum );
		Sim.write ( tmp, strlen ( tmp ) );
		int gtest=0x12345678;
		Sim.write ( ( char* ) ( &gtest ),sizeof ( int ) );
		Sim.put ( '\n' );
		return 0;
	}




	void SaveFrame ( double *buffd, int width, int height,char*  filename )
	{
		FILE *pFile;
		//char szFilename[32];
//		int y;
		// Open file
		int size=width* height;
		pFile=fopen ( filename, "wb" );
		if ( pFile )
		{
			uint8_t *buff=new uint8_t[size];

			float minval=MAXFLOAT;
			float maxval=MINFLOAT;

			for ( int i=0;i<size;i++ )
			{
				if ( minval > buffd[i] )
					minval = ( float ) buffd[i];

				if ( maxval < buffd[i] )
					maxval = ( float ) buffd[i];

			}

			cout << minval<< "x" <<maxval << endl;
			for ( int i=0;i<size;i++ )
				buff[i]= ( uint8_t ) ( 255.0* ( ( float ) buffd[i]-minval ) / ( maxval-minval ) );

			// Write header
			fprintf ( pFile, "P5\n%d %d\n255\n", width, height );
			fwrite ( buff, 1, size, pFile );
			delete []buff;
			// Close file
			fclose ( pFile );
		}
		else
			cerr << "Could not open file"<<endl;

	}


	void SaveFrame ( uint8_t *buff, int width, int height,int bpp,char*  filename )
	{
		FILE *pFile;
		//char szFilename[32];
//		int y;
		// Open file
		pFile=fopen ( filename, "wb" );
		if ( pFile )
		{
			// Write header
			switch ( bpp )
			{
				case 1:
					fprintf ( pFile, "P5\n%d %d\n255\n", width, height );
					break;

				case 3:
					fprintf ( pFile, "P6\n%d %d\n255\n", width, height );
					break;
			}

			fwrite ( buff, 1, height*width*bpp, pFile );
			// Close file
			fclose ( pFile );
		}
		else
			cerr << "Could not open file"<<endl;

	}



	void SaveFrame ( Matrix &buff,char*  filename )
	{
		FILE *pFile;
		//char szFilename[32];
		int count=0;
		int width=buff.Ncols();
		int height=buff.Nrows();
		uint8_t *data;
		data=new uint8_t[width*height];

		for ( int i=1;i<=height;i++ )
			for ( int j=1;j<=width;j++ )
			{
				data[count]= ( uint8_t ) buff ( i,j );
				count++;
			}

		// Open file
		pFile=fopen ( filename, "wb" );
		if ( pFile )
		{
			// Write header
			fprintf ( pFile, "P5\n%d %d\n255\n", width, height );
			fwrite ( ( uint8_t * ) data, 1, width*height, pFile );
			// Close file
			fclose ( pFile );
			delete []data;
		}
		else
			cerr << "Could not open file"<<endl;

	}

	void AddDirEnding ( string & dir )
	{
		if ( dir.c_str() [dir.length() - 1] != '/' )
			dir += "/";
	}


	void CreateDir ( const string & dir )
	{
		int pos = dir.find ( "/" );
		if ( pos == ( int ) string::npos )
			mkdir ( dir.c_str(), 0xfff );

		while ( pos != ( int ) string::npos )
		{
			string base ( dir, 0, pos );
			mkdir ( base.c_str(), 0xfff );
			pos = dir.find ( "/", pos + 1 );
		}
	}


	void CreateDirForFile ( const string & file )
	{
		int pos = file.rfind ( "/" );
		if ( pos != ( int ) string::npos )
		{
			string dir ( file, 0, pos );
			AddDirEnding ( dir );
			cout << "Creating directory " << dir << endl;
			CreateDir ( dir );
		}
	}

	void CreateDirForFile ( const string & file,bool feed )
	{
		int pos = file.rfind ( "/" );
		if ( pos != ( int ) string::npos )
		{
			string dir ( file, 0, pos );
			AddDirEnding ( dir );
			if ( feed )
				cout << "Creating directory " << dir << endl;

			CreateDir ( dir );
		}
	}


	bool DoesFileExists ( const char * fileName )
	{
		ifstream testFile;
		testFile.open ( fileName );
		if ( !testFile.is_open() )
		{
			return false;
		}
		testFile.close();
		return true;
	}

	uint8_t GetY ( uint8_t R,uint8_t G, uint8_t B )
	{
		uint8_t val1=max ( max ( R,G ),B );
		uint8_t val2=min ( min ( R,G ),B );
		float val= ( val1+val2 ) /2.0;
		if ( val>255 )
			return ( 255 );
		else
			return ( ( uint8_t ) val );
	}

	void strupr ( char *p )
	{
		while ( *p )
			* ( p++ ) = toupper ( *p );
	}


	Extension DetectExtension ( string filename )
	{
		Extension ext=unknown;

		size_t found;
		string exten;
		found=filename.find_last_of ( "." );

		if ( found!=string::npos )
		{
			exten=filename.substr ( found+1 );
			strupr ( ( char * ) exten.c_str() );
			if ( exten=="JPG" || exten=="JPEG" )
				return JPG;
			else if ( exten=="PGM" )
				return PGM;
			else if ( exten=="PPM" )
				return PPM;
		}
		return ext;

	}



	void fftshift2D ( double *source,double *dist,int rows,int cols )
	{

		double *tmp=new double[cols*rows];
		int halfh= ( int ) rows/2;
		int halfw= ( int ) cols/2;

//1st quadrant --> 4 quadrant

		for ( int h=0;h<halfh;h++ )
			for ( int l=0;l<halfw;l++ )
				tmp[ ( l+halfw ) + ( h+halfh ) *cols]=source[l+h*cols];

//2 quadrant --> 3 quadrant
		for ( int h=0;h<halfh;h++ )
			for ( int l=halfw;l<cols;l++ )
				tmp[ ( l-halfw ) + ( h+halfh ) *cols]=source[l+h*cols];

//3 quadrant --> 2 quadrant
		for ( int h=halfh;h<rows;h++ )
			for ( int l=0;l<halfw;l++ )
				tmp[ ( l+halfw ) + ( h-halfh ) *cols]=source[l+h*cols];


//4 quadrant --> 1 quadrant
		for ( int h=halfh;h<rows;h++ )
			for ( int l=halfw;l<cols;l++ )
				tmp[ ( l-halfw ) + ( h-halfh ) *cols]=source[l+h*cols];

		if ( !dist )
			dist=new double[cols*rows];

		if ( !dist )
		{
			cerr << "Error allocating more memory" << endl;
			exit ( -1 );
		}

		memcpy ( dist,tmp,cols*rows*sizeof ( double ) );

		delete []tmp;
	}

	double interpData ( double*src,int width,int height, int yl, int yu,int xl,int xu )
	{

		double total = 0.f;
		int pos,count=0;
		for ( int y = yl; y <= yu; ++y )
		{
			for ( int x = xl; x <= xu; ++x )
			{
				pos=y*width + x;
				if ( pos < width*height  && pos >= 0 && y>=0 && x>=0 )
				{
					total += src[pos];
					count++;
				}
			}
		}

		return  total/count;
	}

}
