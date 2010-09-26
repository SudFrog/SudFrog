/***************************************************************************
 *   Copyright (C) 2009 by anouar   *
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
#include "inputs.h"

namespace TMSP_Face_Space
{


	inputs::inputs()
	{
		paramnorm=NULL;
	}


	inputs::~inputs()
	{
	}

	inputs::inputs ( const char* pFilename )
	{
		loadfromxml ( pFilename );
	}

	int inputs:: parsenormparam ( string str )
	{
		vector<string> results;
		int cutAt;
		while ( ( cutAt = str.find_first_of ( ":" ) ) != str.npos )
		{
			if ( cutAt > 0 )
			{
				results.push_back ( str.substr ( 0,cutAt ) );
			}
			str = str.substr ( cutAt+1 );
		}
		if ( str.length() > 0 )
		{
			results.push_back ( str );
		}

		int count=results.size();
		if ( count >0 )
		{
			paramnorm=new double[count];
			for ( int i=0;i<count;i++ )
				paramnorm[i]=atof ( results[i].c_str() );
		}
	}


	int inputs::loadfromxml ( const char* pFilename )
	{
		TiXmlDocument * XDp_doc;
		TiXmlElement * XEp_main;
		TiXmlNode * XEp_Node;

		XDp_doc = new TiXmlDocument;
		if ( ! XDp_doc -> LoadFile ( pFilename ) )
		{
			cerr << "Can't find " << pFilename << "!\n";
			return 99;
		}

		XEp_main=XDp_doc->RootElement();
		verb << "Parameters loaded from xml\n";
//get paramdir
		TinyXPath::xpath_processor xp_proc ( XEp_main, "/PFC_params" );
		if ( xp_proc.u_compute_xpath_node_set() >0 )
		{
			XEp_Node=xp_proc.XNp_get_xpath_node ( 0 );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			params_dir=Xelement->Attribute ( "dir" );
			nthreads=atoi ( Xelement->Attribute ( "threads" ) );
			verboz=atoi ( Xelement->Attribute ( "verb" ) );
			verb.setmaxlevel ( verboz );
			SetVerbose();
			if ( params_dir=="." )
			{
				size_t found;
				found= ( string ( pFilename ) ).find_last_of ( "/\\" );
				if ( found==string::npos )
					params_dir="./";
				else
					params_dir= ( string ( pFilename ) ).substr ( 0,found+1 );
			}

			verb << "\t Parameters directory: " << params_dir << "\n";
		}

		TinyXPath::xpath_processor xp_norm ( XEp_main, "/PFC_params/norm" );
		if ( xp_norm.u_compute_xpath_node_set() >0 )
		{
			XEp_Node=xp_norm.XNp_get_xpath_node ( 0 );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			norm_eye_dist=atoi ( Xelement->Attribute ( "eyedist" ) );
			normlight=Xelement->Attribute ( "illum" );
			string normlightparams=Xelement->Attribute ( "params" );

			verb << "\t normalisation parameters:\n";
			verb << "\t\t eye distance : " << norm_eye_dist << "\n";
			verb << "\t\t light normalization method: " << normlight << "\n";
			verb << "\t\t light normalization params: " << normlightparams << "\n";

			parsenormparam ( normlightparams );
		}


		TinyXPath::xpath_processor xp_gabor ( XEp_main, "/PFC_params/gabor" );
		if ( xp_gabor.u_compute_xpath_node_set() >0 )
		{
			XEp_Node=xp_gabor.XNp_get_xpath_node ( 0 );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			gabor_method=Xelement->Attribute ( "method" );

			Gabscales= atoi ( Xelement->Attribute ( "scales" ) );
			Gaborientations= atoi ( Xelement->Attribute ( "orientations" ) );
			GabminWavelet= atoi ( Xelement->Attribute ( "minWaveLength" ) );
			Gabmult=atof ( Xelement->Attribute ( "mult" ) );
			GabsigmaOnf= atof ( Xelement->Attribute ( "sigmaOnf" ) );
			GabdThetaOnSigma= atof ( Xelement->Attribute ( "ThetaOnSigma" ) );
			Gabreduction=atoi ( Xelement->Attribute ( "reductcoef" ) );

			// verb << gabor_file << "\n";
			verb << "\t Gabor parameters:\n";
			verb << "\t\t method: " << gabor_method << "\n";
			if ( gabor_method != "NONE" )
			{
				verb << "\t\t scales: " << Gabscales<< "\n";;
				verb << "\t\t orientations: " <<Gaborientations<< "\n";
				verb << "\t\t minwave length: " <<GabminWavelet<< "\n";
				verb << "\t\t mult: " <<Gabmult<< "\n";
				verb << "\t\t Sigma on freq: " <<GabsigmaOnf<< "\n";
				verb << "\t\t delta sigma: " <<GabdThetaOnSigma<< "\n";
				verb << "\t\t reduction coefficient: " <<Gabreduction<< "\n";
			}
		}

		TinyXPath::xpath_processor xp_red ( XEp_main, "/PFC_params/reduction_space" );
		if ( xp_red.u_compute_xpath_node_set() >0 )
		{
			XEp_Node=xp_red.XNp_get_xpath_node ( 0 );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			reduction_method=Xelement->Attribute ( "Method" );
			reduction_param=Xelement->Attribute ( "Method-param" );
			reduction_space_file=Xelement->Attribute ( "file" );
			controlled=atoi ( Xelement->Attribute ( "control" ) );
			noncontrolled=atoi ( Xelement->Attribute ( "noncontrol" ) );
			nusers=atoi ( Xelement->Attribute ( "nusers" ) );
			if ( reduction_space_file.find_first_of ( "." ) ==0 )
				reduction_space_file=params_dir+reduction_space_file;

			verb << "\t Reduction space parameters:\n";
			verb << "\t\t Reduction method: " << reduction_method << "\n";

			if ( reduction_param!="" )
				verb << "\t\t Reduction parameters: " << reduction_param << "\n";

			verb << "\t\t file: " << reduction_space_file << "\n";
			verb << "\t\t n° users: " << nusers<< "\n";
			verb << "\t\t n° controlled/user: " << controlled<< "\n";
			verb << "\t\t n° non controlled/user: " << noncontrolled << "\n";
		}

		TinyXPath::xpath_processor xp_sim ( XEp_main, "/PFC_params/Similarity" );
		if ( xp_sim.u_compute_xpath_node_set() >0 )
		{
			XEp_Node=xp_sim.XNp_get_xpath_node ( 0 );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			sim_measure=Xelement->Attribute ( "measure" );

			verb << "\t Similarity parameters:\n";
			verb << "\t\t Similarity/distance Measure : " << sim_measure << "\n";
		}
		return 0;
	}

	vector <string> inputs::loadwatchlist ( const char* pFilename )
	{
		TiXmlDocument * XDp_doc;
		TiXmlElement * XEp_main;
		TiXmlNode * XEp_Node;
		vector <string> users;
//    TiXmlNode * XEp_Image;
		TiXmlElement *element;
		char Request[255];

		XDp_doc = new TiXmlDocument;
		if ( ! XDp_doc -> LoadFile ( pFilename ) )
		{
			printf ( "Can't find %s !\n",pFilename );
			exit ( -1 ) ;
		}
		XEp_main=XDp_doc->RootElement();
		TinyXPath::xpath_processor xp_user ( XEp_main, "//user" );
		int num= xp_user.u_compute_xpath_node_set();

		for ( int i=0;i<num;i++ )
		{
			XEp_Node=xp_user.XNp_get_xpath_node ( i );
			TiXmlElement *Xelement=XEp_Node->ToElement();
			users.push_back ( Xelement->Attribute ( "id" ) );
		}
		return users;
	}

	void inputs::SetVerbose ( verbose &V )
	{
		verb=V;
	}

	void inputs::SetVerbose ()
	{
		verb.Verbose();
	}
	void inputs::SetMute ()
	{
		verb.UnVerbose();
	}


}
