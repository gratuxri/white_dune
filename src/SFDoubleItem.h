/*
 * SFDoubleItem.h
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

#ifndef _SFDOUBLE_ITEM_H
#define _SFDOUBLE_ITEM_H

#include "FieldViewItem.h"

class SFDoubleItem : public FieldViewItem {
public:
                        SFDoubleItem(FieldView *view);

    virtual FieldValue *OnMouseMove(FieldValue *value, int index, int delta);
    virtual void        Draw(SDC dc, int x, int y);
    virtual bool        IsTrackable() const { return true; }
    virtual bool        IsEditable() const { return true; }
    virtual void        StartEditing(MyString &str, int offset);
    virtual FieldValue *StopEditing(const char *str, int offset);
};

#endif // _SFDOUBLE_ITEM_H
