/*
 * Stack.h
 *
 * Copyright (C) 1999 Stephen F. White
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file "COPYING" for details); if 
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave, 
 * Cambridge, MA 02139, USA.
 */

#ifndef _STACK_H
#define _STACK_H

#ifndef _ARRAY_H
#include "Array.h"
#endif

template<class T> 
class MyStack {
public:
                        MyStack()
                           { m_top = 0; }

    void                push(T t)
                           { m_data[m_top++] = t; }
    T                   pop()
                           { 
                           if (m_top == 0)
                               return (T)0;
                           else
                               return m_data[--m_top]; 
                           }
    T                   peek() const
                           { 
                           if (m_top == 0)
                               return (T)0;
                           else
                               return m_data[m_top-1]; 
                           }
    T                   peek(int i) const
                           { 
                           if ((i <= m_top) && (i > 0))
                               return m_data[i-1]; 
                           else 
                               return (T)NULL;
                           }
    int                 empty() const { return m_top == 0; }

    int                 getTop() const { return m_top; }
    
private:
    MyArray<T>          m_data;
    int                 m_top;
};

#endif // _STACK_H
