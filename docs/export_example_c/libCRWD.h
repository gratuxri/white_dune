/*libCRWD Library based on 
  libC++RWD Library for C++ Rendering of White_dune Data (in Development)*/

/* Copyright (c) Stefan Wolf, 2010. */
/* Copyright (c) J. "MUFTI" Scheurich, 2010-2019. */

/*
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#define __USE_XOPEN 1 /* for M_PI */
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>

#ifdef _WIN32
# define random(x) rand(x)
#endif

#define X3dNode void

static int aspect = 1;

#define Z_NEAR 0.05f
#define Z_FAR 7000.0f

#define EPSILON 1E-9

static short viewpointRendered = 0;
static short viewPointExists = 0;

static short lightExists = 0;
static int numLights = 0;

static GLfloat viewpoint1Position[] = { 0, 0, 10 };

static float projectionMatrix[16];

static short preRender = 0;
static short initRender = 0;

static struct X3dSceneGraph scenegraph;


static void error(const char *errormsg)
{
    #ifdef WIN32
    MessageBox(NULL, errormsg, NULL, MB_OK);
    #else
    fprintf(stderr, "%s\n", errormsg);
    #endif
}

static int allocLight(void)
{
    GLint maxLights;
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);
    
    if (numLights >= maxLights) {
        error("too many lights!\n");
        return GL_LIGHT0;
    }
    
    return (GL_LIGHT0 + numLights++);
}

void CRWDIndexedFaceSetRender(X3dNode *data, void *extraData)
{
    if(preRender)
    {
    }
    else if(initRender)
    {
    }
    else
    {
        struct X3dIndexedFaceSet *Xindexedfaceset = (struct X3dIndexedFaceSet*)data;
        struct X3dCoordinate *Xcoordinate = (struct X3dCoordinate*)Xindexedfaceset->coord;
        struct X3dNormal *Xnormal = (struct X3dNormal*)Xindexedfaceset->normal;
        int colorRGBA = 0;
        X3dNode *Xcolor = NULL;
        struct X3dTextureCoordinate *Xtexturecoordinate = (struct X3dTextureCoordinate*)Xindexedfaceset->texCoord;
        GLint *faces = NULL;
        GLfloat *vertex = NULL;
        GLint *normalindex = NULL;
        GLfloat *normal = NULL;
        GLfloat *colors = NULL;
        GLint *colorindex = NULL;
        GLint *texturecoordinateindex = NULL;
        int faces_len, vertex_len, normalindex_len, normal_len, color_len, colorindex_len, texturecoordinateindex_len;
        short normalpervertex, colorpervertex;
        int i;
        int buffer, normalbuffer, texturebuffer;
        int facecounter = 0;

        if (Xindexedfaceset->color) 
            if (((struct X3dColorRGBA*)Xindexedfaceset->color)->m_type == X3dColorRGBAType) {
                Xcolor = (struct X3dColorRGBA*)Xindexedfaceset->color;
                colorRGBA = 1;
            } else
                Xcolor = (struct X3dColor*)Xindexedfaceset->color;;

        if(Xcolor != NULL)
        {
            glEnable(GL_COLOR_MATERIAL); 
            glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
            if (colorRGBA) {
                colors = ((struct X3dColorRGBA *)Xcolor)->color;
                color_len = ((struct X3dColorRGBA *)Xcolor)->color_length;
            } else {
                colors = ((struct X3dColor *)Xcolor)->color;
                color_len = ((struct X3dColor *)Xcolor)->color_length;
            }
            colorpervertex = Xindexedfaceset->colorPerVertex;
        }
        if(Xindexedfaceset->colorIndex != NULL)
        {
            colorindex = Xindexedfaceset->colorIndex;
            colorindex_len = Xindexedfaceset->colorIndex_length;
        }
        if(Xindexedfaceset->texCoordIndex != NULL)
        {
            texturecoordinateindex = Xindexedfaceset->texCoordIndex;
            texturecoordinateindex_len = Xindexedfaceset->texCoordIndex_length;
        }
        vertex = Xcoordinate->point;
        faces = Xindexedfaceset->coordIndex;
        normalindex = Xindexedfaceset->normalIndex;
        if(Xnormal!=NULL)normal = Xnormal->vector;
        if (extraData != NULL)
            normal = (float *)extraData;
        vertex_len = Xcoordinate->point_length;
        faces_len = Xindexedfaceset->coordIndex_length;
        if(Xnormal != NULL)normal_len = Xnormal->vector_length;
        normalindex_len = Xindexedfaceset->normalIndex_length;
        normalpervertex = Xindexedfaceset->normalPerVertex;
        if (Xindexedfaceset->ccw != 0)
        {
           glFrontFace(GL_CCW);
        }
        else
        {
           glFrontFace(GL_CW);
        }
        if (Xindexedfaceset->solid == 0) 
        {
           glDisable(GL_CULL_FACE);
           glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        } 
        else 
        {
           glEnable(GL_CULL_FACE);
           glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
        }

        glBegin(GL_POLYGON);
        if(colors && !colorpervertex)
            if (colorRGBA)
                glColor4f(colors[4 * facecounter], colors[4 * facecounter + 1], colors[4 * facecounter + 2], colors[4 * facecounter + 3]);
            else 
                glColor3f(colors[3 * facecounter], colors[3 * facecounter + 1], colors[3 * facecounter + 2]);


        for(i = 0; i != faces_len; i ++)
        {
            buffer = faces[i];
            if (buffer < 0) {
                glEnd();
                if (i != faces_len - 1)
                    glBegin(GL_POLYGON);
            }
            if(normalindex != NULL)
                normalbuffer = normalindex[i];
            else
                normalbuffer = buffer;
            if(texturecoordinateindex != NULL)
                texturebuffer = texturecoordinateindex[i];
            else
                texturebuffer = buffer;
            if(buffer == -1 && (normalbuffer == -1 || normalindex != NULL))
            {
                facecounter++;
                glEnd();
                glBegin(GL_POLYGON);
                if(colors && !colorpervertex)
                    glColor3f(colors[3 * facecounter], colors[3 * facecounter + 1], colors[3 * facecounter + 2]);
            }
            else if(((buffer == -1 && normalbuffer != -1) || (buffer != -1 && normalbuffer == -1)) && normalindex != NULL)
                error("Error in normalIndex");
            else
            {
                if(normalpervertex && normal != NULL)glNormal3f(normal[normalbuffer*3], normal[normalbuffer*3+1], normal[normalbuffer*3+2]);
                if(colors && !colorpervertex)
                    if (colorRGBA)
                        glColor4f(colors[4 * facecounter], colors[4 * facecounter + 1], colors[4 * facecounter + 2], colors[4 * facecounter + 3]);
                    else 
                         glColor3f(colors[3 * facecounter], colors[3 * facecounter + 1], colors[3 * facecounter + 2]);
            }
        }
        for(i = 0; i != faces_len; i++)
        {
            buffer = faces[i];
            if (buffer < 0) {
                glEnd();
                if (i != faces_len - 1)
                    glBegin(GL_POLYGON);
            }

            if(normalindex != NULL)
                normalbuffer = normalindex[i];
            else 
                normalbuffer = buffer;
            if(texturecoordinateindex != NULL)
                texturebuffer = texturecoordinateindex[i];
            else
                texturebuffer = buffer;
            if(buffer == -1 && (normalbuffer == -1 || normalindex != NULL))
            {
                facecounter++;
                glEnd();
                glBegin(GL_POLYGON);
                if(colors && !colorpervertex)
                    if (colorRGBA)
                        glColor4f(colors[4 * facecounter], colors[4 * facecounter + 1], colors[4 * facecounter + 2], colors[4 * facecounter + 3]);
                    else
                        glColor3f(colors[3 * facecounter], colors[3 * facecounter + 1], colors[3 * facecounter + 2]);
            }
            else if(((buffer == -1 && normalbuffer != -1) || (buffer != -1 && normalbuffer == -1)) && normalindex != NULL)
                error("Error in normalIndex");
            else
            {
                if(normalpervertex && normal != NULL)
                    glNormal3f(normal[normalbuffer*3], normal[normalbuffer*3+1], normal[normalbuffer*3+2]);
                if(colors && color_len > 0 && colorpervertex)
                    if (colorRGBA)
                        glColor4f(colors[buffer*4], colors[buffer*4+1], colors[buffer*4+2], colors[buffer*4+3]);
                    else
                        glColor3f(colors[buffer*3], colors[buffer*3+1], colors[buffer*3+2]);
                if(Xtexturecoordinate)
                    if((Xtexturecoordinate->point) && (texturecoordinateindex))
                        glTexCoord2f(Xtexturecoordinate->point[texturebuffer*2], Xtexturecoordinate->point[texturebuffer*2+1]);
                glVertex3f(vertex[buffer*3], vertex[buffer*3+1], vertex[buffer*3+2]);
            }
        }
        glEnd();
    }
    glDisable(GL_COLOR_MATERIAL); 
}

void CRWDIndexedFaceSetCreateNormals(X3dNode *data, void* extraData)
{
    if(initRender)
    {
        struct X3dIndexedFaceSet *Xindexedfaceset = (struct X3dIndexedFaceSet*)data;
        struct X3dCoordinate *Xcoordinate = (struct X3dCoordinate*)Xindexedfaceset->coord;
        struct X3dNormal *Xnormal = (struct X3dNormal*)Xindexedfaceset->normal;
        int *normalIndex = Xindexedfaceset->normalIndex;
        int normalIndexLen = Xindexedfaceset->normalIndex_length;
        int maxLen = -1;
        float **vec;
        int i;

        if (normalIndex == NULL) {
            normalIndex = Xindexedfaceset->coordIndex;
            normalIndexLen = Xindexedfaceset->coordIndex_length;
        } 
        for (i = 0; i < normalIndexLen; i++)
            if (normalIndex[i] > maxLen)
                maxLen = normalIndex[i];
        maxLen++;
        vec = (float **)extraData;
        *vec = (float *)malloc(maxLen * sizeof(float) * 3);
    }
    else
    {
        struct X3dIndexedFaceSet *Xindexedfaceset = (struct X3dIndexedFaceSet*)data;
        struct X3dCoordinate *Xcoordinate = (struct X3dCoordinate*)Xindexedfaceset->coord;
        struct X3dNormal *Xnormal = (struct X3dNormal*)Xindexedfaceset->normal;
        int *normalIndex = Xindexedfaceset->normalIndex;
        float *vec = *(float **)extraData;
        float creaseAngle = Xindexedfaceset->creaseAngle;
        int *ci = Xindexedfaceset->coordIndex;
        int ciLen = Xindexedfaceset->coordIndex_length;
        float *points = Xcoordinate->point;
        int pointLen = Xcoordinate->point_length;
        int i, j;

        if (normalIndex == NULL)
            normalIndex = Xindexedfaceset->coordIndex;
        for (i = 0; i < ciLen; i++) 
        {
            int first = -1;
            int second = -1;
            int third = -1;
            if (ci[i] > -1) {
                if ((i > 0) && (ci[i - 1] > -1))
                    first = i - 1;
                else 
                {
                    for (j = i; j < ciLen; j++)
                        if (ci[j] < 0)
                        {
                            first = j - 1;
                            break;
                        }
                    if (first == -1)
                        first = ciLen - 1;
                }
                second = i;
                if ((i + 1 < ciLen) && (ci[i + 1] > -1))
                    third = i + 1;
                else {
                    for (j = i; j > -1; j--)
                        if (ci[j] < 0) 
                        {
                            third = j + 1;
                            break;
                        }
                    if (third == -1)
                        third = 0;
                }
                if ((first > -1) && (second > -1) && (third > -1)) 
                {
                    float v1x = points[ci[first] * 3    ] - points[ci[second] * 3    ];
                    float v1y = points[ci[first] * 3 + 1] - points[ci[second] * 3 + 1];
                    float v1z = points[ci[first] * 3 + 2] - points[ci[second] * 3 + 2];

                    float v2x = points[ci[third] * 3    ] - points[ci[second] * 3    ];
                    float v2y = points[ci[third] * 3 + 1] - points[ci[second] * 3 + 1];
                    float v2z = points[ci[third] * 3 + 2] - points[ci[second] * 3 + 2];
    
                    float crossx = v1y * v2z - v1z * v2y;
                    float crossy = v1z * v2x - v1x * v2z;
                    float crossz = v1x * v2y - v1y * v2x;
    
                    float vLen = sqrt(crossx * crossx + crossy * crossy + crossz * crossz);
                    if (vLen > 0) {
                        vec[normalIndex[i] * 3    ] = - crossx / vLen;      
                        vec[normalIndex[i] * 3 + 1] = - crossy / vLen;      
                        vec[normalIndex[i] * 3 + 2] = - crossz / vLen;
                    } else {
                        vec[normalIndex[i] * 3    ] = 0;      
                        vec[normalIndex[i] * 3 + 1] = 0;      
                        vec[normalIndex[i] * 3 + 2] = 0;
                    }
                }
            }    
        }
    }
}

void CRWDTextRender(X3dNode *data, void* extraData)
{
    if(preRender)
    {
    }
    else if(initRender)
    {
    }
    else
    {
        int i,j;
        struct X3dText *text = (struct X3dText*)data;
        float fsize = 1;
        struct X3dFontStyle *fontStyle = (struct X3dFontStyle *)text->fontStyle;
        const float GLUT_STROKE_ROMAN_SIZE = 119.05;
        float scale = 1/GLUT_STROKE_ROMAN_SIZE;
    
        glPushAttrib(GL_ENABLE_BIT);
    
        glDisable(GL_LIGHTING);
        glDisable(GL_BLEND);
        
        glEnable(GL_LINE_SMOOTH);
    
        if (fontStyle != NULL)
           fsize = fontStyle->size;
    
        for (j = 0; j < text->string_length;j++) 
        {
            float fsize = 1;
            const char *str = text->string[j];
            glPushMatrix();
            glTranslatef(0, -j * fsize, 0);
            glScalef(scale * fsize, scale * fsize, 1.0);
            for (i = 0; i < strlen(str); i++)
                glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
            glPopMatrix();
        }
        glPopAttrib();
    }
}

static int not2PowN(int number)
{
    int ret = -1;
    int i;
    for (i = 0; i < 32; i++)
        if (number == (1 << i))
        {
            ret = 0;
            break;
        }
    return ret;
}

static void textureTransformBind(X3dNode *data)
{
    int i;
    struct X3dTextureTransform *texTransform = (struct X3dTextureTransform*) 
                                               data;
    if(preRender)
    {
    }
    else if(initRender)
    {
    }
    else 
    {
        const float *fcenter = texTransform->center;
        const float *fscale = texTransform->scale;
        const float *ftranslation = texTransform->translation;

        glPushMatrix();
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glTranslatef(-fcenter[0], -fcenter[1], 0.0f);
        glScalef(fscale[0], fscale[1], 1.0f);
        glRotatef(texTransform->rotation * 180.0f / M_PI, 0.0f, 0.0f, 1.0f);
        glTranslatef(fcenter[0], fcenter[1], 0.0f);
        glTranslatef(ftranslation[0], ftranslation[1], 0.0f);
        glMatrixMode(GL_MODELVIEW);
    }
}

static void textureTransformUnBind(void)
{
    glPopMatrix();
}

static void renderIntTexture(GLuint *textureName, int height, int width, 
                             int format, GLuint *data, int repeatS, int repeatT)
{
    if (*textureName == 0) {
        glGenTextures(1, textureName);
        glBindTexture(GL_TEXTURE_2D, *textureName);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
                     GL_UNSIGNED_BYTE, data);
    } else {
        glBindTexture(GL_TEXTURE_2D, *textureName);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 
                    repeatS ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 
                    repeatT ? GL_REPEAT : GL_CLAMP);
    glEnable(GL_TEXTURE_2D);
}

static void renderByteTexture(GLuint *textureName, int height, int width,
                              int format, GLbyte *data,
                              int repeatS, int repeatT)
{
    if (*textureName == 0) {
        glGenTextures(1, textureName);
        glBindTexture(GL_TEXTURE_2D, *textureName);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, 
                     GL_UNSIGNED_BYTE, data);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    } else {
        glBindTexture(GL_TEXTURE_2D, *textureName);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 
                    repeatS ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 
                    repeatT ? GL_REPEAT : GL_CLAMP);
    glEnable(GL_TEXTURE_2D);
}

static int errorFlag = 0;
static jmp_buf jpegEnv;

struct ImageTextureExtraDataStruct {
    unsigned int height;
    unsigned int width;
    unsigned int colorMode;
    unsigned char *pixels;
    GLuint* textureName;
};

static void CRWDJpgErrorExit(j_common_ptr cinfo)
{
    longjmp(jpegEnv,1);
    errorFlag = 1;
}

static int CRWDJpgLoad(struct jpeg_decompress_struct *cinfo, FILE *file, 
                       unsigned int *width, unsigned int *height, 
                       unsigned int* components)
{
    static struct jpeg_error_mgr jpgError;

    errorFlag=0;
    if (setjmp(jpegEnv)!=0)
    {
        errorFlag = 1;
        return 1;
    }
    cinfo->err = jpeg_std_error(&jpgError);
    cinfo->err->error_exit = CRWDJpgErrorExit;
    jpeg_create_decompress(cinfo);

    jpeg_stdio_src(cinfo, file);
    jpeg_read_header(cinfo, 1);
    jpeg_start_decompress(cinfo);

    *width = cinfo->output_width;
    *height = cinfo->output_height;
    *components = cinfo->output_components;

    return errorFlag;
}

static int CRWDJpgRead(struct jpeg_decompress_struct *cinfo, 
                       unsigned char *pixels)
{
    JSAMPARRAY buffer;
    int rowStride;
    unsigned char *ptr;
    int buffer_height = 1;

    rowStride = cinfo->output_width * cinfo->output_components;
    buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW) * buffer_height);
    buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * rowStride);
    
    ptr = pixels + rowStride * (cinfo->output_height - 1);
    while (cinfo->output_scanline < cinfo->output_height) 
    {
        jpeg_read_scanlines(cinfo, buffer, 1);
        memcpy(ptr, buffer[0], rowStride);
        ptr -= rowStride;
    }

    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);
    free(buffer[0]);
    free(buffer);
    return errorFlag;
}

static double getGammaExp()
{
    static double defaultExponent = 2.2;
    static int set = 0;

    if (!set)
    {
#if defined(NeXT)
        defaultExponent = 1.0;
#elif defined(__sgi)
        defaultExponent = 1.7;
#elif defined(MACOSX)
        defaultExponent = 1.5;
#endif
        set = 1;
    }
    return defaultExponent;
}
#ifndef png_jmpbuf 
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf) 
#endif 

#define PNG_SIGNATURE_SIZE 8

static int CRWDPngLoad(FILE *f, png_structp *pngPtr, png_infop *infoPtr,
                       unsigned int *width, unsigned int *height, 
                       unsigned int *components)
{
    unsigned char signature[PNG_SIGNATURE_SIZE];
    
    int bitDepth;
    int colorType;
    int interlaceType;
    int grayPalette;

    fread(signature, 1, PNG_SIGNATURE_SIZE, f);
    if (!png_check_sig(signature, PNG_SIGNATURE_SIZE))
    {
        errorFlag = 1;
        fclose(f);
        return 1;
    }
    *pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);    
    if (*pngPtr == NULL)
    {
        errorFlag = 1;
        fclose(f);
        return 1;
    }
    *infoPtr = png_create_info_struct(*pngPtr);
    if (*infoPtr == NULL)
    {
        errorFlag = 1;
        png_destroy_read_struct(pngPtr, NULL, NULL);
        fclose(f);
        return 1;
    }
    if (setjmp(png_jmpbuf(*pngPtr)))
    {
        png_destroy_read_struct(pngPtr, infoPtr, NULL);
        errorFlag = 1;
        fclose(f);
        return 1;
    }
#ifndef PNG_NO_STDIO
    png_init_io(*pngPtr, f);
#endif
    png_set_sig_bytes(*pngPtr, PNG_SIGNATURE_SIZE);
    png_read_info(*pngPtr, *infoPtr);
    png_get_IHDR(*pngPtr, *infoPtr, width, height, &bitDepth, &colorType,
                 &interlaceType, NULL, NULL);
    png_set_strip_16(*pngPtr);
    png_set_packing(*pngPtr);
    *components = png_get_channels(*pngPtr, *infoPtr);

    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_expand(*pngPtr);
        *components = 3;
    }

    if ((colorType == PNG_COLOR_TYPE_GRAY) && (bitDepth < 8))
        png_set_expand(*pngPtr);

    if (png_get_valid(*pngPtr, *infoPtr, PNG_INFO_tRNS))
    {
      png_set_expand(*pngPtr);
      *components = *components + 1;
    }
    
    grayPalette = 0;
    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
        int n;
        int numPalette;
        png_colorp palette;

        if (png_get_PLTE(*pngPtr, *infoPtr, &palette, &numPalette))
        {
          grayPalette = 1;
          for (n = 0; n < numPalette; ++n)
            if ((palette[n].red != palette[n].green) ||
                (palette[n].red != palette[n].blue))
            {
                grayPalette = 0;
                break;
            }
        }
    }
    
    {
        double gamma;
        double exponent = getGammaExp();
        if (png_get_gAMA(*pngPtr, *infoPtr, &gamma))
            png_set_gamma(*pngPtr, exponent, gamma);
        else
            png_set_gamma(*pngPtr, exponent, 0.45455);
    }

    png_read_update_info(*pngPtr, *infoPtr);
    png_get_IHDR(*pngPtr, *infoPtr, width, height, &bitDepth, &colorType,
                 &interlaceType, NULL, NULL);
    return 0;
}

static int CRWDPngRead(png_structp *pngPtr, png_infop *infoPtr,
                       struct ImageTextureExtraDataStruct *data, unsigned int *components)
{
    int rowStride = *components * data->width;
    png_bytep *rowPtrs;
    int row;
    unsigned char *ptr;
    int i;

    rowPtrs = (png_bytep *) malloc(sizeof(png_bytep) * data->height);

    for (row = 0; row < data->height; row++)
    {
        rowPtrs[row] = (png_bytep)png_malloc(*pngPtr, 
                            png_get_rowbytes(*pngPtr, *infoPtr));
    }

    png_read_image(*pngPtr, rowPtrs);

    ptr = data->pixels + rowStride * (data->height - 1);
    for (i = 0; i < data->height; i++)
    {
        memcpy(ptr, rowPtrs[i], rowStride);
        ptr -= rowStride;
    }
    png_destroy_read_struct(pngPtr, infoPtr, NULL);
    free(rowPtrs);
}

void CRWDImageTextureRender(X3dNode *data, void *extraData)
{
    int i;
    struct ImageTextureExtraDataStruct *extraVar;

    struct X3dImageTexture *imageTexture = (struct X3dImageTexture*) data;
    extraVar = (struct ImageTextureExtraDataStruct *)imageTexture->extra_data;
    if(preRender)
    {
    }
    else if(initRender)
    {
        int i;
        const char *filename;
        unsigned int components;
        struct jpeg_decompress_struct cinfo;
        png_structp pngPtr;
        png_infop infoPtr;
        FILE *file;

        imageTexture->extra_data = malloc(sizeof(struct 
                                          ImageTextureExtraDataStruct));
        extraVar = (struct ImageTextureExtraDataStruct *)
                    imageTexture->extra_data;
        extraVar->textureName = malloc(sizeof(GLuint));
        *(extraVar->textureName) = 0;
        filename = *(imageTexture->url);

        if (!filename)
            return;
        else if (strlen(filename) == 0)
            return;
        else if (!(file = fopen(filename, "rb")))
            return;

        /* try to handle file as jpeg file */
        CRWDJpgLoad(&cinfo, file, &(extraVar->width), &(extraVar->height), 
                    &components);
        if (!errorFlag)
        {
            extraVar->pixels = malloc(components *
                                      extraVar->width * extraVar->height);
            if (extraVar->pixels == NULL)
                return;        

            CRWDJpgRead(&cinfo, extraVar->pixels);
            extraVar->colorMode = GL_RGB;
        }
        fclose(file);
        if (errorFlag) 
        { 
            /* try to handle file as PNG file) */
            file = fopen(filename, "rb");
            if (!CRWDPngLoad(file, &pngPtr, &infoPtr, 
                             &(extraVar->width), &(extraVar->height),
                             &components))
            {
                extraVar->pixels = malloc(components *
                                          extraVar->width * extraVar->height);
                if (extraVar->pixels == NULL)
                     return;
                 CRWDPngRead(&pngPtr, &infoPtr, extraVar, &components);
            }
            fclose(file);
        }

        extraVar->colorMode = GL_RGBA;
        switch (components)
        {
          case 1:
            extraVar->colorMode = GL_LUMINANCE;
            break;
          case 2:
            extraVar->colorMode = GL_LUMINANCE_ALPHA;
            break;
          case 3:
            extraVar->colorMode = GL_RGB;
            break;          
        }
        if (not2PowN(extraVar->width))
            error("warning: texture width is not 2 pow N");            
        if (not2PowN(extraVar->height))
            error("warning: texture height is not 2 pow N");            
        if (extraVar->width != extraVar->height)
            error("warning: texture width and height is not equal\n");
    } 
    else 
    {
        X3dNode *textureTransform = NULL;
        if (imageTexture->m_parent)
            textureTransform = ((struct X3dAppearance *)
                                imageTexture->m_parent)->textureTransform;

        if (textureTransform)
            textureTransformBind(textureTransform);
        glMatrixMode(GL_TEXTURE);
        renderByteTexture(extraVar->textureName,
                          extraVar->width, extraVar->height, 
                          extraVar->colorMode, 
                          (GLbyte *)(extraVar->pixels), 
                          imageTexture->repeatS, imageTexture->repeatT); 
        glMatrixMode(GL_MODELVIEW);
        if (textureTransform)
            textureTransformUnBind();
    }
}

void CRWDPixelTextureRender(X3dNode *data, void *extraData)
{
    int i;

    struct X3dPixelTexture *pixelTexture = (struct X3dPixelTexture*) data;
    GLuint *textureName = (GLuint *) pixelTexture->extra_data;
    if(preRender)
    {
    }
    else if(initRender)
    {
        int i;
        int bigEndian = 1;

        bigEndian = (*(unsigned char *)&bigEndian) == 0;

        pixelTexture->extra_data = malloc(sizeof(GLuint));
        textureName = (GLuint *) pixelTexture->extra_data;
        *textureName = 0;

        if (not2PowN(pixelTexture->image[0]))
            error("warning: texture width is not 2 pow N");            
        if (not2PowN(pixelTexture->image[1]))
            error("warning: texture height is not 2 pow N");            
        if (pixelTexture->image[0] != pixelTexture->image[1])
            error("warning: texture width and height is not equal\n");
        /* store modified colors back to pixelTexture.image */
        for (i = 3; i < pixelTexture->image_length; i++) 
        {
            int a = 0xff;
            int r,g,b;
            switch (pixelTexture->image[2])
            {
              case 4:
                r = (pixelTexture->image[i]&0xFF000000) >> 24;
                g = (pixelTexture->image[i]&0x00FF0000) >> 16;
                b = (pixelTexture->image[i]&0x0000FF00) >> 8;
                a = (pixelTexture->image[i]&0x000000FF);
                break;
              case 3:
                r = (pixelTexture->image[i]&0x00FF0000) >> 16;
                g = (pixelTexture->image[i]&0x0000FF00) >> 8;
                b = (pixelTexture->image[i]&0x000000FF);
                break;
              case 2:
                r = g = b = (pixelTexture->image[i]&0x0000FF00) >> 8;
                a = (pixelTexture->image[i]&0x000000FF);
                break;
              case 1:
                r = g = b = (pixelTexture->image[i]&0xFF);
                break;
            }
            if (bigEndian)
            {
                pixelTexture->image[i] = a | 
                                         (b << 8) | (g << 16) | (r << 24);
            }
            else
            {
                pixelTexture->image[i] = r | (g << 8) | (b << 16) | 
                                         (a << 24);
            }
        }
    }
    else 
    {
        X3dNode *textureTransform;
        textureTransform = ((struct X3dAppearance *)pixelTexture->m_parent)->
                           textureTransform;

        if (textureTransform)
            textureTransformBind(textureTransform);
        glMatrixMode(GL_TEXTURE);
        renderIntTexture(textureName,
                         pixelTexture->image[0], pixelTexture->image[1], 
                         GL_RGBA, (GLuint *)&pixelTexture->image[3], 
                         pixelTexture->repeatS, pixelTexture->repeatT); 
        glMatrixMode(GL_MODELVIEW);
        if (textureTransform)
            textureTransformUnBind();
    }
}

void CRWDPointLightRender(X3dNode *data, void *extraData)
{
    struct X3dPointLight *lightNode = (struct X3dPointLight*)data;
    int light = -1;
    if(preRender && lightNode->on)
    {
        int i;
        GLfloat color[4];
        GLfloat ambient_color[4];
        GLfloat position[4];
        GLfloat attenuation[3];

        for (i = 0; i < 3; i++) 
            color[i] = lightNode->color[i] * lightNode->intensity;
        color[3] = 1;

        for (i = 0; i < 3; i++) 
            ambient_color[i] = lightNode->color[i] * lightNode->ambientIntensity;
        ambient_color[3] = 1;

        for (i = 0; i < 3; i++) 
            position[i] = lightNode->location[i];
        if (extraData == NULL)
            position[3] = 1.0f;
        else
            position[3] = *((float *)extraData);

        for (i = 0; i < 3; i++) 
            attenuation[i] = lightNode->attenuation[i];

        light = *(int *)(lightNode->extra_data);
        glLightfv(light, GL_DIFFUSE, color);
        if (extraData != NULL)
            glLightfv(light, GL_SPECULAR, color);
        glLightfv(light, GL_AMBIENT, ambient_color);
        glLightfv(light, GL_POSITION, position);
        glLightf(light, GL_CONSTANT_ATTENUATION, attenuation[0]);
        glLightf(light, GL_LINEAR_ATTENUATION, attenuation[1]);
        glLightf(light, GL_QUADRATIC_ATTENUATION, attenuation[2]);
        glLightf(light, GL_SPOT_CUTOFF, 180.0f);
        glLightf(light, GL_SPOT_EXPONENT, 0.0f);
        glEnable(light);
    }
    else if(initRender)
    {
        lightExists = -1;
        lightNode->extra_data = malloc(sizeof(int));
        *(int *)(lightNode->extra_data) = allocLight();
    }
}

void CRWDDirectionalLightRender(X3dNode *data, void *extraData)
{
    struct X3dDirectionalLight *lightNode = (struct X3dDirectionalLight*)data;
    int light = -1;
    if(preRender && lightNode->on)
    {
        int i;
        GLfloat color[4];
        GLfloat ambient_color[4];
        GLfloat position[4];

        for (i = 0; i < 3; i++) 
            color[i] = lightNode->color[i] * lightNode->intensity;
        color[3] = 1;

        for (i = 0; i < 3; i++) 
            ambient_color[i] = lightNode->color[i] * lightNode->ambientIntensity;
        ambient_color[3] = 1;

        for (i = 0; i < 3; i++)
            position[i] = -lightNode->direction[i];
        position[3] = 0.0f;

        light = *(int *)(lightNode->extra_data);
        glLightfv(light, GL_AMBIENT, ambient_color);
        glLightfv(light, GL_DIFFUSE, color);
        glLightfv(light, GL_POSITION, position);
        glLightfv(light, GL_SPECULAR, color);
        glLightf(light, GL_SPOT_CUTOFF, 180.0f);
        glLightf(light, GL_SPOT_EXPONENT, 0.0f);
        glLightf(light, GL_CONSTANT_ATTENUATION, 1);
        glLightf(light, GL_LINEAR_ATTENUATION, 0);
        glLightf(light, GL_QUADRATIC_ATTENUATION, 0);
        glEnable(light);
    }
    else if(initRender)
    {
        lightExists = -1;
        lightNode->extra_data = malloc(sizeof(int));
        *(int *)(lightNode->extra_data) = allocLight();
    }
}

void CRWDSpotLightRender(X3dNode *data, void *extraData)
{
    struct X3dSpotLight *lightNode = (struct X3dSpotLight*)data;
    int light = -1;
    if(preRender && lightNode->on)
    {
        int i;
        GLfloat color[4];
        GLfloat ambient_color[4];
        GLfloat position[4];
        GLfloat ambientIntensity[4];

        for (i = 0; i < 3; i++) 
            color[i] = lightNode->color[i] * lightNode->intensity;
        color[3] = 1;

        for (i = 0; i < 3; i++) 
            ambient_color[i] = lightNode->color[i] * lightNode->ambientIntensity;
        ambient_color[3] = 1;

        for (i = 0; i < 3; i++)
            position[i] = lightNode->direction[i];
        position[3] = 1.0f;

        light = *(int *)(lightNode->extra_data);
        glLightfv(light, GL_AMBIENT, ambient_color);
        glLightfv(light, GL_DIFFUSE, color);
        glLightfv(light, GL_POSITION, position);
        glLightfv(light, GL_SPECULAR, color);
        glLightfv(light, GL_SPOT_DIRECTION, lightNode->direction);
        glLightf(light, GL_SPOT_CUTOFF, lightNode->cutOffAngle * 180.0f / M_PI);
        glLightf(light, GL_SPOT_EXPONENT, lightNode->beamWidth < lightNode->cutOffAngle ? 1.0f : 0.0f);
        glLightf(light, GL_CONSTANT_ATTENUATION, lightNode->attenuation[0]);
        glLightf(light, GL_LINEAR_ATTENUATION, lightNode->attenuation[1]);
        glLightf(light, GL_QUADRATIC_ATTENUATION, lightNode->attenuation[2]);
        glEnable(light);
    }
    else if(initRender)
    {
        lightExists = -1;
        lightNode->extra_data = malloc(sizeof(int));
        *(int *)(lightNode->extra_data) = allocLight();
    }
}

void CRWDMaterialRender(X3dNode *data, void *extraData)
{
    if(preRender)
    {
    }
    else if(initRender)
    {
    }
    else
    {
        int i;
        struct X3dMaterial *material = (struct X3dMaterial*)data;
        GLfloat diffuse_color[4];
        GLfloat ambient;
        GLfloat ambient_color[4];
        GLfloat emissive_color[4];
        GLfloat specuar_color[4];
        GLfloat shininess;

        for (i = 0; i < 3; i++)
            diffuse_color[i] = material->diffuseColor[i];
        diffuse_color[3] = 1;

        ambient = material->ambientIntensity; 

        for (i = 0; i < 3; i++)
            ambient_color[i] = ambient * material->diffuseColor[i];
        ambient_color[3] = 1;

        for (i = 0; i < 3; i++)
            emissive_color[i] = material->emissiveColor[i];
        emissive_color[3] = 1;

        for (i = 0; i < 3; i++)
            specuar_color[i] = material->specularColor[i];
        specuar_color[3] = 1;
        
        shininess = material->shininess*128.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emissive_color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specuar_color);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }
}

void CRWDGroupTreeRender(X3dNode *data, void *extraData)
{
    struct X3dGroup *group = (struct X3dGroup*)data;
    int i;
    glPushMatrix();
    if (group->children)
       for (i = 0; i < group->children_length; i++)
           if (group->children[i])
           {
               if(!(preRender || initRender))
                   glDisable(GL_TEXTURE_2D);
               X3dTreeRenderCallback(group->children[i], extraData);
           }
    glPopMatrix();
}

void CRWDTransformTreeRender(X3dNode *data, void *extraData)
{
    struct X3dTransform *transform = (struct X3dTransform*)data;
    int i;
    if(preRender || initRender)
    {
        if (transform->children)
            for (i = 0; i < transform->children_length; i++)
                if (transform->children[i])
                    X3dTreeRenderCallback(transform->children[i], extraData);
    }
    else
    {
        glPushMatrix();
        glTranslatef(transform->translation[0], transform->translation[1], transform->translation[2]);
        glTranslatef(transform->center[0], transform->center[1], transform->center[2]);
        glRotatef( ( (transform->rotation[3] / (2*M_PI) ) * 360), transform->rotation[0], transform->rotation[1], transform->rotation[2]);
        glRotatef( ( (transform->scaleOrientation[3] / (2*M_PI) ) * 360), transform->scaleOrientation[0], transform->scaleOrientation[1], transform->scaleOrientation[2]);
        glScalef(transform->scale[0], transform->scale[1], transform->scale[2]);
        glRotatef( ( (transform->scaleOrientation[3] / (2*M_PI) ) * 360) * -1, transform->scaleOrientation[0], transform->scaleOrientation[1], transform->scaleOrientation[2]);
        glTranslatef(transform->center[0] * -1, transform->center[1] * -1, transform->center[2] * -1);
        if (transform->children)
            for (i = 0; i < transform->children_length; i++)
                if (transform->children[i])
                {
                    glDisable(GL_TEXTURE_2D);
                    X3dTreeRenderCallback(transform->children[i], extraData);
                }
        glPopMatrix();
    }
}

void CRWDViewpointRender(X3dNode *data, void* extraData)
{
    int i;
    struct X3dViewpoint *viewpoint = (struct X3dViewpoint*)data;
    if(preRender || initRender)
    {
        viewPointExists = -1;

        if(!viewpointRendered)
        {
            float fieldOfViewdegree;
            viewpointRendered = -1;
            for (i = 0; i < 3; i++)
                viewpoint1Position[i] = viewpoint->position[i];

            glMatrixMode(GL_PROJECTION);
            fieldOfViewdegree = ( (viewpoint->fieldOfView / M_PI ) * 180.0f);

            if(initRender) {
                gluPerspective(fieldOfViewdegree, 1.0, Z_NEAR, Z_FAR);  /* fieldOfView in degree, aspect radio, Z nearest, Z farest */
                glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
            } else if(preRender)
                glLoadMatrixf(projectionMatrix);

            glRotatef( ( -(viewpoint->orientation[3] / (2*M_PI) ) * 360), viewpoint->orientation[0], viewpoint->orientation[1], viewpoint->orientation[2]);
            glTranslatef(-viewpoint->position[0], -viewpoint->position[1], -viewpoint->position[2]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }
    else
    {
    }
}

void CRWDSwitchTreeRender(X3dNode *data, void *extraData)
{
    struct X3dSwitch *nodeSwitch = (struct X3dSwitch *)data;
    glPushMatrix();
    if (nodeSwitch->children)
       if (nodeSwitch->whichChoice < nodeSwitch->children_length)
           if (nodeSwitch->children[nodeSwitch->whichChoice])
           {
               if(!(preRender || initRender))
                   glDisable(GL_TEXTURE_2D);
               X3dTreeRenderCallback(nodeSwitch->children[nodeSwitch->whichChoice], 
                                     extraData);
           }
    glPopMatrix();
}

struct HAnimJointExtraDataStruct {
    float jointMatrix[16];
    float thisMatrix[16];
};

void multMatrixVec(float *ret, float mat[16], float x, float y, float z)
{
    float s = mat[4*0+3]*x + mat[4*1+3]*y + mat[4*2+3]*z + mat[4*3+3];
    if (s == 0)
        s = 1;

    ret[0] = (mat[4*0+0]*x + mat[4*1+0]*y + mat[4*2+0]*z + mat[4*3+0])/s;
    ret[1] = (mat[4*0+1]*x + mat[4*1+1]*y + mat[4*2+1]*z + mat[4*3+1])/s;
    ret[2] = (mat[4*0+2]*x + mat[4*1+2]*y + mat[4*2+2]*z + mat[4*3+2])/s;
}

struct HAnimHumanoidExtraDataStruct {
     float *origVertices;
     int origVertices_length;
     float thisMatrix[16];
     float *normalData;
};

void applyJoint(X3dNode *data, X3dNode *x3dcoord, float *origVertices, 
                X3dNode *parent)
{
     struct X3dHAnimJoint *joint = (struct X3dHAnimJoint*)data;
     struct X3dCoordinate *coord = x3dcoord;
     int i;
     struct HAnimJointExtraDataStruct *extraVar;
     struct X3dHAnimJoint *mayJoint = (struct X3dHAnimJoint*)parent;
     struct X3dHAnimHumanoid *mayHuman = (struct X3dHAnimHumanoid*)parent;
     float *mayMatrix;

     extraVar = (struct HAnimJointExtraDataStruct *)(joint->extra_data);
     if (extraVar == NULL)
         return;
     if (mayJoint->m_type == X3dHAnimJointType) 
     {
         struct HAnimJointExtraDataStruct *jointData =
             (struct HAnimJointExtraDataStruct *)(mayJoint->extra_data);
         mayMatrix = jointData->jointMatrix;
     } else {
         struct HAnimHumanoidExtraDataStruct *humanData =
             (struct HAnimHumanoidExtraDataStruct *)(mayHuman->extra_data);
         mayMatrix = humanData->thisMatrix;
     }
     glMatrixMode(GL_MODELVIEW);
     glPushMatrix();
     glLoadIdentity();
     glMultMatrixf(mayMatrix);
     glMultMatrixf(extraVar->thisMatrix);
     glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->jointMatrix);
     glPopMatrix();

     if (coord && coord->point)
     {
         int *mskinCoordIndex = joint->skinCoordIndex;
         float *mskinCoordWeight = joint->skinCoordWeight;
         for (i = 0; i < joint->skinCoordIndex_length; i++)
             if (mskinCoordIndex[i] > -1) {
                 int index = mskinCoordIndex[i];
                 float *point = origVertices + index * 3;
                 float wpoint[3];
                 float newpoint[3];
                 int sCwI = joint->skinCoordWeight_length - 1;
                 float weight = mskinCoordWeight[i < sCwI ? i : sCwI];
                 newpoint[0] = coord->point[index * 3];
                 newpoint[1] = coord->point[index * 3 + 1];
                 newpoint[2] = coord->point[index * 3 + 2];
                 multMatrixVec(wpoint, extraVar->jointMatrix, 
                               point[0], point[1], point[2]);
                 wpoint[0] = wpoint[0] * weight; 
                 wpoint[1] = wpoint[1] * weight; 
                 wpoint[2] = wpoint[2] * weight; 
                 newpoint[0] += wpoint[0];
                 newpoint[1] += wpoint[1];
                 newpoint[2] += wpoint[2];
                 coord->point[index * 3 + 0] = newpoint[0];
                 coord->point[index * 3 + 1] = newpoint[1];
                 coord->point[index * 3 + 2] = newpoint[2];
             }
    }
    if (joint->children)
        for (i = 0; i < joint->children_length; i++)
            if (joint->children[i])
            {
                struct X3dHAnimJoint *mayJoint = joint->children[i];
                if (mayJoint->m_type == X3dHAnimJointType)
                    if (coord && coord->point)
                        applyJoint(joint->children[i], x3dcoord, 
                                   origVertices, data);
            }
}

void CRWDHAnimHumanoidTreeRender(X3dNode *data, void *extraData)
{
    struct X3dHAnimHumanoid *humanoid = (struct X3dHAnimHumanoid*)data;
    int i;
    struct HAnimHumanoidExtraDataStruct *extraVar;
    struct X3dCoordinate *coord = humanoid->skinCoord;

    if(initRender)
    {
        humanoid->extra_data = malloc(sizeof(struct 
                                             HAnimHumanoidExtraDataStruct));
    }
    extraVar = (struct HAnimHumanoidExtraDataStruct *)humanoid->extra_data;
    if (extraVar == NULL)
        return;
    if(initRender) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->thisMatrix);
        glPopMatrix();

        if (coord && coord->point) 
        {
            extraVar->origVertices = malloc(coord->point_length * 
                                            sizeof(float));
            for (i = 0; i < coord->point_length; i++)
                extraVar->origVertices[i] = coord->point[i];
            extraVar->origVertices_length = coord->point_length;
        }
    }
    if(initRender)
    {
        if (humanoid->skeleton)
            for (i = 0; i < humanoid->skeleton_length; i++)
                if (humanoid->skeleton[i]) 
                {
                    X3dTreeRenderCallback(humanoid->skeleton[i], extraData);
                }       
        if (humanoid->skin) {
            for (i = 0; i < humanoid->skin_length; i++)
                if (humanoid->skin[i])
                    X3dTreeRenderCallback(humanoid->skin[i], extraData);
            for (i = 0; i < humanoid->skin_length; i++)
                if (humanoid->skin[i]) {
                    struct X3dIndexedFaceSet *geometry = NULL;
                    if (((struct X3dShape *)humanoid->skin[i])->m_type == X3dShapeType) {
                        struct X3dShape *shape = (struct X3dShape *)humanoid->skin[i];
                        if (shape->geometry && ((struct X3dIndexedFaceSet *)shape->geometry)->m_type == X3dIndexedFaceSetType)
                            geometry = (struct X3dIndexedFaceSet *)shape->geometry;
                    }
                    if (geometry)
                        X3dIndexedFaceSetCreateNormalsCallback(geometry, &extraVar->normalData);
                    X3dTreeRenderCallback(humanoid->skin[i], extraVar->normalData);
               }
        }
    }
    else
    {
        struct X3dCoordinate *coord = humanoid->skinCoord;

        if (preRender)
        {
            if (coord == NULL)
                return;
            glPushMatrix();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(humanoid->translation[0], humanoid->translation[1], humanoid->translation[2]);
            glTranslatef(humanoid->center[0], humanoid->center[1], humanoid->center[2]);
            glRotatef( ( (humanoid->rotation[3] / (2*M_PI) ) * 360), humanoid->rotation[0], humanoid->rotation[1], humanoid->rotation[2]);
            glRotatef( ( (humanoid->scaleOrientation[3] / (2*M_PI) ) * 360), humanoid->scaleOrientation[0], humanoid->scaleOrientation[1], humanoid->scaleOrientation[2]);
            glScalef(humanoid->scale[0], humanoid->scale[1], humanoid->scale[2]);
            glRotatef( ( (humanoid->scaleOrientation[3] / (2*M_PI) ) * 360) * -1, humanoid->scaleOrientation[0], humanoid->scaleOrientation[1], humanoid->scaleOrientation[2]);
            glTranslatef(humanoid->center[0] * -1, humanoid->center[1] * -1, humanoid->center[2] * -1);
            glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->thisMatrix);
            for (i = 0; i < coord->point_length; i++)
                coord->point[i] = 0;
             glPopMatrix();
        }
        glPushMatrix();
        glMultMatrixf(extraVar->thisMatrix);
        if (humanoid->skeleton)
            for (i = 0; i < humanoid->skeleton_length; i++)
                if (humanoid->skeleton[i])
                {
                    glPushMatrix();
                    if (preRender) 
                    {
                        struct X3dHAnimJoint *mayJoint = humanoid->skeleton[i];
                        if (mayJoint->m_type == X3dHAnimJointType)
                            if (coord && coord->point)
                                applyJoint(humanoid->skeleton[i], coord, 
                                           extraVar->origVertices, data);
                    }
                    glDisable(GL_TEXTURE_2D);
                    X3dTreeRenderCallback(humanoid->skeleton[i], extraData);
                    glPopMatrix();
                }
        if (humanoid->skin)
            for (i = 0; i < humanoid->skin_length; i++)
                if (humanoid->skin[i]) {
                    struct X3dIndexedFaceSet *geometry = NULL;
                    glDisable(GL_TEXTURE_2D);
                    if (((struct X3dShape *)humanoid->skin[i])->m_type == X3dShapeType) {
                        struct X3dShape *shape = (struct X3dShape *)humanoid->skin[i];
                        if (shape->geometry && ((struct X3dIndexedFaceSet *)shape->geometry)->m_type == X3dIndexedFaceSetType)
                            geometry = (struct X3dIndexedFaceSet *)shape->geometry;
                    }
                    if (geometry)
                        X3dIndexedFaceSetCreateNormalsCallback(geometry, &extraVar->normalData);
                    X3dTreeRenderCallback(humanoid->skin[i], extraVar->normalData);
                }
        glPopMatrix();
    }
}

void CRWDHAnimJointTreeRender(X3dNode *data, void *extraData)
{
    struct X3dHAnimJoint *joint = (struct X3dHAnimJoint*)data;
    struct HAnimJointExtraDataStruct *extraVar;
    int i;

    if(initRender)
    {
        joint->extra_data = malloc(sizeof(struct 
                                          HAnimJointExtraDataStruct));
    }
    extraVar = (struct HAnimJointExtraDataStruct *)joint->extra_data;
    if (extraVar == NULL)
        return;
    if(initRender)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->jointMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->thisMatrix);
        glPopMatrix();
        if (joint->children)
            for (i = 0; i < joint->children_length; i++)
                if (joint->children[i])
                {
                    glDisable(GL_TEXTURE_2D);
                    X3dTreeRenderCallback(joint->children[i], extraData);
                }
    }
    else
    {
        if (preRender)
        {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();
            glTranslatef(joint->translation[0], joint->translation[1], joint->translation[2]);
            glTranslatef(joint->center[0], joint->center[1], joint->center[2]);
            glRotatef( ( (joint->rotation[3] / (2*M_PI) ) * 360), joint->rotation[0], joint->rotation[1], joint->rotation[2]);
            glRotatef( ( (joint->scaleOrientation[3] / (2*M_PI) ) * 360), joint->scaleOrientation[0], joint->scaleOrientation[1], joint->scaleOrientation[2]);
            glScalef(joint->scale[0], joint->scale[1], joint->scale[2]);
            glRotatef( ( (joint->scaleOrientation[3] / (2*M_PI) ) * 360) * -1, joint->scaleOrientation[0], joint->scaleOrientation[1], joint->scaleOrientation[2]);
            glTranslatef(joint->center[0] * -1, joint->center[1] * -1, joint->center[2] * -1);
            glGetFloatv(GL_MODELVIEW_MATRIX, extraVar->thisMatrix);
            glPopMatrix();
        }
        glMultMatrixf(extraVar->thisMatrix);
        if (joint->children)
            for (i = 0; i < joint->children_length; i++)
                if (joint->children[i])
                {
                    glDisable(GL_TEXTURE_2D);
                    X3dTreeRenderCallback(joint->children[i], extraData);
                }
    }
}

static double getTimerTime(void)
{
#ifdef WIN32
    struct _timeb t;
    _ftime(&t);
    return (double) t.time + t.millitm / 1000.0;
#else
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) t.tv_sec + (double) t.tv_usec / 1000000.0;
#endif
}

struct ParticleSystemExtraDataStruct {
    int m_enabled;
    float m_force[3];
    float *m_internPosition; 
    float *m_internVector; 
    double *m_lifeTime;
    double *m_startTime; 
    int     m_particlesDirty;
    double  m_internTime;
    float   m_mass;
};

void startParticle(struct X3dParticleSystem *system, int i, 
                   struct ParticleSystemExtraDataStruct *extraVar)
{
    float angle; 
    struct X3dConeEmitter *mayEmitter;
    float speed = 1;
    float alpha = (random() / (float)RAND_MAX) * 2.0 * M_PI;
    float maxAngle = 2.0 * M_PI;
    int setInternVector = 0;

    extraVar->m_internPosition[3 * i] = 0;
    extraVar->m_internPosition[3 * i + 1] = 0; 
    extraVar->m_internPosition[3 * i + 2] = 0;

    angle = (random() / (float)RAND_MAX) * maxAngle; 
    mayEmitter = (struct X3dConeEmitter *) system->emitter;
    if (system->emitter)
        if (mayEmitter->m_type == X3dConeEmitterType)
        {
            struct X3dConeEmitter *emit = (struct X3dConeEmitter *) 
                                          system->emitter;
            maxAngle = emit->angle;
        }

    extraVar->m_internVector[3 * i] = speed * extraVar->m_force[0] * 
                                      sin(angle) * cos(alpha);
    extraVar->m_internVector[3 * i + 1] = speed * extraVar->m_force[1] * 
                                          cos(angle);
    extraVar->m_internVector[3 * i + 2]= speed * extraVar->m_force[2] * 
                                         sin(angle) * sin(alpha);

    if (system->emitter)
        if (mayEmitter->m_type == X3dConeEmitterType) 
        {
            struct X3dConeEmitter *emit = (struct X3dConeEmitter *)
                                          system->emitter;
            extraVar->m_mass = emit->mass;
            extraVar->m_internPosition[3 * i] = emit->position[0];
            extraVar->m_internPosition[3 * i + 1] = emit->position[1]; 
            extraVar->m_internPosition[3 * i + 2] = emit->position[2];
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
        } 
        else if (mayEmitter->m_type == X3dPointEmitterType) 
        {
            struct X3dPointEmitter *emit = (struct X3dPointEmitter *)
                                           system->emitter;
            extraVar->m_mass = emit->mass;
            extraVar->m_internPosition[i * 3] = emit->position[0];
            extraVar->m_internPosition[i * 3 + 1] = emit->position[1]; 
            extraVar->m_internPosition[i * 3 + 2] = emit->position[2];
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
            if ((emit->direction[0] != 0) &&
                (emit->direction[1] != 0) &&
                (emit->direction[0] != 0)) {
                setInternVector = 1;
                extraVar->m_internVector[i * 3] = speed * 
                                                  emit->direction[0];
                extraVar->m_internVector[i * 3 + 1] = speed * 
                                                      emit->direction[1];
                extraVar->m_internVector[i * 3 + 2] = speed * 
                                                      emit->direction[2];
            }
        }
        else if (mayEmitter->m_type == X3dExplosionEmitterType) 
        {
            struct X3dExplosionEmitter *emit = (struct X3dExplosionEmitter *)
                                               system->emitter;
            extraVar->m_mass = emit->mass;
            extraVar->m_internPosition[3 * i] = emit->position[0];
            extraVar->m_internPosition[3 * i + 1] = emit->position[1]; 
            extraVar->m_internPosition[3 * i + 2] = emit->position[2];
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
        }
        else if (mayEmitter->m_type == X3dPolylineEmitterType) 
        {
            struct X3dPolylineEmitter *emit = (struct X3dPolylineEmitter *)
                                              system->emitter;
            struct X3dCoordinate *ncoord = (struct X3dCoordinate *)emit->coord;
            int numLines = 0;
            float *coords = NULL;
            int *lineIndicesCoord1 = (int *)malloc(sizeof(int));
            int *lineIndicesCoord2 = (int *)malloc(sizeof(int));

            extraVar->m_mass = emit->mass;
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
            if (ncoord) 
            {
                int j;
                int startLine = 1;
                int validLine = 0;
                coords = ncoord->point;
                for (j = 0; j < emit->coordIndex_length; j++) {
                    if (emit->coordIndex[j] < 0) 
                    {
                        startLine = 1;
                        validLine = 0;
                    } else if (!validLine)
                        validLine = 1;  
                    else if (emit->coordIndex[j] != emit->coordIndex[j - 1]) 
                    {
                         numLines++;
                         lineIndicesCoord1 = (int *)realloc(lineIndicesCoord1,
                                                            numLines *
                                                            sizeof(int));
                         lineIndicesCoord2 = (int *)realloc(lineIndicesCoord2,
                                                            numLines *
                                                            sizeof(int));
                         lineIndicesCoord1[numLines - 1] = emit->coordIndex[j];
                         lineIndicesCoord2[numLines - 1] = emit->coordIndex[j - 
                                                                            1];
                    }
                }
            }
            if (coords && (numLines > 0))
            {
                int numLine = (random() / (float)RAND_MAX) * numLines;
                int coord1 = lineIndicesCoord1[numLine];              
                int coord2 = lineIndicesCoord2[numLine];
                if (numLine == numLines)
                    numLine--;
                if ((coord1 * 3 < ncoord->point_length) && 
                    (coord2 * 3 < ncoord->point_length))
                {
                    float point1x = coords[coord1 * 3];              
                    float point1y = coords[coord1 * 3 + 1];              
                    float point1z = coords[coord1 * 3 + 2];              
                    float point2x = coords[coord2 * 3];              
                    float point2y = coords[coord2 * 3 + 1];              
                    float point2z = coords[coord2 * 3 + 2];              
                    float vecx = point1x - point2x;
                    float vecy = point1y - point2y;
                    float vecz = point1z - point2z;
                    float ran = (random() / (float)RAND_MAX);
                    float startPointx = point2x + vecx * ran;
                    float startPointy = point2y + vecy * ran;
                    float startPointz = point2z + vecz * ran;
                    extraVar->m_internPosition[i * 3    ] = startPointx;
                    extraVar->m_internPosition[i * 3 + 1] = startPointy;
                    extraVar->m_internPosition[i * 3 + 2] = startPointz;
                } 
                else 
                {
                    extraVar->m_internPosition[i * 3    ] = 0;
                    extraVar->m_internPosition[i * 3 + 1] = 0;
                    extraVar->m_internPosition[i * 3 + 2] = 0;
                }
            } 
            else 
            {
                extraVar->m_internPosition[i * 3    ] = 0;
                extraVar->m_internPosition[i * 3 + 1] = 0;
                extraVar->m_internPosition[i * 3 + 2] = 0;
            }
            free(lineIndicesCoord1);
            free(lineIndicesCoord2);
            if ((emit->direction[0] != 0) &&
                (emit->direction[1] != 0) &&
                (emit->direction[0] != 0))
            {
                setInternVector = 1;
                extraVar->m_internVector[3 * i] = speed * 
                                                  emit->direction[0];
                extraVar->m_internVector[3 * i + 1] = speed * 
                                                      emit->direction[1];
                extraVar->m_internVector[3 * i + 1] = speed * 
                                                      emit->direction[2];
            }
        }
        else if (mayEmitter->m_type == X3dVolumeEmitterType) 
        {
            struct X3dVolumeEmitter *emit = (struct X3dVolumeEmitter *)
                                            system->emitter;
            struct X3dCoordinate *ncoord = (struct X3dCoordinate *)emit->coord;
            int numPoints = 0;
            float *coords = NULL;
            float *coordArray = (float *)malloc(sizeof(float));

            extraVar->m_mass = emit->mass;
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
            if (ncoord) {
                int j;
                coords = ncoord->point;
                for (j = 0; j < emit->coordIndex_length; j++)
                {
                    int index = emit->coordIndex[j];
                    if (index > -1)
                    {
                        numPoints++;
                        coordArray = (float *)realloc(coordArray, 
                                                      numPoints * 3 *
                                                      sizeof(float));
                        coordArray[(numPoints - 1) * 3    ] = 
                            coords[index * 3    ];
                        coordArray[(numPoints - 1) * 3 + 1] = 
                            coords[index * 3 + 1];
                        coordArray[(numPoints - 1) * 3 + 2] = 
                            coords[index * 3 + 2];
                    }
                }
            }
            if (coords && (numPoints > 0)) {
                int numPoint = (random() / (float)RAND_MAX) * numPoints;
                extraVar->m_internPosition[i * 3    ] = 
                    coordArray[numPoint * 3    ];
                extraVar->m_internPosition[i * 3 + 1] = 
                    coordArray[numPoint * 3 + 1];
                extraVar->m_internPosition[i * 3 + 2] =
                    coordArray[numPoint * 3 + 2]; 
            }
            free(coordArray);
        }
        else if (mayEmitter->m_type == X3dSurfaceEmitterType) 
        {
            static int once = 0;
            struct X3dSurfaceEmitter *emit = (struct X3dSurfaceEmitter *)
                                             system->emitter;
            extraVar->m_mass = emit->mass;
            speed = (random() / (float)RAND_MAX) * 
                    (emit->variation / 2.0f + 1) * emit->speed;
            if (!once) 
            {
                fprintf(stderr, "Sorry, C/C++/java export of SurfaceEmitter is not supported\n");
                once = 1;
            }
            extraVar->m_internPosition[i * 3    ] = 0;
            extraVar->m_internPosition[i * 3 + 1] = 0;
            extraVar->m_internPosition[i * 3 + 2] = 0;
        }

    if (!setInternVector)
    {
        extraVar->m_internVector[3 * i] = speed * extraVar->m_force[1] * 
                                          cos(angle) * cos(alpha);
        extraVar->m_internVector[3 * i + 1] = speed * extraVar->m_force[2] * 
                                              sin(angle);
        extraVar->m_internVector[3 * i + 2] = speed * extraVar->m_force[3] * 
                                              cos(angle) * sin(alpha);
    }
    extraVar->m_lifeTime[i] = (random() / (float)RAND_MAX) * 
                              (system->lifetimeVariation / 2.0f + 1) * 
                              system->particleLifetime;
    extraVar->m_startTime[i] = getTimerTime();
}

void CRWDParticleSystemTreeRender(X3dNode *data, void *dataptr)
{
    struct X3dParticleSystem *system = (struct X3dParticleSystem*)data;
    struct ParticleSystemExtraDataStruct *extraVar;
    int i;

    if(initRender)
    {
        system->extra_data = malloc(sizeof(struct 
                                           ParticleSystemExtraDataStruct));
        if (system->appearance)
            X3dTreeRenderCallback(system->appearance, NULL);
        if (system->geometry)
            X3dTreeRenderCallback(system->geometry, NULL);
    }
    extraVar = (struct ParticleSystemExtraDataStruct *)system->extra_data;
    if (extraVar == NULL)
        return;
    if(initRender)
    {
         extraVar->m_enabled = 1;
         if (strcmp(system->geometryType, "GEOMETRY") != 0) 
         {
             extraVar->m_enabled = 0;
             fprintf(stderr, "%s%s\n", 
                     "Warning: only ParticleSystem.geometryType ",
                     "== GEOMETRY are rendered\n");
         } else {
             int numParticles = system->maxParticles;
             extraVar->m_internPosition = (float *)malloc(numParticles * 
                                               3 * sizeof(float)); 
             extraVar->m_internVector = (float *)malloc(numParticles * 
                                                        3 * sizeof(float));
             extraVar->m_lifeTime = (double *)malloc(numParticles * 
                                                     sizeof(double));
             extraVar->m_startTime = (double *)malloc(numParticles * 
                                                      sizeof(double));; 
             for (i = 0; i < numParticles; i++)
                 startParticle(system, i, extraVar);                      
         }
         extraVar->m_particlesDirty = 1;
         extraVar->m_internTime = 0;
         extraVar->m_mass = 0;

         extraVar->m_force[0] = 0;
         extraVar->m_force[1] = 0;
         extraVar->m_force[2] = 0;
    }
    else if (!preRender)
    {
        if (extraVar->m_enabled) 
        {
            int numberParticles;
            double t;
            double delta;
            int i,j;

            for (j = 0; j < 3; j ++)
                extraVar->m_force[j] += 0;
    
            for (i = 0; i < system->physics_length; i++)
            {
                struct X3dForcePhysicsModel *mayForce = 
                    (struct X3dForcePhysicsModel *)system->physics[i];

                if (system->physics[i]) 
                {
                    X3dNode *phys = system->physics[i];
                    if (mayForce->m_type == X3dForcePhysicsModelType) 
                    {
                        struct X3dForcePhysicsModel *model = 
                            (struct X3dForcePhysicsModel *) phys;
                        const float *forceForce = model->force;
                        for (j = 0; j < 3; j ++)
                            extraVar->m_force[j] += forceForce[j];
                    } else if (mayForce->m_type == X3dWindPhysicsModelType) {
                        struct X3dWindPhysicsModel *model = 
                            (struct X3dWindPhysicsModel *) phys;
                        for (j = 0; j < 3; j++)
                            extraVar->m_force[j] += model->direction[j] *
                                                    pow(10, 2.0f * 
                                                            log(model->speed) * 
                                                            0.64615f);
                    }
                }
            }
            numberParticles = system->maxParticles;
        
            t = getTimerTime();
            delta = 0;
            if (extraVar->m_internTime == 0)
                delta = 0;
            else
               delta = t - extraVar->m_internTime;
            for (i = 0; i < numberParticles; i++) 
            {
                int j;
                float mass;
                if ((t - extraVar->m_startTime[i]) > extraVar->m_lifeTime[i])
                    startParticle(system, i, extraVar);
                else  
                    for (j = 0; j < 3; j++)
                        extraVar->m_internVector[i * 3 + j] += 
                            delta * extraVar->m_force[j];
                mass = extraVar->m_mass;
                if (mass == 0.0f)
                    mass = 1.0f;
                for (j = 0; j < 3; j++)
                    extraVar->m_internPosition[3 * i + j] += 
                        mass * extraVar->m_internVector[3 * i + j] * delta;
        
                glPushMatrix();
                glTranslatef(extraVar->m_internPosition[3 * i],
                             extraVar->m_internPosition[3 * i + 1],
                             extraVar->m_internPosition[3 * i + 2]);
                if (system->appearance)
                    X3dTreeRenderCallback(system->appearance, NULL);
                if (system->geometry)
                    X3dTreeRenderCallback(system->geometry, NULL);
                glPopMatrix();
            }
            extraVar->m_internTime = getTimerTime();

        }
    }
}

struct CRWDTimeSensorData {
    double startTime;
    double stopTime;
};

int CRWDTimeSensorSendEvents(X3dNode *data, const char *event, void* extraData)
{
    static double startTime;
    struct X3dTimeSensor *timeSensor = (struct X3dTimeSensor*)data;
    double currentTime = 0;
    struct CRWDTimeSensorData *dataPtr;
    double callTime;
    if (timeSensor->m_data == NULL) {
        timeSensor->m_data = malloc(sizeof(struct CRWDTimeSensorData));
        timeSensor->isActive = 0;     
        dataPtr = (struct CRWDTimeSensorData *)timeSensor->m_data;
        dataPtr->startTime = timeSensor->startTime;
        dataPtr->stopTime = timeSensor->stopTime;
    }

    dataPtr = (struct CRWDTimeSensorData *)timeSensor->m_data;

    if (!timeSensor->enabled)
        return 0;

    currentTime = getTimerTime();

    if (timeSensor->isActive) {
        timeSensor->time = currentTime;
    } else {
        if (timeSensor->startTime > dataPtr->stopTime)
            dataPtr->startTime = timeSensor->startTime;
    }
    if (!timeSensor->loop) { 
        if (dataPtr->stopTime > dataPtr->startTime) {
            if (timeSensor->isActive) {
                dataPtr->stopTime = currentTime;
                timeSensor->isActive = 1;
                return 1;  
            }
            return 0;
        }
        if (currentTime > (dataPtr->startTime + 
                           timeSensor->cycleInterval)) {
            timeSensor->fraction_changed = 1.0f;
            if (timeSensor->isActive) {
                dataPtr->stopTime = currentTime;
                timeSensor->isActive = 0;
                return 1;
            }
            return 0;
        }
        if (currentTime < timeSensor->startTime)
            return 0;
    }

    timeSensor->isActive = 1;
                
    callTime = currentTime - dataPtr->startTime;
    timeSensor->fraction_changed = (float)(fmod(callTime, 
                                                timeSensor->cycleInterval) / 
                                           timeSensor->cycleInterval);
    return 1;
}

static float interpolate(float t, float key, float oldKey, 
                         float value, float oldValue)
{
    float delta = (key == oldKey) ? 0 : (t - oldKey) / (key - oldKey);
    return oldValue + delta * (value - oldValue);
}

static void accountInterpolator(float *target, float fraction,
                                float *keys, float *keyValues, int numKeys,
                                int stride, int rotation)
{
    int i;
    int j;

    if (!rotation) {
        if (numKeys > 0) {
            int oldi = 0;
            for (j = 0; j < stride; j++)
                target[j]=keyValues[j];
            for (i = 0; i < numKeys; i++) {
                if ((fraction >= keys[oldi]) && (fraction < keys[i])) {
                    for (j = 0; j < stride; j++)
                        target[j] = interpolate(fraction, keys[i], keys[oldi], 
                                                keyValues[i * stride + j], 
                                                keyValues[oldi * stride + j]); 
                    break;
                }
                oldi = i;
            }
            if (i == numKeys)
                for (j = 0; j < stride; j++)
                    target[j] = keyValues[(numKeys - 1) * stride + j];
        }
    } else {
        float k1 = 0.0;
        float k2 = 1.0;
        int pos1 = 0;
        int pos2 = 0;
        int pos;
        float s;
        float q[4];
        float q1[4];
        float q2[4];
        int flip;
        float alpha;
        float beta;
        float theta;
        float sin_t;
        float cos_t;
        float len;

        if (numKeys == 0) {
            target[0] = 0;
            target[1] = 0;
            target[2] = 1;
            target[3] = 0;
            return;
        }

        for (pos = 0; pos < numKeys; pos++)
            if (keys[pos] > fraction) 
                break;

        if (pos == 0) {
            k1 = 0.0f;
            pos1 = 0;
        } else {
            k1 = keys[pos - 1];
            pos1 = pos - 1;
        }

        if (pos == numKeys) {
            k2 = 1.0f;
            pos2 = numKeys - 1;
        } else {
           k2 = keys[pos];
           pos2 = pos;
        }

        if ((k1 == k2) || (pos1==pos2))
            alpha = 0.0f;
        else
            alpha = (fraction - k1) / (k2 - k1);

        s = sin(keyValues[pos1 * 4 + 3] * 0.5f);
        for (i = 0; i < 3; i++)
             q1[i] = keyValues[pos1 * 4 + i] * s;
        q1[3] = cos(keyValues[pos1 * 4 + 3] * 0.5f);

        s = sin(keyValues[pos2 * 4 + 3] * 0.5f);
        for (i = 0; i < 3; i++)
             q2[i] = keyValues[pos2 * 4 + i] * s;
        q2[3] = cos(keyValues[pos2 * 4 + 3] * 0.5f);
        
        cos_t = q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3];

        len = (float) sqrt(cos_t);
        if (len > 0)
            len = 1 / len;
        else
            len = 1;

        if (cos_t < 0.0) {
            cos_t = -cos_t;
            flip = 1;
        } else
            flip = 0;
        if (1.0 - cos_t < EPSILON)
            beta = 1.0f - alpha;
        else {
            theta = (float) acos(cos_t);
            sin_t = (float) sin(theta);
            beta = (float) sin(theta - alpha * theta) / sin_t;
            alpha = (float) sin(alpha * theta) / sin_t;
        }
        if (flip)
            alpha = -alpha;
        for (i = 0; i < 4; i++)
            q[i] = (q1[i] * beta + q2[i] * alpha);
        for (i = 0; i < 3; i++)
            target[i] = q[i] * len;
        target[3] = 2.0f * (float) acos(q[3]);
    }
}

int CRWDPositionInterpolatorSendEvents(X3dNode *data, const char *event,
                                       void* extraData)
{
    static int warned = 0;
    struct X3dPositionInterpolator *interpolator = 
           (struct X3dPositionInterpolator*)data;
    if (interpolator->key_length * 3 != interpolator->keyValue_length) { 
        if (!warned)
            error("invalid PositionInterpolator: number keys do not match number values\n");
        warned = 1;
        return 0;
    }
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 3, 0); 
    return 1;
}

int CRWDOrientationInterpolatorSendEvents(X3dNode *data, const char *event,
                                           void* extraData)
{
    static int warned = 0;
    struct X3dOrientationInterpolator *interpolator = 
           (struct X3dOrientationInterpolator*)data;
    if (interpolator->key_length * 4 != interpolator->keyValue_length) { 
        if (!warned)
            error("invalid OrientationInterpolator: number keys do not match number values\n");
        warned = 1;
        return 0;
    }
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 4, 1); 
    return 1;
}

int CRWDColorInterpolatorSendEvents(X3dNode *data,  const char *event,
                                     void* extraData)
{
    static int warned = 0;
    struct X3dColorInterpolator *interpolator = 
           (struct X3dColorInterpolator*)data;
    if (interpolator->key_length * 3 != interpolator->keyValue_length) { 
        if (!warned)
            error("invalid ColorInterpolator: number keys do not match number values\n");
        warned = 1;
        return 0;
    }
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 3, 0); 
    return 1;
}

int CRWDScalarInterpolatorSendEvents(X3dNode *data, const char *event,
                                      void* extraData)
{
    static int warned = 0;
    struct X3dScalarInterpolator *interpolator = 
           (struct X3dScalarInterpolator*)data;
    if (interpolator->key_length * 1 != interpolator->keyValue_length) { 
        if (!warned)
            error("invalid ScalarInterpolator: number keys do not match number values\n");
        warned = 1;
        return 0;
    }
    accountInterpolator(&interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 1, 0); 
    return 1;
}

int CRWDCoordinateInterpolatorSendEvents(X3dNode *data, const char *event,
                                          void* extraData)
{
    struct X3dCoordinateInterpolator *interpolator = 
           (struct X3dCoordinateInterpolator*)data;
    int targetSize = interpolator->keyValue_length / interpolator->key_length;
    if (interpolator->value_changed == NULL) {
        if (interpolator->key_length * targetSize != 
            interpolator->keyValue_length){
            error("invalid CoodinateInterpolator: number keys do not match number values\n");
            return 0;
        }
        interpolator->value_changed_length = targetSize;
        interpolator->value_changed = malloc(targetSize == 0 ? 1 :
                                             targetSize * sizeof(float) * 3);
    }
        
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 
                        interpolator->value_changed_length, 0);
    return 1;
}

int CRWDNormalInterpolatorSendEvents(X3dNode *data, const char *event,
                                      void* extraData)
{
    int i,j;
    struct X3dNormalInterpolator *interpolator = 
           (struct X3dNormalInterpolator*)data;
    int targetSize = interpolator->keyValue_length / interpolator->key_length;
    if (interpolator->value_changed == NULL) {
        if (interpolator->key_length * targetSize != 
            interpolator->keyValue_length){
            error("invalid CoodinateInterpolator: number keys do not match number values\n");
            return 0;
        }
        interpolator->value_changed_length = targetSize;
        interpolator->value_changed = malloc(targetSize == 0 ? 1 :
                                             targetSize * sizeof(float) * 3);
    }
        
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 
                        interpolator->value_changed_length, 0);

    /* normalize results */
    for (i = 0; i < targetSize; i += 3) {
        float *vec = interpolator->keyValue + 3 * i;
        float len = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
        if (len != 0)
            for (j = 0; j < 3; j++)
                vec[j] /= len;
    }   
    return 1;
}

int CRWDPositionInterpolator2DSendEvents(X3dNode *data, const char *event,
                                          void* extraData)
{
    static int warned = 0;
    struct X3dPositionInterpolator2D *interpolator = 
           (struct X3dPositionInterpolator2D*)data;
    if (interpolator->key_length * 2 != interpolator->keyValue_length) { 
        if (!warned)
            error("invalid PositionInterpolator: number keys do not match number values\n");
        warned = 1;
        return 0;
    }
    accountInterpolator(interpolator->value_changed, interpolator->set_fraction,
                        interpolator->key, interpolator->keyValue, 
                        interpolator->key_length, 2, 0); 
    return 1;
}

X3dNode *rootNode = &scenegraph.root;
/*X3dNode *rootNode = &scenegraph.DEFNAME;*/

void CRWDdraw(float *matrix)
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixf(matrix);

    viewpointRendered = 0;
    
    preRender = -1;

    X3dTreeRenderCallback(rootNode, NULL);

    if(!lightExists)
    {
        static int light;
        if (numLights == 0)
            light = allocLight();
        glEnable(light);
    } 
    preRender = 0;

    X3dTreeRenderCallback(rootNode, NULL);
}

void CRWDprocessEvents() {
    X3dProcessEvents(&scenegraph, NULL);
}

void CRWDinit()
{
    GLint zero[4] = {0, 0, 0, 0};

    X3dIndexedFaceSetRenderCallback = CRWDIndexedFaceSetRender;
    X3dIndexedFaceSetCreateNormalsCallback = CRWDIndexedFaceSetCreateNormals;
    X3dTextRenderCallback = CRWDTextRender;
    X3dPointLightRenderCallback = CRWDPointLightRender;
    X3dDirectionalLightRenderCallback = CRWDDirectionalLightRender;
    X3dSpotLightRenderCallback = CRWDSpotLightRender;
    X3dMaterialRenderCallback = CRWDMaterialRender;
    X3dTransformTreeRenderCallback = CRWDTransformTreeRender;
    X3dGroupTreeRenderCallback = CRWDGroupTreeRender;
    X3dViewpointRenderCallback = CRWDViewpointRender;
    X3dImageTextureRenderCallback = CRWDImageTextureRender;
    X3dPixelTextureRenderCallback = CRWDPixelTextureRender;
    X3dSwitchTreeRenderCallback = CRWDSwitchTreeRender;
    X3dHAnimHumanoidTreeRenderCallback = CRWDHAnimHumanoidTreeRender;
    X3dHAnimJointTreeRenderCallback = CRWDHAnimJointTreeRender;
    X3dParticleSystemTreeRenderCallback = CRWDParticleSystemTreeRender;
    X3dTimeSensorProcessEventCallback = CRWDTimeSensorSendEvents;
    X3dPositionInterpolatorProcessEventCallback = CRWDPositionInterpolatorSendEvents;
    X3dOrientationInterpolatorProcessEventCallback = CRWDOrientationInterpolatorSendEvents;
    X3dColorInterpolatorProcessEventCallback = CRWDColorInterpolatorSendEvents;
    X3dScalarInterpolatorProcessEventCallback = CRWDScalarInterpolatorSendEvents;
    X3dCoordinateInterpolatorProcessEventCallback = CRWDCoordinateInterpolatorSendEvents;
    X3dNormalInterpolatorProcessEventCallback = CRWDNormalInterpolatorSendEvents;
    X3dPositionInterpolator2DProcessEventCallback = CRWDPositionInterpolator2DSendEvents;
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_EMISSION);
    glColorMaterial(GL_FRONT_AND_BACK, GL_SPECULAR);
    glColorMaterial(GL_FRONT_AND_BACK, GL_SHININESS);

    /*Enable light*/
    glEnable(GL_LIGHTING);

    glLightModeliv(GL_LIGHT_MODEL_AMBIENT, zero);

    /* Use depth buffering for hidden surface elimination. */

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glAlphaFunc(GL_NOTEQUAL, 0);

    glShadeModel(GL_SMOOTH);

    X3dSceneGraphInit(&scenegraph);

    preRender = 0;
    initRender = -1;

    X3dTreeRenderCallback(rootNode, NULL);

    if(!viewPointExists)
    {
        struct X3dViewpoint viewpoint;
        viewpoint.fieldOfView = 0.785398;
        viewpoint.position = malloc(3 * sizeof(float));
        viewpoint.position[0] = 0;
        viewpoint.position[1] = 0;
        viewpoint.position[2] = 10;
        viewpoint.orientation = malloc(4 * sizeof(float));
        viewpoint.orientation[0] = 0;
        viewpoint.orientation[1] = 0;
        viewpoint.orientation[2] = 1;
        viewpoint.orientation[3] = 0;
        X3dViewpointRenderCallback(&viewpoint, NULL);
        free(viewpoint.position);
        viewpoint.position = NULL;
        free(viewpoint.orientation);
        viewpoint.orientation = NULL;
    }

    initRender = 0;
}

void CRWDfinalize()
{
}

