#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define __GE_MESH_MODE_POINT 0                  //0000 0000
#define __GE_MESH_MODE_LINE 1                   //0000 0001
#define __GE_MESH_MODE_TRIANGLE 4               //0000 0002
#define __GE_MESH_TEXTURE_ENABLE 2147483648     //8000 0000
#define __GE_MESH_TEXTURE_DISABLE 0             //0000 0000
#define __GE_MESH_PRECISION 2147483648          //8000 0000
                                                //2 ^ (8 * sizeof(unsigned int))
#define __GE_MESH_HEADER_SIZE 6                 //size of the header in the file

class geBufferf {
public:
    float* buf;
    geBufferf() {}
};

class geBufferui {
public:
    unsigned int* buf;
    geBufferui() {}
};

//read -----------------------------------------

char* readfile(const char *file) {
    FILE *fptr;
    long length;
    char *buf;


    fptr = fopen(file, "rb");
    if (!fptr) {
        return NULL;
    }
    
    fseek(fptr, 0, SEEK_END); 
    length = ftell(fptr);

    buf = (char*)malloc(length+1);
    
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    
    fclose(fptr);
    
    buf[length] = 0;
    return buf;
}

unsigned int *gereadmeshfile(const char *file, geBufferf* vbuffer, geBufferf* cbuffer) {
    //variables
    FILE *fptr;
    unsigned int *buf;
    float *bufout;
    long flength;
    long length;
    unsigned int *args;
    args = (unsigned int*)malloc(6 * sizeof(unsigned int));

    //header variables
    unsigned int hlength = 6;
    unsigned int mode;
    unsigned int vlen;
    unsigned int clen;
    unsigned int vdim;
    unsigned int cdim;
    unsigned int *hbuf;

    //open file
    fptr = fopen(file, "rb");
    if (!fptr) {
        return NULL;
    }
    fseek(fptr, 0, SEEK_END); 
    flength = ftell(fptr);

    //read header
    fseek(fptr, 0, SEEK_SET);
    hbuf = (unsigned int*)malloc(hlength * sizeof(unsigned int));
    fread(hbuf, sizeof(unsigned int), hlength, fptr);
    hlength = hbuf[0];
    mode = hbuf[1];
    vlen = hbuf[2];
    vdim = hbuf[3];
    clen = hbuf[4];
    cdim = hbuf[5];
    length = (vlen * vdim) + (clen * cdim);

    //write header info to args buffer
    for(int i = 0; i < hlength; i++) {
        args[i] = hbuf[i];
    }

    //read actual data from file
    buf = (unsigned int*)malloc(length * sizeof(unsigned int));
    fread(buf, sizeof(unsigned int), length, fptr);
    
    //convert data from uint to float and split the data
    vbuffer->buf = (float*)malloc(vlen * vdim * sizeof(float));
    cbuffer->buf = (float*)malloc(clen * cdim * sizeof(float));
    for(int i = 0; i < vlen * vdim; i++) {
        vbuffer->buf[i] = ((((float)buf[i]) / __GE_MESH_PRECISION) * 2) - 1;
    }
    for(int i = 0; i < clen * cdim; i++) {
        cbuffer->buf[i] = (((float)buf[i + (vlen * vdim)]) / __GE_MESH_PRECISION);
    }
    
    //close file and return buffer
    fclose(fptr);
    return args;
}

//write ----------------------------------------

bool gewritemeshfile(const char* file, const char* texturepath, unsigned int vertexmode,
    unsigned int drawmode, unsigned int vlen, unsigned int vdim, 
    unsigned int clen, unsigned int cdim, geBufferf vbufferin,
    geBufferf cbufferin, geBufferf indexbufferin) {

    //variables
    geBufferf bufferout;
    unsigned int fulllength = 0;
    unsigned int indexbuffersize = 0;
    unsigned int vertexbuffersize = 0;
    unsigned int texturebuffersize = 0;
    bool textureenabled = false;
    bool indexbufferenabled = false;

    printf("Writing file... \n");
    printf("\tCollecting data... ");

    //math
    if(vertexmode >= __GE_MESH_TEXTURE_ENABLE) {
        textureenabled = true;
    }
    if (textureenabled) {
        indexbuffersize = vertexmode - __GE_MESH_TEXTURE_ENABLE;
        if (vertexmode != __GE_MESH_TEXTURE_ENABLE)
        {
            indexbufferenabled = true;
        }
    } else {
        indexbuffersize = vertexmode - __GE_MESH_TEXTURE_DISABLE;
        if (vertexmode != __GE_MESH_TEXTURE_DISABLE)
        {
            indexbufferenabled = true;
        }
    }
    vertexbuffersize = (vlen * vdim);
    texturebuffersize = (clen * cdim);
    if(textureenabled) {
        texturebuffersize = texturebuffersize * 4;
    }
    fulllength = vertexbuffersize + texturebuffersize + indexbuffersize;

    //allocate buffer
    bufferout.buf = (float*)malloc(sizeof(float) * fulllength);

    //do math
    for(int i = 0; i < indexbuffersize; i++) {
        bufferout.buf[i] = indexbufferin.buf[i];
    }
    for(int i = 0; i < vertexbuffersize; i++) {
        bufferout.buf[i + indexbuffersize] = vbufferin.buf[i];
    }
    for(int i = 0; i < texturebuffersize; i++) {
        bufferout.buf[i + indexbuffersize + vertexbuffersize] = cbufferin.buf[i];
    }

    printf("done! \n");

    // ===================================== WRITE ===============================================

    printf("\tConverting data... ");

    //variables
    geBufferui header;
    geBufferui fulldata;
    geBufferui filebuffer;

    //write header buffer
    header.buf = (unsigned int*)malloc(sizeof(unsigned int) * __GE_MESH_HEADER_SIZE);
    header.buf[0] = vertexmode;
    header.buf[1] = drawmode;
    header.buf[2] = vlen;
    header.buf[3] = vdim;
    header.buf[4] = clen;
    header.buf[5] = cdim;

    //convert data from float to unsigned int
    for(int i = 0; i < fulllength; i++) {
        fulldata.buf[i] = (unsigned int)floor(
                ((bufferout.buf[i] + 1) / 2 ) * __GE_MESH_PRECISION);
    }

    //merge header and data
    for(int i = 0; i < __GE_MESH_HEADER_SIZE; i++) {
        filebuffer.buf[i] = header.buf[i];
    }
    for(int i = 0; i < fulllength; i++) {
        filebuffer.buf[i + __GE_MESH_HEADER_SIZE] = fulldata.buf[i];
    }

    printf("done! \n");

    // ===================================== WRITE TO FILE ========================================

    printf("\tWriting to file: \"%s\" ... ", file);

    //variables
    FILE *fptr;

    //open file
    fptr = fopen(file, "wb");
    if(fptr == NULL) {
        return false;
    }

    //write to file
    fseek(fptr, 0, SEEK_SET);
    fwrite(filebuffer.buf, sizeof(unsigned int), fulllength + __GE_MESH_HEADER_SIZE, fptr);

    //close file  
    fclose(fptr);

    printf("done! \n");

    printf("\tSuccess!\n");

    return true;
}