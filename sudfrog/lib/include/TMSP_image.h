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
#ifndef PFC_IMAGE_H
#define PFC_IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <vector>


#include "jpegutilities.h"
#include "miscellaneous.h"
#include "TMSP_image.h"
#include "mask.h"

#define CENTER 0
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4


#define Between(var, value, delta) ((var >= (value - delta)) && (var <= (value + delta)))

using namespace std;
/**
 @author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/

namespace TMSP_Face_Space
{


	/** \brief Supported Light correction */
	enum LightEnhance
	{
		Im_NoEnhance=0 /**< no Light correction */,
		Im_Histogram /**<Histogram Equalization */,
		Im_Gamma /**<Gamma correction*/,
		Im_Log /**<logarithmic correction */,
		Im_AnisSmooth /**<Anisotropic smoothing */,
		Im_MultiRetinex /**<MultiRetinex \b Not-Implemented  */,
		Im_Pers /**<CLS (correction of light by symmetry) \b Not-Implemented */,
	};

	/** \brief Image pixels format  */
	enum ImFormat
	{
		Im_8=1 /**< (1 Byte by pixel) **/,
		Im_16/**< (2 Byte by pixel) **/,
		Im_24/**< (3 Byte by pixel) **/,
		Im_32/**< (4 Byte by pixel) **/,
	};

	/** \brief Structure for storing the coordinates of Eyes, Nose and Mouth location in the original image */
	struct Face_Coordinates
	{
		int LeftEyeCenterX /** The X Coordinate of the center's left Eye  */,
		LeftEyeCenterY /** The Y Coordinate of the center's left Eye  */,
		RightEyeCenterX /** The X Coordinate of the center's right Eye  */,
		RightEyeCenterY /** The Y Coordinate of the center's right Eye  */,
		NoseCenterX /** The X Coordinate of the center's nose  */,
		NoseCenterY /** The Y Coordinate of the center's nose */,
		MouthCenterX /** The X Coordinate of the center's mouth  */,
		MouthCenterY /** The Y Coordinate of the center's mouth */;
	};

	/** \brief structure for storing a point */
	struct PfcPoint
	{
		int x /** The X Coordinate */ ,y /** The Y Coordinate */;
	};

	/**
	* \brief Class that allows the manipulation of image for the baseline.
	*/
	class Pfc_Image
	{
		public:
			/**
			 * Constructor
			 */
			Pfc_Image();

			/**
			 * Constructor that initialize the class by loading image from a file
			 * @param filename  file name of the image to be loaded (supported format JPEG, PGM and PPM)
			 */
			Pfc_Image ( const char *filename );
			/**
			 * Constructor that initialize the class from a memory block
			 * @param Dataptr pointer to the image data
			 * @param width width of the image
			 * @param height height of the image
			 * @param Bytebp number of Byte by pixel
			 */


			Pfc_Image ( uint8_t *Dataptr,int width,int height,int Bytebp );
			/**
			 * Constructor that initialize the class from a memory block supposing that the image is in grey level
			 * @param Dataptr pointer to the image data
			 * @param width width of the image
			 * @param height height of the image
			 */
			Pfc_Image ( uint8_t *Dataptr,int width,int height );
			/**
			 * Constructor that initialize the class by allocating a memory block of size=w*h
			 * @param w width of the image
			 * @param h height of the image
			 */
			Pfc_Image ( int w,int h );
			/**
			 * Destructor
			 */
			~Pfc_Image();
			/**
			 *
			 * @return the image width
			 */
			int GetImWidth();
			/**
			 *
			 * @return  the image height
			 */
			int GetImHeight();

			/**
			 *
			 * @return the data size = Width*Height*Bytebp
			 */
			int GetImSize();
			/**
			 * set the width of the image
			 * @param w width
			 */
			void SetImWidth ( int w );
			/**
			 * set the height of the image
			 * @param h height
			 */
			void SetImHeight ( int h );
			/**
			 * set the width and the height of the image
			 * @param w width
			 * @param h height
			 */
			void SetImDim ( int w,int h );
			/**
			 *
			 * set the width, the height and the bytesbypixel of the image
			 * @param w width
			 * @param h height
			 * @param Bb bytesbypixel
			 */
			void SetImParam ( int w,int h,int Bb );
			/**
			 * Set the bytesbypixel of the image
			 * @param Bb bytesbypixel
			 */
			void SetImBytebp ( int Bb );
			/**
			 *
			 * @return the bytesbypixel of the image
			 */
			int GetImBytebp();
			/**
			 *
			 * @return the maximum value of the gray scale image
			 */
			uint8_t maximum();
			/**
			 *
			 * @return the minimum value of the gray scale image
			 */
			uint8_t minimum();
			/**
			 * allocate a memory space for the image (width*height*Bytebp)
			 * @param width width of the image
			 * @param height height of the image
			 * @param Bytebp bytesbypixel of the image
			 * @return
			 */
			int AllocImData ( int width,int height,int Bytebp );
			/**
			 *
			 * @return the pointer to the image data
			 */
			uint8_t *GetDataptr();

			/**
			 * Set the value of the image pixel at a specific pointer position
			 * @param pos the pointer position
			 * @param val the value to be set
			 * @return
			 */
			void SetData ( int pos,uint8_t val );
			/**
			 * Get the value of a pixel at a specific pointer position
			 * @param pos he pointer position
			 * @return the value of a pixel
			 */
			uint8_t GetData ( int pos );
			/**
			 * fill the image to the specified pointer (don't copy the data but just point to it)
			 * @param Dataptr the pointer to the image data
			 * @param width width of the image
			 * @param height height of the image
			 * @param Bytebp bytesbypixel of the image
			 */
			void SetImData ( uint8_t *Dataptr,int width,int height,int Bytebp );
			/**
			 * fill the image with the same data of another Pfc_Image (don't copy just point is quicker)
			 * @param o another Pfc_Image object
			 * @return this
			 */
			Pfc_Image & operator= ( const Pfc_Image & o );
			/**
			 * fill the image object with the same data of another Pfc_Image object (allocate then copy the data object)
			 * @param o another Pfc_Image object
			 */
			void copy ( const Pfc_Image & o );

			/**
			 * stretch the histogram of the image
			 * @param method two values are available
			   \li 0 : do a min-max stretching
			   \li 1: center the histogram to 128 and standard deviation of 50
			 */
			void Stretch ( int method );

			/**
			 * stretching the histogram of the Masked image
			 * @param method  two values are available 0 and 1
			 * @param ROI the mask to be applied
			 */
			void Stretch ( int method, Mask &ROI );
			/**
			 * Histogramm Equalization
			 */
			void Histeq();
			/**
			 *
			 * @return a vector of the pixels values distribution
			 */
			std::vector<int> GetHistogram();
			/**
			 *
			 * @return the Cumulative histogramm of the image
			 */
			std::vector<int> GetCumHistogramme();

			/**
			 * Rotate image by +90°
			 */
			void Rotate90 ();


			/**
			 * Set the data of the object by reading an image from file
			 * @param filename the filename of the image
			 * @return 0 if well done
			 */
			int ReadImage ( const char *filename );

			/**
			 * Set the data of the object by reading an image from file specifying the extension of the file
			 * @param filename the filename of the image
			 * @param ext the extension or format values are JPG, PGM or PPM
			 * @return  0 if well done
			 */
			int ReadImage ( const char *filename,Extension ext );

			/**
			 * Set the data of the object by reading an image from PPM file
			 * @param filename the filename of the image
			 * @return  0 if well done
			 */
			int ReadPpm ( const char *filename );

			/**
			 * Set the data of the object by reading an image from PGM file
			 * @param filename the filename of the image
			 * @return 0 if well done
			 */
			int ReadPgm ( const char *filename );

			/**
			 * Set the data of the object by reading an image from JPEG file
			 * @param filename the filename of the image
			 * @return 0 if well done
			 */
			int ReadJpeg ( const char *filename );

			/**
			 * Correct illumination problem using a specified method
			 * @param method the illumination method values are ("NONE","HISTOGRAM","GAMMA","LOG" or "AS")
			 * @param param parameters of the method
			 * @return a pointer to one or two Pfc_Image objects (depends on the chosen LightEnhance method)
			 */
			Pfc_Image * LightCorrect ( string method,double* param );

			/**
			 * Correct illumination problem using a specified method and a Mask
			 * @param methodname the name of the illumination method values are ("NONE","HISTOGRAM","GAMMA","LOG" or "AS")
			 * @param param parameters of the method
			 * @param ROI a Mask to specify the Region Of Interest
			 * @return a pointer to one or two Pfc_Image object (depends on the chosen LightEnhance method)
			 */
			Pfc_Image *LightCorrect ( string methodname,double* param,Mask ROI );

			/**
			 * Correct illumination problem using a specified method
			 * @param method the LightEnhance method
			 * @param param parameters of the method
			 * @return a pointer to one or two Pfc_Image objects (depends on the chosen LightEnhance method)
			 */
			Pfc_Image * LightCorrect ( LightEnhance method,double* param );


			/**
			 * Correct illumination problem using a specified method and a Mask
			 * @param method the LightEnhance method
			 * @param param parameters of the method
			 * @param ROI a Mask to specify the Region Of Interest
			 * @return a pointer to one or two Pfc_Image object (depends on the chosen LightEnhance method)
			 */
			Pfc_Image *LightCorrect ( LightEnhance method,double* param ,Mask ROI );
			/**
			 * save the image to file
			 * @param filename the output filename
			 */
			void SaveImage ( char*  filename );
			/**
			 *
			 * @return the sum square of the pixels values
			 */
			float sum_square();
			/**
			 *
			 * @return  the mean of the pixels values
			 */
			float mean();
			/**
			 *
			 * @return the standard deviation of the pixels values
			 */
			float std();

			/**
			 * return the value of a pixel at the image coordinates i and j
			 * @param i the row coordinate
			 * @param j the column coordinate
			 * @return the value of the pixel
			 */
			uint8_t operator () ( int i,int j );


			/**
			 * \brief return a pointer to a Matrix from Im_8 Pfc_Image data
			 * @return Matrix
			 */
			ReturnMatrix MatFromPFCImage ( );


		private:

			uint8_t *Data;
			bool allocated;
			int  width,height,Bytebp;
			int EnhanceMethod;
	};



	/**
	 * \brief return a pointer to a Pfc_Image from Matrix data
	 * @param A Matrix Object
	 * @return Pfc_Image object of type Im_8
	 */
	Pfc_Image PFCImageFromMat ( Matrix &A );

	/**
	 * \brief apply the Anisotropic smoothing
	 * @param InputImage a Pfc_Image to be treated
	 * @param steps number of iterations for the method
	 * @param lambda controlling coefficient
	 * @param ReflectImage returned reflectance component
	 * @param LightImage returned illumination component
	 * @return 0 when well done
	 */
	int ASNorm ( Pfc_Image &InputImage,int steps, float lambda, Pfc_Image &ReflectImage, Pfc_Image &LightImage );
	/**
	 * \brief apply the Anisotropic smoothing with histogramm correction using a ROI and mean and std
	 * @param InputImage a Pfc_Image to be treated
	 * @param steps number of iterations for the method
	 * @param lambda controlling coefficient
	 * @param ReflectImage returned reflectance component
	 * @param LightImage returned illumination component
	 * @param ROI Mask of the region of interest
	 * @param meanref the mean value to center the reflectance component value between (0-255)
	 * @param std the std value to strech the histogram of the reflectance component value between (30-60)
	 * @return 0 when well done
	 */
	int ASNorm ( Pfc_Image &InputImage,int steps, float lambda, Pfc_Image &ReflectImage, Pfc_Image &LightImage, Mask ROI, int meanref,float std
	           );
}
#endif
