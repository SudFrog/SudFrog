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
#include "linearreducer.h"

namespace TMSP_Face_Space
{
	/**
	 *
	 */

	LinearReducer::LinearReducer ( LProblem w )
	{
		Init();
		SetProblem ( w );
	}

	LinearReducer::LinearReducer ( Matrix & A,LProblem w )
	{

		Init();
		Data << A;
		SetProblem ( w );
	}

	LinearReducer::LinearReducer()
	{
		Init();
	}


	/**
	 *
	 * @param A
	 */
	LinearReducer::LinearReducer ( Matrix & A )
	{

		Init();
		Data << A;
	}

	/**
	 *
	 * @param A
	 * @param lab
	 */
	LinearReducer::LinearReducer ( Matrix & A , vector < int > lab )
	{

		Init();
		Data << A;
		labels=lab;
	}

	/**
	 *
	 */
	void LinearReducer::Init()
	{
		EigenVectors.ReSize ( 0, 0 );
		MeanFace.ReSize ( 0 );
		EigenValues.ReSize ( 0 );
		MaxVariance=100.0;
		SetProblem ( PCA );
	}

	/**
	 * Destructor
	 */
	LinearReducer::~LinearReducer()
	{
		//free();
	}

	/**
	 *
	 */
	void LinearReducer::free()
	{
		EigenVectors.ReleaseAndDelete();
		MeanFace.ReleaseAndDelete();
		EigenValues.ReleaseAndDelete();
		Data.ReleaseAndDelete();
		CentredData.ReleaseAndDelete();

	}

	void LinearReducer::SetProblem ( LProblem w )
	{
		which=w;

		switch ( which )
		{
			case PCA:
				methodname="PCA";
				break;

			case LDA:
				methodname="LDA";
				break;

			case DLDA:
				methodname="DLDA";
				break;

		}
	}

	void LinearReducer::SetProblem ( string w )
	{


		if ( w=="PCA" )
		{
			which=PCA;
			methodname="PCA";
		}

		if ( w=="LDA" )
		{
			which=LDA;
			methodname="LDA";
		}

		if ( w=="DLDA" )
		{
			which=DLDA;
			methodname="DLDA";
		}

	}

	LProblem LinearReducer::GetProblem ( )
	{

		return which;
	}

	string LinearReducer::GetProblemName ( )
	{
		return methodname;

	}

	void LinearReducer::SetData ( Matrix & A )
	{
		Data << A;
	}

	void LinearReducer::SetLabels ( vector <int> labs )
	{

		labels=labs;
	}


	bool LinearReducer::GetPCACompound()
	{
		SymmetricMatrix CM;
//     ColumnVector Tmpface;
		DiagonalMatrix D;


		verb << "Centring Data..\n";

		CenterData();

		Data.ReleaseAndDelete();  // free space if you want to Get only PCA components

//correlation Matrix
		if ( CentredData.Nrows() < CentredData.Ncols() )
		{
			verb << "Creating Correlation matrix..\n";

			CM << CentredData * CentredData.t();

			verb << "Computing Eigen parameters..\n";

			ComputeEigens ( CM, D );

			EigenValues.ReSize ( D.Nrows() );

			for ( int i = 1; i <= D.Nrows(); i++ )
			{
				EigenValues ( i ) = D ( i, i ) / CentredData.Ncols();
			}

		}
		else
		{

			verb << "Creating Correlation matrix for snapshot method ..\n";

			CM << CentredData.t() * CentredData;


			verb << "Computing Eigen parameters..\n";

			ComputeEigens ( CM, D );


			verb << "Eigen parameters Computed\n";

			EigenValues.ReSize ( D.Nrows() );

			for ( int i = 1; i <= D.Nrows(); i++ )
			{
				EigenValues ( i ) = D ( i, i ) / CentredData.Ncols();
				//    verb << EigenValues(i) << "\n";
				D ( i, i ) = 1. / sqrt ( D ( i, i ) );
			}

			CM.ReleaseAndDelete();


			verb << "Computing the Real Eigen parameters..  \n";

			EigenVectors=EigenVectors * D;

			D.ReleaseAndDelete();

			EigenVectors << CentredData * EigenVectors;
		}

		CentredData.ReleaseAndDelete();


		verb << "Normalizing Eigen Vectors ..\n";

		GetNonZeroEigVal();

		NormalizeMat ( EigenVectors );

		//by default
		verb << "PCA Space Computed ..\n";

		return true;
	}

	bool LinearReducer::GetLDACompound()
	{

		int classnumber=labels.size();
		LinearReducer PCASPACE;
		PCASPACE.SetProblem ( PCA );
		PCASPACE.SetData ( Data );
		PCASPACE.Setmaxvariance ( 98.0 );
		PCASPACE.GetPCACompound();
		PCASPACE.ReduceToVariance();

		//reduction a la Buhumur Number of feature - Nclass
		PCASPACE.EigenVectors=PCASPACE.EigenVectors.Columns ( 1,PCASPACE.EigenVectors.Ncols()-classnumber );
		Data= ( Data.t() *PCASPACE.EigenVectors ).t();
		verb << "Reduction by PCA done!\n";

		int DataDim=Data.Nrows();
		Matrix SW ( DataDim,DataDim );
		Matrix SB ( DataDim,DataDim );

		SW=0.0;
		SB=0.0;

		//get meanclass and mean Data
		cout << "Computing meanclass and mean Data" << endl;
		vector < ColumnVector > MeanClasses ( classnumber );
		ColumnVector MeanData ( DataDim );
		MeanData=0.0;                                           //init MeanData;
		int start,end=0;
		for ( int i=0;i< classnumber;i++ )
		{
			start=end+1;                                        //start=end+1
			end=start + labels[i]-1;
			MeanClasses[i]=GetMeanData ( Data,start,end );         //Get MeanData class
			MeanData+=MeanClasses[i]/classnumber;
		}

		cout << "meanclass and mean Data Computed" << endl;
		cout << "Computing within et between scatter matrix..." << endl;
		start=1;end=0;
		for ( int i=0;i<classnumber;i++ )
		{
			start=end+1;
			end=start + labels[i]-1;
			//compute within-class scatter matrix
			for ( int j=start;j<=end;j++ )
				SW+= ( Data.Column ( j )-MeanClasses[i] ) * ( Data.Column ( j )-MeanClasses[i] ).t();
			//compute between-class scatter matrix
			SB+=labels[i]* ( ( MeanClasses[i]-MeanData ) * ( MeanClasses[i]-MeanData ).t() );
		}

		cout << "Within et between scatter matrix Computed" << endl;
		cout << "Computing Eigen Problem SW.i()*SB =lamda*V ..." << endl;
		DiagonalMatrix D;
		SVD ( ( SW.i() *SB ), D, EigenVectors );
		SortSV ( D, EigenVectors, false );
		cout << "Eigen problem SW.i()*SB =lamda*V solved" << endl;

		//discard weak Eigenvalues
		int Nz;
		for ( Nz=1;Nz<=D.Nrows();Nz++ )
			if ( D ( Nz,Nz ) < 0.00001 )
				break;


		Nz-=1;                                                  // non zero Vectors
		EigenVectors=EigenVectors.Columns ( 1,Nz );
		EigenValues.ReSize ( Nz );

		for ( int i=1;i<=Nz;i++ )
			EigenValues ( i ) =D ( i,i );

		EigenVectors=PCASPACE.EigenVectors*EigenVectors;        //compute the Complete EigenVectors

		for ( int i = 1; i <= EigenVectors.Ncols(); i++ )       // normalize Vectors
			EigenVectors.Column ( i ) = ( EigenVectors.Column ( i ) / EigenVectors.Column ( i ).NormFrobenius() );

		MeanFace=PCASPACE.MeanFace;
		SW.ReleaseAndDelete();
		SB.ReleaseAndDelete();
		D.ReleaseAndDelete();

		return true;
	}


	bool LinearReducer::GetDLDACompound()
	{
		int DataDim=Data.Nrows();
		int classnumber=labels.size();
		Matrix SW ( DataDim,Data.Ncols() );
		Matrix SB ( DataDim,classnumber );

		SW=0.0;
		SB=0.0;
//get meanclass and mean Data
		verb << "Computing meanclass and mean Data\n";
		vector < ColumnVector > MeanClasses ( classnumber );
		MeanFace.ReSize ( DataDim );
		MeanFace=0.0;                                           //init MeanData;
		int start,end=0;

		for ( int i=0;i< classnumber;i++ )
		{
			start=end+1;                                        //start=end+1
			end=start + labels[i]-1;
			MeanClasses[i]=GetMeanData ( Data,start,end );         //Get MeanData class
			MeanFace+=MeanClasses[i]/classnumber;
		}


		verb << "meanclass and mean Data Computed\n";
		verb << "Computing Sb matrix...\n";


		for ( int i=0;i<classnumber;i++ )
		{
			SB.Column ( i+1 ) =sqrt ( labels[i] ) * ( MeanClasses[i]-MeanFace );
		}


		verb << "Computing Sb Eigen \n";

		DiagonalMatrix Db,Dw;

		Matrix EigB,EigW;

		GetCovEigenCompound ( SB,Db,EigB );

		int Nz;

		for ( Nz=1;Nz<=Db.Nrows();Nz++ )
			if ( Db ( Nz,Nz ) < 0.00001 )
				break;

		Nz-=1;

		// non zero Vectors
		verb << "Non zero: "<< Nz << "\n";

//  verb << Nz << "\n";
		EigB<<EigB.Columns ( 1,Nz );

		Db<<Db.submatrix ( 1,Nz,1,Nz );


		verb << "Sb Eigen Computed\n";

		start=1;

		end=0;

		int count=1;

		for ( int i=0;i<classnumber;i++ )
		{
			start=end+1;
			end=start + labels[i]-1;
//compute within-class scatter matrix

			for ( int j=start;j<=end;j++ )
			{
				SW.Column ( count ) = ( Data.Column ( j )-MeanClasses[i] );
				count++;
			}
		}

//     verb << "Computing Z matrix\n";

		for ( Nz=1;Nz<=Db.Nrows();Nz++ )
			Db ( Nz,Nz ) =1/sqrt ( Db ( Nz,Nz ) );

		Matrix Z;

		Z=EigB*Db;

//     verb << "Z matrix Computed\n";

		verb << "Computing Eigen for Z.t()*Sw*Z by snap shot\n";

		SW << ( SW.t() *Z ).t();

		GetCovEigenCompound ( SW,Dw,EigW );


		verb << "Eigen for Z.t()*Sw*Z computed\n";

		EigW << EigW.Columns ( 1,EigW.Ncols() );

		Dw << Dw.submatrix ( 1,Dw.Ncols(),1,Dw.Ncols() );

		for ( Nz=1;Nz<=Dw.Nrows();Nz++ )
			Dw ( Nz,Nz ) =1.0/sqrt ( Dw ( Nz,Nz ) );

		EigenVectors= ( Dw*EigW.t() *Z.t() ).t();


		EigenValues.ReSize ( Dw.Ncols() );

		for ( int i=1;i<=Dw.Ncols();i++ )
			EigenValues ( i ) =Dw ( i,i );

		SW.ReleaseAndDelete();

		SB.ReleaseAndDelete();

		Dw.ReleaseAndDelete();

		EigB.ReleaseAndDelete();

		Db.ReleaseAndDelete();

		EigW.ReleaseAndDelete();


		verb << "Reduction LDA Matrix Dw^(-0.5)*EigW.t()*Z.t() Computed\n";

		return true;
	}



	void LinearReducer::GetNonZeroEigVal()
	{

		int i;

		for ( i = 1; i <= EigenValues.Nrows(); i++ )
		{
			if ( EigenValues ( i ) < 0.00001 )
				break;
		}

		EigenValues = EigenValues.Rows ( 1, i - 1 );

		SpaceUsedSize=EigenValues.Nrows();
		EigenVectors = EigenVectors.Columns ( 1, i - 1 );
	}



	int LinearReducer::ComputeEigens ( GeneralMatrix & CM, DiagonalMatrix & D )
	{
		SVD ( CM,D,EigenVectors );
		SortSV ( D, EigenVectors, false );
		return 0;
	}


	int LinearReducer::ComputeSpace ()
	{

		switch ( which )
		{
			case PCA:
				GetPCACompound();
				break;

			case LDA:
				GetLDACompound();
				break;

			case DLDA:
				GetDLDACompound();
				break;
		}

		return 0;
	}

	void LinearReducer::SaveSpace ( char * filename,GaborParams &params )
	{
		ofstream outfileMat;
		outfileMat.open ( filename );

		outfileMat << "<Method=\""<< methodname << "\" Gabor=\""<< params.methodname<<"\" Gwidth=\""<<params.width<<"\" Gheight=\""<<params.height<<"\" Gres=\""<<params.nscale<<"\" Gorient=\""<<params.norient <<"\" Gminwave=\""<<params.minWaveLength <<"\" Gmult=\""<<params.mult <<"\" Gsigf=\""<< params.sigmaOnf<<"\" GThetasig=\""<< params.dThetaOnSigma<<"\" Greduction=\""<< params.reductcoef<<"\" >"<< "\n";

		outfileMat << "<Space_Part=\"Mean Face\" Size=\"" << MeanFace.Nrows() << "\">\n";
		outfileMat << MeanFace;
		outfileMat << "\n";


		int   importantEigens=EigenValues.Nrows();
		EigenValues=GetEigensVariance();

		outfileMat << "<Space_Part=\"EigenValues\" Size=\"" << importantEigens << "\">\n";
		outfileMat << EigenValues.Rows ( 1,importantEigens );
		outfileMat << "\n";

		outfileMat << "<Space_Part=\"EigenVectors\" Size=\"" << EigenVectors.Nrows() << ","
		<< importantEigens<< "\">\n";
		outfileMat << EigenVectors.Columns ( 1,importantEigens );
		outfileMat.close();
//verb << "test " <<filename << "\n";
	}




	void LinearReducer::SaveSpace ( char * filename )
	{
		ofstream outfileMat;
		outfileMat.open ( filename );


		outfileMat << "<Space_Part=\"Mean Face\" Size=\"" << MeanFace.Nrows() << "\">\n";
		outfileMat << MeanFace;
		outfileMat << "\n";

		int   importantEigens=EigenValues.Nrows();
		EigenValues=GetEigensVariance();

		outfileMat << "<Space_Part=\"EigenValues\" Size=\"" << importantEigens << "\">\n";
		outfileMat << EigenValues.Rows ( 1,importantEigens );
		outfileMat << "\n";

		outfileMat << "<Space_Part=\"EigenVectors\" Size=\"" << EigenVectors.Nrows() << ","
		<< importantEigens<< "\">\n";
		outfileMat << EigenVectors.Columns ( 1,importantEigens );
		outfileMat.close();
	}



	void LinearReducer::LoadSpace ( const char * filename )
	{

		string line;
		int size1, size2;
		string part;
		ifstream outfileMat;
		outfileMat.open ( filename );

		if ( outfileMat.good() )
		{
			while ( outfileMat.good() )
			{
				getline ( outfileMat, line );
				part = GetSpacePart ( line, size1, size2 );

				if ( part != "none" && part != "unknown" )
				{
					if ( part == "Mean Face" )
						MeanFace = LoadVectorFromfile ( outfileMat, size1 );
					else if ( part == "EigenValues" )
						EigenValues = LoadVectorFromfile ( outfileMat, size1 );
					else if ( part == "EigenVectors" )
						EigenVectors = LoadMatrixFromfile ( outfileMat, size1, size2 );

				}
			}

			SpaceSize=EigenVectors.Ncols();

			SpaceUsedSize=SpaceSize;                            //by default
			outfileMat.close();

		}
		else
		{
			verb << filename << " not found\n";
			exit ( -1 );
		}

		//verb << "Mean face size: " << MeanFace.Nrows() << "\n";
		//verb << "Space size: " <<  EigenVectors.Ncols() << "x"<<  EigenVectors.Nrows() <<  "\n";

	}


	ReturnMatrix LinearReducer::Projection ( ColumnVector & face )
	{

		ColumnVector null ( 0 );
		ColumnVector centredface ( face.Nrows() );
		ColumnVector Projectedface ( face.Nrows() );

		if ( face.Nrows() == MeanFace.Nrows() )
		{
			centredface = face - MeanFace;
//         verb << centredface.Ncols() << "x" << EigenVectors.Ncols() << "\n";
			Projectedface = ( centredface.t() *EigenVectors ).t();
// 	verb <<Projectedface.Nrows() << "\n";
			Projectedface.Release();
			return Projectedface;

		}
		else
		{
			cerr << "Error: dimension mismatch between image and meanface\n";
			return null;
		}

	}


	double LinearReducer::Distance ( ColumnVector & face1,ColumnVector & face2,string Method )
	{

		double dist=0;
		ColumnVector vect1,vect2;

		if ( Method=="normdist" )
		{
			vect1<<face1/face1.norm_Frobenius();
			vect2<<face2/face2.norm_Frobenius();
			dist= ( vect1-vect2 ).norm_Frobenius();
			vect1.ReleaseAndDelete();
			vect2.ReleaseAndDelete();
		}

		if ( Method=="cos" )
		{
			dist=-dotproduct ( face1,face2 ) / ( sqrt ( face1.sum_square() *face2.sum_square() ) );

		}
		else

			if ( Method=="L1" )
			{
				dist= ( face1-face2 ).sum_absolute_value();

			}
			else

				if ( Method=="L2" )
				{
					dist= ( face1-face2 ).norm_Frobenius();

				}
				else

					if ( Method=="MahalL1" )
					{
//the projection must be divided by the eigenvalue before this step
						dist= ( face1-face2 ).sum_absolute_value();

					}
					else

						if ( Method=="MahalL2" )
						{
//the projection must be divided by the eigenvalue before this step

							dist= ( face1-face2 ).norm_Frobenius();

						}
						else

							if ( Method=="Mahalcos" )
							{
//the projection must be divided by the eigenvalue before this step
								dist=-dotproduct ( face1,face2 ) / ( sqrt ( face1.sum_square() *face2.sum_square() ) );
							}

		return dist;
	}


	double LinearReducer::Distance ( ColumnVector & face1,ColumnVector & face2,string Method,ColumnVector &wheigts )
	{

		double dist=0;
		ColumnVector vect,vect1,vect2;

		if ( Method=="normdist" )
		{
			vect1<<face1/face1.norm_Frobenius();
			vect2<<face2/face2.norm_Frobenius();
			dist=SP ( ( vect1-vect2 ),wheigts ).norm_Frobenius();
			vect1.ReleaseAndDelete();
			vect2.ReleaseAndDelete();

		}
		else

			if ( Method=="cos" )
			{
				vect<<SP ( face1,wheigts );
//verb << vect << enl;
				dist=-dotproduct ( vect,face2 ) / ( sqrt ( face1.sum_square() *face2.sum_square() ) );
//verb << dist << "\n";
				vect.ReleaseAndDelete();

			}
			else

				if ( Method=="L1" )
				{
					dist=SP ( ( face1-face2 ),wheigts ).sum_absolute_value();

				}
				else

					if ( Method=="L2" )
					{
//dist=(face1-face2).norm_Frobenius();
						vect= ( face1-face2 );
						vect=SP ( vect,vect );
						vect=SP ( wheigts,vect );
						dist=vect.sum();
						vect.ReleaseAndDelete();

					}
					else

						if ( Method=="MahalL1" )
						{
//the projection must be divided by the eigenvalue before this step
							dist= ( face1-face2 ).sum_absolute_value();

						}
						else

							if ( Method=="MahalL2" )
							{
//the projection must be divided by the eigenvalue before this step

								dist= ( face1-face2 ).norm_Frobenius();

							}
							else

								if ( Method=="Mahalcos" )
								{
//the projection must be divided by the eigenvalue before this step
									vect=SP ( face1,wheigts );
									dist=-dotproduct ( vect,face2 ) / ( sqrt ( face1.sum_square() *face2.sum_square() ) );
								}

		return dist;
	}



	void LinearReducer::SetSpaceUsedSize ( long size )
	{
		SpaceUsedSize=size;
	}



	long LinearReducer::GetSpaceSize()
	{
		return SpaceUsedSize;
	}



	string  LinearReducer::GetSpacePart ( string line, int & size1, int & size2 )
	{
		size1 = 0;
		size2 = 0;
		int pos1, pos2;
		int pos = line.find ( "Space_Part" );

		if ( pos != ( int ) string::npos )
		{
			pos1 = line.find ( "\"" );
			pos2 = line.find ( "\"", pos1 + 1 );
			string occ = line.substr ( pos1 + 1, pos2 - pos1 - 1 );

			if ( occ == "Mean Face" )
			{
				pos = line.find ( "Size" );

				if ( pos != ( int ) string::npos )
				{
					pos1 = line.find ( "\"", pos + 1 );
					pos2 = line.find ( "\"", pos1 + 1 );
					size1 = ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
				}

				return occ;

			}
			else if ( occ == "EigenValues" )
			{
				pos = line.find ( "Size" );

				if ( pos != ( int ) string::npos )
				{
					pos1 = line.find ( "\"", pos + 1 );
					pos2 = line.find ( "\"", pos1 + 1 );
					size1 = ( int ) atof ( line.substr ( pos1 + 1, pos2 - pos1 - 1 ).c_str() );
				}

				return occ;

			}
			else if ( occ == "EigenVectors" )
			{
				pos = line.find ( "Size" );

				if ( pos != ( int ) string::npos )
				{
					pos1 = line.find ( "\"", pos + 1 );
					pos2 = line.find ( "\"", pos1 + 1 );
					string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
					pos = tmp.find ( "," );
					size1 = ( int ) atof ( tmp.substr ( 0, pos ).c_str() );
					size2 = ( int ) atof ( tmp.substr ( pos + 1 ).c_str() );
				}

				return occ;

			}
			else if ( occ == "MeansProj" )
			{
				pos = line.find ( "Size" );

				if ( pos != ( int ) string::npos )
				{
					pos1 = line.find ( "\"", pos + 1 );
					pos2 = line.find ( "\"", pos1 + 1 );
					string tmp = line.substr ( pos1 + 1, pos2 - pos1 - 1 );
					pos = tmp.find ( "," );
					size1 = ( int ) atof ( tmp.substr ( 0, pos ).c_str() );
					size2 = ( int ) atof ( tmp.substr ( pos + 1 ).c_str() );
				}

				return occ;

			}
			else
				return "unknown";

		}
		else
			return "none";

	}


	bool LinearReducer::CenterData()
	{
		//Matrix tmp = Data;                                      // or any other matrix type
		MeanFace.ReSize ( Data.Nrows() );
		CentredData.ReSize ( Data.Nrows(), Data.Ncols() );
		MeanFace = 0.0;

		for ( int j = 1; j <= Data.Ncols(); j++ )
			MeanFace += Data.Column ( j ) / Data.Ncols();

		for ( int j = 1; j <= Data.Ncols(); j++ )
			CentredData.Column ( j ) = Data.Column ( j )-MeanFace;


		return true;
	}



	ReturnMatrix LinearReducer::NormalizeColumns ( Matrix & A )
	{
		Matrix Normalised ( A.Nrows(), A.Ncols() );

		for ( int i = 1; i <= A.Ncols(); i++ )
		{
			Normalised.Column ( i ) = ( A.Column ( i ) / A.Column ( i ).NormFrobenius() );
		}

		Normalised.Release();

		return Normalised;
	}


	void LinearReducer::NormalizeMat ( Matrix & A )
	{

		for ( int i = 1; i <= A.Ncols(); i++ )
		{
			A.Column ( i ) = ( A.Column ( i ) / A.Column ( i ).NormFrobenius() );
		}

	}



	int LinearReducer::GetVarianceCount()
	{

		float TotalSum = EigenValues.sum();
		float Per = MaxVariance / 100;
		int i=100;

		for ( i = 0; i <= EigenValues.Nrows(); i++ )
		{
			if ( ( EigenValues.Rows ( 1, i ).sum() / TotalSum ) >= Per )
				break;
		}

		return i;
	}

	void LinearReducer::Setmaxvariance ( string line )
	{

		int pos1,pos = line.find ( "variance" );
		if ( pos != ( int ) string::npos )
		{
			pos1 = line.find ( "=", pos + 1 );
			MaxVariance = atoi ( line.substr ( pos1 + 1 ).c_str() );
		}
		else
		{
			MaxVariance=100;
		}

	}


	void LinearReducer::Setmaxvariance ( float var )
	{
		if ( var<0 || var >100 )
			MaxVariance=100;
		else
			MaxVariance=var;
	}


	float LinearReducer::Getmaxvariance()
	{
		return MaxVariance;
	}


	void LinearReducer::ReduceToVariance()
	{
		int importantEig=GetVarianceCount();


		verb << "reducing EigenParameters to " << importantEig << "\n";

		EigenValues=EigenValues.Rows ( 1,importantEig );

		EigenVectors=EigenVectors.Columns ( 1,importantEig );

	}


	ReturnMatrix LinearReducer::GetEigensVariance()
	{

		ColumnVector Variances ( EigenValues.Nrows() );

		for ( int i=1;i<=EigenValues.Nrows();i++ )
			Variances ( i ) = ( 1/sqrt ( EigenValues ( i ) ) );

		Variances.Release();

		return Variances;

	}


	ReturnMatrix LinearReducer::Reconstruction ( ColumnVector &Comp )
	{

		ColumnVector Reconstructed ( EigenVectors.Nrows() );
		Reconstructed=0.0;

		for ( int i=1;i<= Comp.Nrows();i++ )
			Reconstructed+=Comp ( i ) *EigenVectors.Column ( i );

		Reconstructed+=MeanFace;

		Reconstructed.Release();

		return Reconstructed;

	}


	ReturnMatrix LinearReducer::GetMeanData ( Matrix &A )
	{
		return GetMeanData ( A,1,A.Ncols() );
	}


	ReturnMatrix LinearReducer::GetMeanData ( Matrix &A, int start,int end )
	{
		ColumnVector meanface ( A.Nrows() );
		meanface= 0.0;                                          //init meanface
		float size=end-start+1;

		for ( int i=start;i<=end;i++ )
			meanface+=A.Column ( i ) /size;

		meanface.Release();

		return meanface;

	}

	bool LinearReducer::GetCovEigenCompound ( Matrix &Mat,DiagonalMatrix &EigenV,Matrix &EigenVectors )
	{
		Matrix CM;
		DiagonalMatrix D;

		if ( Mat.Nrows() < Mat.Ncols() )
		{
			CM << Mat * Mat.t();
			SVD ( CM,D,EigenVectors );
			SortSV ( D, EigenVectors, false );
			EigenV << D/Mat.Ncols();
			CM.ReleaseAndDelete();
			D.ReleaseAndDelete();

		}
		else
		{
// 	verb << "Computing CM\n";
			CM << Mat.t() * Mat;
//CentredData.Release();
//         verb << "Computing SVD\n";
			SVD ( CM,D,EigenVectors );
			SortSV ( D, EigenVectors, false );
//         verb << "Computing EigenV\n";
			EigenV << D/Mat.Ncols();
//         verb << EigenV.Nrows() << "x" << EigenV.Ncols() << "\n";

			for ( int i = 1; i <= D.Nrows(); i++ )
				D ( i, i ) = 1. / sqrt ( D ( i, i ) );

			CM.ReleaseAndDelete();

//         verb << "Computing EigenVectors 1\n";
			EigenVectors<<EigenVectors * D;

			D.ReleaseAndDelete();

//         verb << "Computing EigenVectors 2\n";
			EigenVectors << Mat * EigenVectors;
		}

		return true;
	}
	void LinearReducer::SetVerbose ( verbose &v )
	{
		verb=v;
	}


}


