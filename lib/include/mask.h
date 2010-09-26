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
#ifndef PFC_SPACEMASK_H
#define PFC_SPACEMASK_H
/**
	@author Télécom SudParis. Major Developer: Anouar Mellakh <me.anouar@gmail.com>
*/

namespace TMSP_Face_Space
{
	/**
	* \brief Class that creates an elliptic mask to be applied to faces
	* \image html norm1.jpg
	*
	* \image latex norm1.eps "Face normalized to 50 pixels between eyes and with elliptic mask " width=3cm
	*/
	class Mask
	{
		public:
			/**
			 * Constructor
			 */
			Mask();
			/**
			 * Constructor
			 * initialize the mask with the eyes distance
			 * @param EYE_DISTANCE distance between eyes' centers
			 */
			Mask ( int EYE_DISTANCE );
			/**
			 * Destructor
			 */
			~Mask();
			/**
			 * initialize the mask with the eyes distance
			 * @param EYE_DISTANCE
			 */
			void Init ( int EYE_DISTANCE );
			/**
			 * test if the point belgons to the mask or not
			 * @param x point x coordinate
			 * @param y point y coordinate
			 * @return True if the point belongs else False
			 */
			bool Ismask ( const int& x,const int& y );
			/**
			 * apply mask (set to zero the non mask point)
			 * @param imageout a pointer to a double data
			 * @param imageRows height of the data matrix
			 * @param imageCols width of  the data matrix
			 */
			void ApplyMask ( double *&imageout,int imageRows,int imageCols );
			/**
			 *
			 * @return the mask's width
			 */
			int getwidth();
			/**
			 *
			 * @return the mask's height
			 */
			int getheight();

		private:
			int NORMALIZED_ROW,NORMALIZED_COL,ellipseX0,ellipseY0,ellipseSemiMinor2,ellipseSemiMajor2;


	};

}

#endif
