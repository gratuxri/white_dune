/*
 * ToIndexedFaceSetDialog.h
 *
 * Copyright (C) 1999 Stephen F. White, 2017 J. "MUFTI" Scheurich
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

#ifndef TO_INDEXED_FACE_SET_DIALOG_H
#define TO_INDEXED_FACE_SET_DIALOG_H

#ifndef _DIALOG_H
#include "Dialog.h"
#endif

#include "swttypedef.h"

class ToIndexedFaceSetDialog : public Dialog 
{
public:
                        ToIndexedFaceSetDialog(SWND parent);
    virtual            ~ToIndexedFaceSetDialog();

    void                LoadData();
    virtual void        SaveData();
    bool                getTriangulation() {return m_triangulate;}
private:
    bool                m_triangulate;
};

#endif
