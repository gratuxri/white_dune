/*
 * URLDialog.h
 *
 * Copyright (C) 2004 J. "MUFTI" Scheurich
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

#ifndef _URL_DIALOG_H
#define _URL_DIALOG_H

#ifndef _DIALOG_H
#include "Dialog.h"
#endif
#include "Scene.h"
#include "Array.h"
#include "swt.h"

#include "swttypedef.h"

class URLDialog : public Dialog {
public:
                URLDialog(SWND parent);                
               ~URLDialog();
    char       *getPath() { return m_path; }

protected:
    void        LoadData();
    bool        Validate();
    void        SaveData();

private:
    bool        m_valid;
    char        m_path[1024];
};

#endif
