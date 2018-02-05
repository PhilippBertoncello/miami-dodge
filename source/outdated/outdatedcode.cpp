// DONT COMPILE THIS CODE; THIS IS JUST A BACKUP OF OLD CODE!

namespace readfile {
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

	bool gewritemeshfile(const char* file, const char* texturepath, unsigned int vertexmode,
	    unsigned int drawmode, unsigned int vlen, unsigned int vdim, 
	    unsigned int tlen, unsigned int tdim, geBufferf inbuffer) {

	    //variables
	    FILE *fptr;
	    unsigned int* buf; //4-Byte-Integer
	    unsigned int* hbuf;

	    //calculate header data
	    unsigned int hlength = 6;
	    unsigned int length;
	    length = (vlen * vdim) + (clen * cdim);

	    //allocate buffer
	    buf = (unsigned int*)malloc(sizeof(unsigned int) * length);
	    for(int i = 0; i < vlen * vdim; i++) {
	        buf[i] =
	        (unsigned int)floor(((buffer.buf[i] + 1) / 2) * __GE_MESH_PRECISION);
	    } for(int i = 0; i < clen * cdim; i++) {
	        buf[i + (vlen * vdim)] = 
	        (unsigned int)floor(buffer.buf[i + (vlen * vdim)] * __GE_MESH_PRECISION);
	    }

	    //open file
	    fptr = fopen(file, "wb");
	    if (!fptr) {
	        return NULL;
	    }
	    fseek(fptr, 0, SEEK_SET);

	    //make header
	    hbuf = (unsigned int*)malloc(6 * sizeof(int));
	    hbuf[0] = (unsigned int)hlength;
	    hbuf[1] = mode;
	    hbuf[2] = vlen;
	    hbuf[3] = vdim;
	    hbuf[4] = clen;
	    hbuf[5] = cdim;

	    //write data
	    fseek(fptr, 0, SEEK_SET);
	    fwrite(hbuf, sizeof(unsigned int), hlength, fptr);
	    fwrite(buf, sizeof(unsigned int), length, fptr);

	    //close file
	    fclose(fptr);
	    return true;
	}

	bool gewritemeshfile(const char *file, unsigned int mode, unsigned int vlen, unsigned int clen,
	    unsigned int vdim, unsigned int cdim, geBufferf vbuffer, geBufferf cbuffer) {

	    //variables
	    FILE *fptr;
	    unsigned int* buf; //4-Byte-Integer
	    unsigned int* hbuf;

	    //calculate header data
	    unsigned int hlength = 6;
	    unsigned int length;
	    length = (vlen * vdim) + (clen * cdim);

	    //allocate buffer
	    buf = (unsigned int*)malloc(sizeof(unsigned int) * length);

	    //write to buffer
	    for(int i = 0; i < vlen * vdim; i++) {
	        buf[i] = (unsigned int)floor((vbuffer.buf[i] + 1) / 2 * __GE_MESH_PRECISION);
	    } for(int i = 0; i < clen * cdim; i++) {
	        buf[i + (vlen * vdim)] = (unsigned int)floor((cbuffer.buf[i] + 1) / 2 * __GE_MESH_PRECISION);
	    }

	    //open file
	    fptr = fopen(file, "wb");
	    if (!fptr) {
	        return NULL;
	    }
	    fseek(fptr, 0, SEEK_SET);

	    //make header
	    hbuf = (unsigned int*)malloc(6 * sizeof(int));
	    hbuf[0] = (unsigned int)hlength;
	    hbuf[1] = mode;
	    hbuf[2] = vlen;
	    hbuf[3] = clen;
	    hbuf[4] = vdim;
	    hbuf[5] = cdim;

	    //write data
	    fseek(fptr, 0, SEEK_SET);
	    fwrite(hbuf, 4, hlength, fptr);
	    fwrite(buf, 4, length, fptr);

	    //close file
	    fclose(fptr);    
	}

	bool gewritemeshfile(const char *file, unsigned int args[], geBufferf buffer) {
	    //variables
	    FILE *fptr;
	    unsigned int* buf; //4-Byte-Integer
	    unsigned int* hbuf;

	    //calculate header data
	    unsigned int hlength = args[0];
	    unsigned int mode = args[1];
	    unsigned int vlen = args[2];
	    unsigned int clen = args[3];
	    unsigned int vdim = args[4];
	    unsigned int cdim = args[5];
	    int length;
	    length = (vlen * vdim) + (clen * cdim);

	    //allocate buffer
	    buf = (unsigned int*)malloc(sizeof(unsigned int) * length);

	    for(int i = 0; i < length; i++) {
	        buf[i] = (unsigned int)floor((buffer.buf[i] + 1) / 2 * __GE_MESH_PRECISION);
	    }

	    //open file
	    fptr = fopen(file, "wb");
	    if (!fptr) {
	        return NULL;
	    }
	    fseek(fptr, 0, SEEK_SET);

	    //make header
	    hbuf = (unsigned int*)malloc(6 * sizeof(int));
	    hbuf[0] = hlength;
	    hbuf[1] = mode;
	    hbuf[2] = vlen;
	    hbuf[3] = clen;
	    hbuf[4] = vdim;
	    hbuf[5] = cdim;

	    //write data
	    fwrite(hbuf, 4, hlength, fptr);
	    fwrite(buf, 4, length, fptr);

	    //close file
	    fclose(fptr);    
	}
};