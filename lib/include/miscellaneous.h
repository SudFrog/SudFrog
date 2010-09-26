/**************************************************************************
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

#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>


#include "newmatap.h"
#include "Matrix_util.h"
#include "newmatio.h"


namespace TMSP_Face_Space
{


	/** \brief Image Extension of supported image format */
	enum Extension
	{
		unknown=0 /** Unkown format*/,JPG /** Jpeg format */,PGM /** PGM format */ ,PPM/** PPM format */,
	};
	/**
	 *
	 * @param filename
	 * @return Extension
	 */
	Extension DetectExtension ( string filename );

	/**
	 *
	 * @param arg
	 * @return
	 */
	string applicationname ( const char *arg );
	/**
	 *
	 * @param arg
	 * @return
	 */
	string applicationpath ( const char *arg );

	/**
	 *
	 * @param filename
	 * @param ListRef
	 * @param ListTest
	 * @return
	 */
	int GetComparisonList ( const char * filename, vector <string> &ListRef,vector <string> &ListTest );
	/**
	 *
	 * @param str
	 * @param newext
	 * @return
	 */
	string ChangeFileExt ( string str,string newext );
	/**
	 *
	 * @param buff
	 * @param width
	 * @param height
	 * @param bpp
	 * @param filename
	 */
	void SaveFrame ( uint8_t *buff, int width, int height,int bpp,char*  filename );
	/**
	 *
	 * @param buffd
	 * @param width
	 * @param height
	 * @param filename
	 */
	void SaveFrame ( double *buffd, int width, int height,char*  filename );
	/**
	 *
	 * @param buff
	 * @param filename
	 */
	void SaveFrame ( Matrix &buff, char*  filename );


	/**
	 *
	 * @param Sim
	 * @param M
	 * @param Queryfilename
	 * @param Targetfilename
	 * @param QueryNum
	 * @param TargetNum
	 * @return
	 */
	int InitialSimMatrix ( ofstream & Sim, char M, char * Queryfilename, char * Targetfilename, int QueryNum, int TargetNum );

	/**
	 *
	 * @param fileName
	 * @return
	 */
	bool DoesFileExists ( const char * fileName );
	/**
	 *
	 * @param dir
	 */
	void AddDirEnding ( string & dir );
	/**
	 *
	 * @param dir
	 */
	void CreateDir ( const string & dir );
	/**
	 *
	 * @param file
	 */
	void CreateDirForFile ( const string & file );
	/**
	 *
	 * @param file
	 * @param feed
	 */
	void CreateDirForFile ( const string & file,bool feed );


	/**
	 * \brief the pixel intensity in the HSV colorspace
	 * @param R uint8 0<red< 255
	 * @param G uint8 0<green< 255
	 * @param B uint8 0<blue<255
	 * @return uint8  0< value<255
	 */
	uint8_t GetY ( uint8_t R,uint8_t G, uint8_t B );

	/**
	* \brief return a Capital letter of the string
	*/
	void strupr ( char *p );

	bool IsEven ( int i,int j,int width );
	void gaussSeidel ( float *Result, float *Source, int InputHeight,int InputWidth, float lambda );
	void ComputeRho ( float *rho, float *image, int InputHeight,int InputWidth, int posx, int posy );
	void weber ( float *rho, float *image, int InputHeight,int InputWidth, int posx, int posy, int position );

	double interpData ( double*src,int width,int height, int yl, int yu,int xl,int xu );
	void fftshift2D ( double *source,double *dist,int rows,int cols );

}
#endif

