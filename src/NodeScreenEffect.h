/*
 * NodeScreenEffect.h
 *
 * Copyright (C) 2011 J. "MUFTI" Scheurich
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

#ifndef _NODE_SCREEN_EFFECT_H
#define _NODE_SCREEN_EFFECT_H

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

class ProtoScreenEffect : public Proto {
public:
                    ProtoScreenEffect(Scene *scene);
    virtual Node   *create(Scene *scene);

    virtual int     getType() const { return KAMBI_SCREEN_EFFECT; }

    FieldIndex enabled;
    FieldIndex needsDepth;
    FieldIndex shaders;
};

class NodeScreenEffect : public Node {
public:
                    NodeScreenEffect(Scene *scene, Proto *proto);

//    virtual int     getProfile(void) const { return PROFILE_; }
//    virtual const char* getComponentName(void) const { return ""; }
//    virtual int         getComponentLevel(void) const { return -1; }
    virtual int     getX3dVersion(void) const { return -1; } 
    virtual Node   *copy() const { return new NodeScreenEffect(*this); }

    fieldMacros(SFBool, enabled, ProtoScreenEffect);
    fieldMacros(SFBool, needsDepth, ProtoScreenEffect);
    fieldMacros(MFNode, shaders, ProtoScreenEffect);
};

#endif
