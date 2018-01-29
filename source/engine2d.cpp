#include"math.h"
#include"readfile.cpp"
#include"GL/glut.h"
#include"stdio.h"
#include"glm/glm.hpp"
#include"glm/gtc/type_ptr.hpp"
#include"glm/gtc/matrix_transform.hpp"
//#include"object.cpp"

#define MATH_PI 3.14159265358979323846264338237950288f
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLuint _maxShaderCompilerLogLength = 512;

namespace shaderCompiler {
	GLuint makeVertexShader(const char* source) {
		GLuint out;
		int length;
		GLint compileStatus;
		GLchar log[_maxShaderCompilerLogLength];

		out = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(out, 1, (GLchar **)&source, NULL);
		glCompileShader(out);

		glGetShaderiv(out, GL_COMPILE_STATUS, &compileStatus);
		glGetShaderInfoLog(out, _maxShaderCompilerLogLength, &length, log);

		printf("Vertex Shader:   %s%s\n\n",
			compileStatus ? "\e[32mCompilation Successful!\e[39m" :
			"\e[31mCompilation Failed!\e[34m\n    Compiler output:\n    ",
			compileStatus ? "" : log);

		return out;
	}

	GLuint makeFragmentShader(const char* source) {
		GLuint out;
		int length;
		GLint compileStatus;
		GLchar log[_maxShaderCompilerLogLength];	

		out = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(out, 1, (GLchar **)&source, NULL);
		glCompileShader(out);

		glGetShaderiv(out, GL_COMPILE_STATUS, &compileStatus);
		glGetShaderInfoLog(out, _maxShaderCompilerLogLength, &length, log);


		printf("Fragment Shader: %s%s%s\n\n",
			compileStatus ? "\e[32mCompilation Successful!\e[39m" :
			"\e[31mCompilation Failed!\e[34m\n    Compiler output:\n    ",
			compileStatus ? "" : log,
			"\e[39m");

		return out;	
	}

	GLuint makeShaderProgram(GLuint vID, GLuint fID) {
		GLuint out;

		out = glCreateProgram();
		glAttachShader(out, vID);
		glAttachShader(out, fID);
		glLinkProgram(out);

		return out;
	}
};

class Transform {
public:
	glm::mat4 transform;

	Transform() {
		transform = glm::mat4(1.0f);
	}

	void translate(float x, float y) {
		transform = glm::transpose(glm::translate(glm::transpose(transform), glm::vec3(x, y, 0)));
	}

	void rotate(float theta) { //in degrees
		transform = glm::rotate(transform, (theta * MATH_PI) / 180, glm::vec3(0.0f, 0.0f, 1.0f));
	}
};

class Camera {
public:
	Transform transform;
};

class Mesh {
public:
	GLuint vaoID;
	GLuint vboID;
	unsigned int vertexdatasize;
	geBufferf vertexdata;
	GLuint vertexdataID;
	unsigned int colordatasize;
	geBufferf colordata;
	GLuint colordataID;
	GLuint shaderID;
	Transform transform;
	GLuint viewmodelID;
	GLenum mode;

	GLuint clippingplaneID;

	bool initialized;
	bool loaded;

	Mesh() {
		vertexdatasize = 0;
		colordatasize = 0;
		initialized = false;
		loaded = false;
	}

	void init(const char* vertexpath, const char* fragmentpath) {
		//Shader stuff
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		vertexShaderID = shaderCompiler::makeVertexShader(readfile(vertexpath));
		fragmentShaderID = shaderCompiler::makeFragmentShader(readfile(fragmentpath));
		shaderID = shaderCompiler::makeShaderProgram(vertexShaderID, fragmentShaderID);

		//Make VBO and VAO and fill VBO with data
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, ((vertexdatasize) + (colordatasize)) * sizeof(GLfloat), 
			NULL, GL_STATIC_DRAW );
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexdatasize * sizeof(float), vertexdata.buf);
		glBufferSubData(GL_ARRAY_BUFFER, vertexdatasize * sizeof(float), 
			colordatasize * sizeof(float), colordata.buf);

		//Get vertex attribute pointers and activate the reader
		vertexdataID = glGetAttribLocation(shaderID, "in_position");
		colordataID = glGetAttribLocation(shaderID, "in_color");
		glVertexAttribPointer(vertexdataID, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(colordataID, 4, GL_FLOAT, GL_FALSE, 0, 
			BUFFER_OFFSET(vertexdatasize * sizeof(float)));
		glEnableVertexAttribArray(vertexdataID);
		glEnableVertexAttribArray(colordataID);

		//Prepare matrices
		transform = Transform();
		transform.transform = glm::mat4(1.0f);

		//Configure perspective
		clippingplaneID = glGetUniformLocation(shaderID, "uni_clippingplane");
		viewmodelID = glGetUniformLocation(shaderID, "uni_viewmodel");
		glUniform2f(clippingplaneID, 0.1f, 10.0f);

		//set standart values
		glUniformMatrix4fv(viewmodelID, 1, GL_FALSE, glm::value_ptr(transform.transform));

		//confirm initialization
		initialized = true;
	}

	bool load(geBufferf* vdatain, unsigned int vlengthin,
		geBufferf* cdatain, unsigned int clengthin, GLenum modeIn) {
		vertexdatasize = vlengthin;
		colordatasize = clengthin;

		vertexdata.buf = vdatain->buf;
		colordata.buf = cdatain->buf;

		mode = modeIn;
		loaded = true;
	}

	bool load(float *vdatain, unsigned int vlengthin,
		unsigned int clengthin, GLenum modeIn) {
		vertexdatasize = vlengthin;
		colordatasize = clengthin;

		vertexdata.buf = (float*)malloc(sizeof(float) * (vertexdatasize));
		colordata.buf = (float*)malloc(sizeof(float) * (colordatasize));

		for(int i = 0; i < vertexdatasize; i++) {
			vertexdata.buf[i] = vdatain[i];			
		}

		for(int i = 0; i < colordatasize; i++) {
			colordata.buf[i] = vdatain[i + vertexdatasize];			
		}

		mode = modeIn;
		loaded = true;
	}

	bool loadfile(const char* path) {
		unsigned int *args;
		geBufferf vdata;
		geBufferf cdata;
		args = readmeshfile(path, &vdata, &cdata);
		GLenum modeIn = args[1];
		unsigned int vlen = args[2] * args[3];
		unsigned int clen = args[4] * args[5];


		load(&vdata, vlen, &cdata, clen, modeIn);

		return true;
	}

	bool loadPtr(float *vdatain, unsigned int vlengthin,
		unsigned int clengthin, GLenum modeIn) {
		vertexdatasize = vlengthin;
		colordatasize = clengthin;

		vertexdata.buf = (float*)malloc(sizeof(float) * (vertexdatasize));
		colordata.buf = (float*)malloc(sizeof(float) * (colordatasize));

		memcpy(vertexdata.buf, vdatain, vertexdatasize);
		memcpy(colordata.buf, vdatain, colordatasize);

		mode = modeIn;
		loaded = true;
	}

	void draw() {
		if (!initialized || !loaded) return;

		glUseProgram(shaderID);
		glBindVertexArray(vaoID);
		glDrawArrays(mode, 0, vertexdatasize / 2);
	}

	void setViewmodelUniform(glm::mat4 view) {
		if (!initialized || !loaded) return;

		glm::mat4 viewmodel = glm::mat4(1.0f);
		viewmodel = transform.transform;
		glUniformMatrix4fv(viewmodelID, 1, GL_FALSE, glm::value_ptr(viewmodel));
	}
};

using namespace shaderCompiler;

void onDraw();
void onKeyboard(unsigned char c, int x, int y);
void onMouse(int button, int state, int x, int y);
void onInitEnd();

void initGLUT(int argc, char** argv, const char* title, int w, int h) {
	glutInit(&argc, argv);
	glutCreateWindow(title);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(50, 50);
	glutDisplayFunc(onDraw);
	glutKeyboardFunc(onKeyboard);
	glutMouseFunc(onMouse);

	onInitEnd();
}
