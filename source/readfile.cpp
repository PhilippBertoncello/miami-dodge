#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define __GE_MESH_MODE_POINT 0                  //0000 0000
#define __GE_MESH_MODE_LINE 1                   //0000 0001
#define __GE_MESH_MODE_TRIANGLE 4               //0000 0002
#define __GE_MESH_TEXTURE_ENABLE 2147483648     //8000 0000
#define __GE_MESH_TEXTURE_DISABLE 0             //0000 0000
#define __GE_MESH_FLOAT_RANGE 2                 //float range from -2 to +2
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

int gereadmeshfile(const char* filepath, unsigned int& vmode, unsigned int& cmode,
    unsigned int& vlen, unsigned int& vdim, unsigned int& clen, unsigned int& cdim, 
    geBufferui* indexbuffer, geBufferf* vertexdata, geBufferf* colordata) {

    //create variables
    FILE* fptr;
    geBufferui filebuf;
    geBufferui headbuf;
    geBufferf floatbuf;
    unsigned int totallength;
    unsigned int indexbuffersize;
    unsigned int vertexbuffersize;
    unsigned int colorbuffersize;
    bool textureenabled;
    bool indexenabled;

    //open file
    printf("Loading file: \"%s\"... ", filepath);
    fptr = fopen(filepath, "rb");
    if(fptr == NULL) {
        printf("Fail!\n");
        printf("File couldn't be opened!\n");
        return 1;
    }

    //allocate header buffer and read header
    headbuf.buf = (unsigned int*)malloc(sizeof(unsigned int) * __GE_MESH_HEADER_SIZE);
    fseek(fptr, 0, SEEK_SET);
    fread(headbuf.buf, sizeof(unsigned int), __GE_MESH_HEADER_SIZE, fptr);
    vmode = headbuf.buf[0];
    cmode = headbuf.buf[1];
    vlen = headbuf.buf[2];
    vdim = headbuf.buf[3];
    clen = headbuf.buf[4];
    cdim = headbuf.buf[5];
    vertexbuffersize = vlen * vdim;
    colorbuffersize = clen * cdim;

    //get file length and compare
    if(vmode >= __GE_MESH_TEXTURE_ENABLE) {
        textureenabled = true;
    } if (textureenabled) {
        indexbuffersize = vmode - __GE_MESH_TEXTURE_ENABLE;
    } else {
        indexbuffersize = vmode - __GE_MESH_TEXTURE_DISABLE;
    } if(indexbuffersize != 0) {
        indexenabled = true;
    } 
    totallength = (indexbuffersize) + (vertexbuffersize) + (colorbuffersize);
    fseek(fptr, 0, SEEK_END);
    // if((long unsigned int)ftell(fptr) == (long unsigned int)((long unsigned int)totallength + (long unsigned int)__GE_MESH_HEADER_SIZE)) {
    //     printf("Fail! File is corrupt!\n");
    //     printf("File pointer: %d; %d\n", (long unsigned int)(ftell(fptr) / sizeof(unsigned int)), (long unsigned int)(totallength + __GE_MESH_HEADER_SIZE));
    //     //return 2;
    // }

    //read data
    fseek(fptr, 0, SEEK_SET);
    filebuf.buf = (unsigned int*)malloc(sizeof(unsigned int) * totallength);
    fread(filebuf.buf, sizeof(unsigned int), totallength, fptr);

    //convert to float and split data up
    floatbuf.buf = (float*)malloc(sizeof(float) * totallength);
    indexbuffer->buf = (unsigned int*)malloc(sizeof(unsigned int) * indexbuffersize);
    vertexdata->buf = (float*)malloc(sizeof(float) * vertexbuffersize);
    colordata->buf = (float*)malloc(sizeof(float) * colorbuffersize);
    for(int i = 0; i < totallength; i++) {
        floatbuf.buf[i] = (((float)filebuf.buf[i] / __GE_MESH_PRECISION) * 2)
            - __GE_MESH_FLOAT_RANGE;
    } for (int i = 0; i < indexbuffersize; i++) {
        indexbuffer->buf[i] = filebuf.buf[i];
    } for (int i = 0; i < vertexbuffersize; i++) {
        vertexdata->buf[i] = floatbuf.buf[i + indexbuffersize];
    } for (int i = 0; i < colorbuffersize; i++) {
        colordata->buf[i] = floatbuf.buf[i + indexbuffersize + vertexbuffersize];
    }

    //close file and return
    fclose(fptr);
    printf("Done!\n");
    printf("\tFile read correctly: \"%s\"\n", filepath);
    return 0;
}


//write ----------------------------------------

bool gewritemeshfile(const char* file, unsigned int vertexmode,
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
    } if (textureenabled) {
        indexbuffersize = vertexmode - __GE_MESH_TEXTURE_ENABLE;
    } else {
        indexbuffersize = vertexmode - __GE_MESH_TEXTURE_DISABLE;
    } if(indexbuffersize != 0) {
        indexbufferenabled = true;
    }
    vertexbuffersize = (vlen * vdim);
    texturebuffersize = (clen * cdim);
    fulllength = indexbuffersize + vertexbuffersize + texturebuffersize;

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
    fulldata.buf = (unsigned int*)malloc(sizeof(unsigned int) * fulllength);
    filebuffer.buf = (unsigned int*)malloc(sizeof(unsigned int) * (fulllength + __GE_MESH_HEADER_SIZE));

    //convert data from float to unsigned int
    for(int i = 0; i < indexbuffersize; i++) {
        fulldata.buf[i] = (unsigned int)floor(
                ((bufferout.buf[i] + __GE_MESH_FLOAT_RANGE) / 2 ) * __GE_MESH_PRECISION);
    } for(int i = indexbuffersize; i < indexbuffersize + vertexbuffersize; i++) {
        fulldata.buf[i] = (unsigned int)floor(
                ((bufferout.buf[i] + __GE_MESH_FLOAT_RANGE) / 2 ) * __GE_MESH_PRECISION);
    } for(int i = indexbuffersize + vertexbuffersize; i < fulllength; i++) {
        fulldata.buf[i] = (unsigned int)floor(
                ((bufferout.buf[i] + __GE_MESH_FLOAT_RANGE) / 2 ) * __GE_MESH_PRECISION);
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

    //free buffers
    free(bufferout.buf);
    free(header.buf);
    free(fulldata.buf);
    free(filebuffer.buf);

    return true;
}