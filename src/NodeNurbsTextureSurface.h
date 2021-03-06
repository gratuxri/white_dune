/*
 * NodeNurbsTextureSurface.h
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

#ifndef _NODE_NURBS_TEXTURE_SURFACE_H
#define _NODE_NURBS_TEXTURE_SURFACE_H

#ifndef NODE_H
#include "Node.h"
#endif
#ifndef _PROTO_MACROS_H
#include "ProtoMacros.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif
#ifndef _VEC3F
# include "Vec3f.h"
#endif
#ifndef _SFMFTYPES_H
# include "SFMFTypes.h"
#endif

#include "NodeNurbsTextureCoordinate.h"

class ProtoNurbsTextureSurface : public ProtoNurbsTextureCoordinate {
public:
                    ProtoNurbsTextureSurface(Scene *scene);
    virtual Node   *create(Scene *scene);

    virtual int     getType() const { return VRML_NURBS_TEXTURE_SURFACE; }
};

class NodeNurbsTextureSurface : public NodeNurbsTextureCoordinate {
public:
                    NodeNurbsTextureSurface(Scene *scene, Proto *proto);
protected:
    virtual        ~NodeNurbsTextureSurface();

public:
    virtual int     getX3dVersion(void) const { return -1; }
    virtual Node   *copy() const { return new NodeNurbsTextureSurface(*this); }

    virtual bool    avoidProtoOnPureVrml(void) { return true; }
    int             writeProto(int filedes);
    int             write(int filedes, int indent);
};

#endif // _NODE_NURBS_TEXTURE_SURFACE_H
