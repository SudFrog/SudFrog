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


#ifndef VECTOREXTENSION_H
#define  VECTOREXTENSION_H

#include <iostream>
#include <cmath>
#include <cstdlib>

#include <vector>

using namespace std;

namespace TMSP_Face_Space
  {

  template <class T,class V>
  vector<T> operator-(vector<T> & a,vector<V> & b)
  {
    vector<T> out;
    thrownSizeException(&a,&b);

    out=a;
    for (int i=0;i<out.size();i++)
      out[i]-=(V)b[i];

    return out;
  }


  template <class T,class V>
  vector<T> operator*(vector<T> & a,vector<V> & b)
  {
    vector<T> out;
    thrownSizeException(&a,&b);

    out=a;
    for (int i=0;i<out.size();i++)
      out[i]*=(T)b[i];
    return out;
  }

  template <class T,class V>
  vector<T> operator+(vector<T> & a,vector<V> & b)
  {
    vector<T> out;
    thrownSizeException(&a,&b);

    out=a;
    for (int i=0;i<out.size();i++)
      out[i]+=(T)b[i];
    return out;
  }
  template <class T,class V>
  vector<T> operator-(vector<T> & a,V b)
  {
    vector<T> out;
    out=a;
    for (int i=0;i<out.size();i++)
      out[i]-=(T)b;
    return out;
  }

  template <class T,class V>
  vector<T> operator+(vector<T> & a,V & b)
  {
    vector<T> out;
    out=a;
    for (int i=0;i<out.size();i++)
      out[i]*=(T)b;
    return out;
  }

  template <class T,class V>
  vector<T> operator*(vector<T> & a,V b)
  {
    vector<T> out;
    out=a;
    for (int i=0;i<out.size();i++)
      out[i]*=(T)b;
    return out;
  }

  template <class T,class V>
  vector<T> operator/(vector<T> & a,V b)
  {
    vector<T> out;
    if (!b)
      {
        cerr << "Error: divison by zero" << endl;
        return out;
      }

    out=a;
    for (int i=0;i<out.size();i++)
      out[i]/=(T)b;
    return out;
  }

  template <class T>
  T sum_absolute_value(vector<T> *a)
  {
    T val=0;
    for (int i=0;i<(*a).size();i++)
      val+=fabs((*a)[i]);
    return val;
  }

  template <class T>
  T sum_square(vector<T> *a)
  {
    T val=0;
    for (int i=0;i<(*a).size();i++)
      val+=(*a)[i]*(*a)[i];
    return val;
  }

  template <class T>
  T norm_Frobenius(vector<T> *a)
  {
    return sqrt(sum_square(a));
  }

  template <class T>
  T sum(vector<T> *a)
  {
    T val=0;
    for (int i=0;i<(*a).size();i++)
      val+=(*a)[i];
    return val;
  }

  template <class T,class V>
  T dotproduct(vector<T> *a,vector<V> *b)
  {
    vector<T> out;
    thrownSizeException(a,b);

    T val=0;
    for (int i=0;i<(*a).size();i++)
      val+=(*a)[i]*(*b)[i];
    return val;
  }

  template <class T,class V>
  void thrownSizeException(vector<T> *a,vector<V> *b)
  {
    if (!(*a).size() || !(*b).size())
      cout << "Warning @comparator: comparing empty vectors: maybe you forgot to initialize" << endl;

    if ((*a).size()!=(*b).size())
      {
        cerr << "Error @comparator: Size vectors mismatch!" << endl;
        exit(-1);
      }
  }
}
#endif
