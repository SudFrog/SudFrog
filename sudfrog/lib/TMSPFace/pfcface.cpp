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
#include "pfcface.h"

namespace TMSP_Face_Space
{

	PFCface::PFCface()
	{
		//Face_pts;
		NormLight=Im_Histogram;
		EyesDistance=50;
		Pfc_mask.Init ( 50 );
		Face_pts.LeftEyeCenterX=0;
		Face_pts.LeftEyeCenterY=0;
		Face_pts.RightEyeCenterX=0;
		Face_pts.RightEyeCenterY=0;
		Face_pts.NoseCenterX=0;
		Face_pts.NoseCenterY=0;
		Face_pts.MouthCenterX=0;
		Face_pts.MouthCenterY=0;
		GeomNorm.SetImData ( NULL,0,0,1 );
		OriginalGray.SetImData ( NULL,0,0,1 );
		LightNorm.SetImData ( NULL,0,0,1 );
		Reserved.SetImData ( NULL,0,0,1 );
	}


	PFCface::~PFCface()
	{
	}

	PFCface & PFCface::operator= ( const PFCface &o )
	{
		SetFaceCoordinate ( o.Face_pts );
		Original.copy ( o.Original );
		OriginalGray.copy ( o.OriginalGray );
		GeomNorm.copy ( o.GeomNorm );
		LightNorm.copy ( o.LightNorm );
		Reserved.copy ( o.Reserved );

		return * ( this );
	}

	void PFCface::SetNormLight ( int Norm )
	{
		NormLight=Norm;
	};

	int PFCface::GetNormLight()
	{
		return NormLight;
	};

	void PFCface::SetEyeDistance ( int Eydist )
	{
		EyesDistance=Eydist;
		Pfc_mask.Init ( EyesDistance );
	};

	int PFCface::GetEyeDistance()
	{
		return EyesDistance;
	};


	void PFCface::LoadOriginal ( const char* filename )
	{

		Pfc_Image face;
		face.ReadImage ( filename );
		SetOriginal ( face );

	}

	void PFCface::LoadOriginal ( const char* filename,Extension ext )
	{

		Pfc_Image face;
		face.ReadImage ( filename,ext );
		SetOriginal ( face );
	}

	void PFCface::SetOriginal ( Pfc_Image &face )
	{
		GeomNorm.SetImData ( NULL,0,0,1 );
		Original=face;
		int w=face.GetImWidth();
		int h=face.GetImHeight();
		int size=w*h;
		switch ( face.GetImBytebp() )
		{
			case Im_8:
				OriginalGray=face;
				break;
			case Im_24:
				OriginalGray.AllocImData ( w,h,Im_8 );
				for ( int i=0;i<size;i++ )
					OriginalGray.SetData ( i,GetY ( face.GetData ( 3*i ),face.GetData ( 3*i+1 ),face.GetData ( 3*i+2 ) ) );
				break;
		}

	};



	void PFCface::SetFaceCoordinate ( Face_Coordinates Fc )
	{
		Face_pts.LeftEyeCenterX=Fc.LeftEyeCenterX;
		Face_pts.LeftEyeCenterY=Fc.LeftEyeCenterY;
		Face_pts.RightEyeCenterX=Fc.RightEyeCenterX;
		Face_pts.RightEyeCenterY=Fc.RightEyeCenterY;
		Face_pts.NoseCenterX=Fc.NoseCenterX;
		Face_pts.NoseCenterY=Fc.NoseCenterY;
		Face_pts.MouthCenterX=Fc.MouthCenterX;
		Face_pts.MouthCenterY=Fc.MouthCenterY;
	};

	void PFCface::SetFaceCoordinate ( int LEX,int LEY,int REX,int REY,
	                                  int NX,int NY,int MX,int MY )
	{
		Face_pts.LeftEyeCenterX=LEX;
		Face_pts.LeftEyeCenterY=LEY;
		Face_pts.RightEyeCenterX=REX;
		Face_pts.RightEyeCenterY=REY;
		Face_pts.NoseCenterX=NX;
		Face_pts.NoseCenterY=NY;
		Face_pts.MouthCenterX=MX;
		Face_pts.MouthCenterY=MY;
	};

	int PFCface::LightCorrect ( string methodname,double* param,Mask ROI )
	{
		Pfc_Image *Lightenhanced;
		strupr ( ( char * ) methodname.c_str() );

		if ( methodname=="NONE" )
		{
			LightNorm=GeomNorm;
			return 0;
		}

		Lightenhanced=GeomNorm.LightCorrect ( methodname,param,ROI );

		if ( methodname=="HISTOGRAMME" || methodname=="GAMMA" || methodname=="LOG" )
			LightNorm=Lightenhanced[0];
		else if ( methodname=="AS" || methodname=="MRETINEX" || methodname=="PERSO" )
		{
			LightNorm=Lightenhanced[0];
			Reserved=Lightenhanced[1];
		}
		return 0;
	}

	int PFCface::LightCorrect ( string methodname,double* param )
	{
		Pfc_Image *Lightenhanced;

		strupr ( ( char * ) methodname.c_str() );

		if ( methodname=="NONE" )
		{
			LightNorm=GeomNorm;
			return 0;
		}

		Lightenhanced=GeomNorm.LightCorrect ( methodname,param );

		if ( methodname=="HISTOGRAMME" || methodname=="GAMMA" || methodname=="LOG" )
			LightNorm=Lightenhanced[0];
		else if ( methodname=="AS" || methodname=="MRETINEX" || methodname=="PERSO" )
		{
			LightNorm=Lightenhanced[0];
			Reserved=Lightenhanced[1];
		}

		return 0;
	}

	int PFCface::NormGeometric()
	{
		uint8_t *tmpNImage=NULL;

		int heightNorm= ( int ) ( 2.56*EyesDistance +0.5f );
		int widthNorm= ( int ) ( 2.56*EyesDistance +0.5f );
		GeomNorm.AllocImData ( heightNorm,widthNorm,Im_8 );
		LightNorm.AllocImData ( heightNorm,widthNorm,Im_8 );
		Reserved.AllocImData ( heightNorm,widthNorm,Im_8 );

		tmpNImage=new uint8_t[GeomNorm.GetImHeight() *GeomNorm.GetImWidth() ];

		double xdist = Face_pts.RightEyeCenterX - Face_pts.LeftEyeCenterX;
		double ydist = Face_pts.RightEyeCenterY - Face_pts.LeftEyeCenterY;

		double xi = xdist/2 + Face_pts.LeftEyeCenterX;
		double yi = ydist/2 +  Face_pts.LeftEyeCenterY;

		double eyedist = 0.8/EyesDistance;
		double dscale =  sqrt ( xdist*xdist + ydist*ydist ) / ( 2.5*EyesDistance );

		double mouthscale1,mouthscale2;
		if ( Face_pts.MouthCenterY>0 )
		{
			mouthscale1= ( Face_pts.MouthCenterY-Face_pts.RightEyeCenterY ) /
			             sqrt ( xdist*xdist + ydist*ydist );
			mouthscale2=mouthscale1;
		}
		else
		{
			mouthscale1=1;
			mouthscale2=0;
		}

		int k=0;

		for ( int i=0; i < heightNorm; ++i )
		{
			for ( int j=0; j < widthNorm; ++j )
			{
				double yf = ( i-heightNorm/3.0 );
				double xf = ( j-widthNorm/2.0 );

				double x = eyedist* ( xf*xdist - yf*ydist ) + xi;
				double y = eyedist* ( yf*xdist + xf*ydist ) *mouthscale1 + yi + mouthscale2 ;
				double xl = x-dscale;
				double xu = x+dscale;
				double yl = y-dscale;
				double yu = y+dscale;

				if ( Pfc_mask.Ismask ( j,i ) )
					tmpNImage[k]=interpData ( xl, yl, xu, yu );
				else
					tmpNImage[k]=0;

				tmpNImage[k]= ( uint8_t ) ceil ( tmpNImage[k] );
				++k;
			}
		}

		for ( int i=0; i < heightNorm*widthNorm; ++i )
			GeomNorm.SetData ( i,tmpNImage[i] );

		delete []tmpNImage;
		return 0;
	}

	int8_t PFCface::interpData ( double xl, double yl, double xu, double yu )
	{
		int i_xl = ( int ) ( xl + 0.5f );
		int i_xu = ( int ) ( xu + 0.5f );
		int i_yl = ( int ) ( yl + 0.5f );
		int i_yu = ( int ) ( yu + 0.5f );

		double yfrac, xfrac;
		double total = 0.f;
		int pos;
		for ( int y = i_yl; y <= i_yu; ++y )
		{
			yfrac = 1.0f;
			if ( y == i_yl )
				yfrac =  - yl + ( double ) i_yl + 0.5f;
			if ( y == i_yu )
				yfrac = yu - ( double ) i_yu + 0.5f;
			if ( i_yl == i_yu )
				yfrac = yu - yl;
			for ( int x = i_xl; x <= i_xu; ++x )
			{
				xfrac = 1.0f;
				if ( x == i_xl )
					xfrac = - xl + ( double ) i_xl + 0.5f;
				if ( x == i_xu )
					xfrac = xu - ( double ) i_xu + 0.5f;
				if ( i_xl == i_xu )
					xfrac = xu - xl;
				double frac = xfrac*yfrac;

				pos=y*OriginalGray.GetImWidth() + x;
				if ( pos < OriginalGray.GetImWidth() *OriginalGray.GetImHeight()  && pos > 0 )
					total += frac * OriginalGray.GetData ( pos );
			}
		}

		double val1 = total / ( ( xu-xl ) * ( yu-yl ) );
		return ( uint8_t ) val1;
	}

	void PFCface::Printmeta()
	{
		cout <<  "LeftEyeCenterX: " << Face_pts.LeftEyeCenterX  <<  " LeftEyeCenterY: " << Face_pts.LeftEyeCenterY
		<< " RightEyeCenterX: " <<   Face_pts.RightEyeCenterX<<  " RightEyeCenterY: " <<  Face_pts.RightEyeCenterY
		<< " NoseCenterX: " <<  Face_pts.NoseCenterX<<  " NoseCenterY: " <<  Face_pts.NoseCenterY
		<<  " MouthCenterX: " << Face_pts.MouthCenterX<<  " MouthCenterY: " << Face_pts.MouthCenterY <<   endl;
	}


}
