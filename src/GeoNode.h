/*
 * GeoNode.h
 *
 * Copyright (C) 1999 Stephen F. White, 2006 J. "MUFTI" Scheurich
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

#ifndef _GEO_NODE_H
#define _GEO_NODE_H

#ifndef _NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_MACROS_H
#include "ProtoMacros.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

#include "SFMFTypes.h"

class GeoProto : public Proto {
public:
               GeoProto(Scene *scene, const char *name);
    FieldIndex geoOrigin;
    FieldIndex geoSystem;
};

class GeoNode : public Node {
public:
                    GeoNode(Scene *scene, Proto *proto);

    virtual const char* getComponentName(void) const { return "Geospatial"; }
    virtual int         getComponentLevel(void) const { return 1; }

    fieldMacros(SFNode,   geoOrigin, GeoProto)
    fieldMacros(MFString, geoSystem, GeoProto)
};

#endif // _GEO_NODE_H

