/*
 * TimeShiftDialog.h
 *
 * Copyright (C) 2003 J. "MUFTI" Scheurich
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

#ifndef _TIME_SHIFT_DIALOG_H
#define _TIME_SHIFT_DIALOG_H

#ifndef _DIALOG_H
#include "Dialog.h"
#endif
#include "Scene.h"
#include "Array.h"
#include "swt.h"

#include "swttypedef.h"

class TimeShiftDialog : public Dialog {
public:
                TimeShiftDialog(SWND parent);
               ~TimeShiftDialog();
    float       getFraction() { return m_fraction; }
    bool        getWrapAround() { return m_wrapAround; }

protected:
    void        LoadData();
    bool        Validate();
    void        SaveData();

private:
    float       m_fraction;
    bool        m_wrapAround;
};

#endif
