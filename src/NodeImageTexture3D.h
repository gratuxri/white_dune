/*
 * NodeImageTexture3D.h
 *
 * Copyright (C) 2019 J. "MUFTI" Scheurich
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

#ifndef _NODE_IMAGE_TEXTURE_3D_H
#define _NODE_IMAGE_TEXTURE_3D_H

#ifndef _MESH_BASED_NODE_H
#include "MeshBasedNode.h"
#endif
#ifndef _PROTO_MACROS_H
#include "ProtoMacros.h"
#endif
#ifndef _PROTO_H
#include "Proto.h"
#endif

#include "KambiTextureCommonFields.h"

#include "SFMFTypes.h"

#include "freewrl_define.h"
#include "Texture3DNode.h"


class NodeVolumeData;
class MyMesh;

class textureTableIndexStruct;
typedef textureTableIndexStruct textureTableIndexStruct_s;
class Vector;
typedef struct pTextures{
    struct Vector *activeTextureTable;
    textureTableIndexStruct_s* loadThisTexture;

    /* current index into loadparams that texture thread is working on */
    int currentlyWorkingOn;// = -1;
    int textureInProcess;// = -1;
}* ppTextures;
#endif


class ProtoImageTexture3D : public Proto {
public:
                    ProtoImageTexture3D(Scene *scene);
    virtual Node   *create(Scene *scene);

    virtual int     getType() const { return X3D_IMAGE_TEXTURE_3D; }
    virtual int     getNodeClass() const 
                       { return TEXTURE_3D_NODE | URL_NODE; }

    virtual bool    isX3dInternalProto(void) { return true; }

    virtual bool    isMesh(void) { return true; }

    FieldIndex url;
    FieldIndex repeatS;
    FieldIndex repeatT;
    FieldIndex repeatR;
    FieldIndex textureProperties;
    kambiTextureCommonFieldIndex()
    FieldIndex crossOrigin;
    FieldIndex hideChildren;
    FieldIndex origChannelCount;
    FieldIndex scale;
};

class NodeImageTexture3D : public Texture3DNode, public Node {
public:
                    NodeImageTexture3D(Scene *scene, Proto *proto);

    virtual const char* getComponentName(void) const { return "Texturing3D"; }
    virtual int         getComponentLevel(void) const { return 2; }
    virtual int     getX3dVersion(void) const { return 1; }
    virtual Node   *copy() const 
                        { return (Node *)new NodeImageTexture3D(*this); }

    bool            texture_load_from_file(textureTableIndexStruct_s* tti, 
                                           const char*filename);
    virtual void    load();
    virtual void    draw(int pass);
    virtual void    preDraw() 
                        {
                        load(); 
                        Texture3DNode::preDraw(); 
                        }
    virtual void    setField(int index, FieldValue *value, 
                             int containerField = -1);

    fieldMacros(MFString, url,               ProtoImageTexture3D);
    fieldMacros(SFBool,   repeatS,           ProtoImageTexture3D);
    fieldMacros(SFBool,   repeatT,           ProtoImageTexture3D);
    fieldMacros(SFBool,   repeatR,           ProtoImageTexture3D);
    fieldMacros(SFNode,   textureProperties, ProtoImageTexture3D);
    kambiTextureCommonFieldMacros(ProtoImageTexture3D)
    fieldMacros(SFString, crossOrigin,       ProtoImageTexture3D);
    fieldMacros(SFBool,   hideChildren,      ProtoImageTexture3D);
    fieldMacros(SFInt32,  origChannelCount,  ProtoImageTexture3D);
    fieldMacros(SFBool,   scale,             ProtoImageTexture3D);

public:
    bool                       m_loaded;
    int                        m_textureTableIndex;
    ppTextures                 m_textures_prv;
    textureTableIndexStruct_s *m_tableIndex;
};

// The following uses code from FreeWRL
/****************************************************************************
    ... is part of the FreeWRL/FreeX3D Distribution.

    Copyright 2009 CRC Canada. (http://www.crc.gc.ca)

    FreeWRL/FreeX3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FreeWRL/FreeX3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeWRL/FreeX3D.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#ifndef MAX_MULTITEXTURE
#define MAX_MULTITEXTURE 4
#endif

#define uint32 unsigned int

#define ConsoleMessage(...) FPRINTF(stderr, __VA_ARGS__)

#define GLDOUBLE double

typedef struct _s_list_t {

    void *elem;
    struct _s_list_t *next;

} s_list_t;

#define ml_elem(_item) (_item->elem)
#define ml_next(_item) (_item->next)

typedef void f_free_t(void *ptr);
#if defined(DEBUG_MALLOC) && defined(DEBUG_MALLOC_LIST)
extern s_list_t* _ml_new(const void *elem, int line, char *fi);
#define ml_new(elem) _ml_new(elem,__LINE__,__FILE__)
#else
extern s_list_t* ml_new(const void *elem);
#endif

struct tRenderTextures{
    //struct multiTexParams textureParameterStack[MAX_MULTITEXTURE];
    void *textureParameterStack;
    void *prv;
};//RenderTextures;

typedef struct {

    const char *renderer; /* replace GL_REN */
    const char *version;
    const char *vendor;
    const char *extensions;
    float versionf;
    bool have_GL_VERSION_1_1;
    bool have_GL_VERSION_1_2;
    bool have_GL_VERSION_1_3;
    bool have_GL_VERSION_1_4;
    bool have_GL_VERSION_1_5;
    bool have_GL_VERSION_2_0;
    bool have_GL_VERSION_2_1;
    bool have_GL_VERSION_3_0;

    bool av_multitexture; /* Multi textures available ? */
    bool av_npot_texture; /* Non power of 2 textures available ? */
    bool av_texture_rect; /* Rectangle textures available ? */
    bool av_occlusion_q;  /* Occlusion query available ? */
    
    int texture_units;
    int runtime_max_texture_size;
    int system_max_texture_size;
    float anisotropicDegree;
    
    GLboolean quadBuffer;        /* does platform support quadbuffer? */

} s_renderer_capabilities_t;

enum {
    IMAGETYPE_UNKNOWN = 0,
    IMAGETYPE_PNG = 1,
    IMAGETYPE_JPEG,
    IMAGETYPE_GIF,
    IMAGETYPE_DDS,
    IMAGETYPE_WEB3DIT,
    IMAGETYPE_NRRD,
    IMAGETYPE_VOL,
};

#define TEXTURE_INVALID 0

/* appearance does material depending on last texture depth */
#define NOTEXTURE 0
#define TEXTURE_NO_ALPHA 1
#define TEXTURE_ALPHA 2

/* for texIsloaded structure */
#define TEX_NOTLOADED       0
#define TEX_LOADING         1
#define TEX_READ            2
#define TEX_NEEDSBINDING    3
#define TEX_LOADED          4
#define TEX_UNSQUASHED      5
#define TEX_NOTFOUND        6

/* Texture loading table :
   newer Texture handling procedures
   each texture has this kind of structure
*/

class textureTableIndexStruct {
public:
    Node*  scenegraphNode;
    int    nodeType;
    int    status;
    int    hasAlpha;
    GLuint OpenGLTexture;
    GLuint ifbobuffer; //in case this texture is used as an fbo render target
    GLuint idepthbuffer; //in case this texture is used as an fbo render target
    int    frames;
    char   *filename;
    int    x;
    int    y;
    int    z;
    int    tiles[3]; //when using TILED emulator for texture3D, nx, ny tiles, and resampled z
    unsigned char *texdata;
    GLint  repeatSTR[3]; //repeatR - used for non-builtin-Texture3D ie shader will manually apply this rule
    GLint magFilter; //needed in TEX3D frag shader for Z
    int textureNumber;
    int channels; //number of original image file image channels/components 0=no texture default, 1=Intensity 2=IntensityAlpha 3=RGB 4=RGBA
};

#define IBOOL int

#define INSTANCEGLOBAL 1

typedef struct pLoadTextures{
    s_list_t* texture_request_list;// = NULL;
    bool loader_waiting;// = false;
    /* list of texture table entries to load */
    s_list_t *texture_list;// = NULL;
    /* are we currently active? */
    int TextureParsing; // = FALSE;
}* ppLoadTextures;


