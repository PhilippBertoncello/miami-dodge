#include"stdio.h"
#include"source/engine2d.cpp"

// //vertex data
// float vertexSource[] = {
// 	-0.5, -0.5,
// 	0.5, -0.5,
// 	0.5, -0.5,
// 	0, 0.5,
// 	0, 0.5,
// 	-0.5, -0.5
// };

// //color data
// float colorSource[] = {
// 	1.0f,0.0f,0.0f,1.0f,
// 	0.0f,1.0f,0.0f,1.0f,
// 	0.0f,1.0f,0.0f,1.0f,
// 	0.0f,0.0f,1.0f,1.0f,
// 	0.0f,0.0f,1.0f,1.0f,
// 	1.0f,0.0f,0.0f,1.0f
// };

// float computeSource[] = {
// 	-0.5, -0.5,
// 	0.5, -0.5,
// 	0.5, -0.5,
// 	0, 0.5,
// 	0, 0.5,
// 	-0.5, -0.5,
// 	1.0f,0.0f,0.0f,1.0f,
// 	0.0f,1.0f,0.0f,1.0f,
// 	0.0f,1.0f,0.0f,1.0f,
// 	0.0f,0.0f,1.0f,1.0f,
// 	0.0f,0.0f,1.0f,1.0f,
// 	1.0f,0.0f,0.0f,1.0f
// };

// float fillSource[] = {
// 	-0.5, -0.5,
// 	0.5, -0.5,
// 	0, 0.5,
// 	1.0f,0.0f,0.0f,1.0f,
// 	0.0f,1.0f,0.0f,1.0f,
// 	0.0f,0.0f,1.0f,1.0f
// };

// geBufferf vSource;
// geBufferf cSource;
// geBufferf compSource;
// geBufferf fSource;

Mesh triangle;

glm::mat4 view;

int main(int argc, char** argv) {
	// compSource.buf = computeSource;
	// fSource.buf = fillSource;
	// writemeshfile("assets/mesh/tri.mesh", __READFILE_MESH_MODE_LINE, 6, 2, 6, 4, compSource);
	// unsigned int *args;
	// args = readmeshfile("assets/mesh/tri.mesh", &vSource, &cSource);


	initGLUT(argc, argv, "Miami Dodge", 480, 360);
	glutMainLoop();
	return(0);
}

void onInitEnd() {
	view = glm::mat4(1.0f);
	triangle.loadfile("assets/mesh/tri.mesh");
	triangle.init("source/shader/vertex.vsh", "source/shader/fragment.fsh");
	triangle.setViewmodelUniform(view);
	onDraw();
}

void onDraw() {
	triangle.setViewmodelUniform(view);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	triangle.draw();

	glutSwapBuffers();
}

void onKeyboard(unsigned char c, int x, int y) {
	switch(c) {
	case 'q':
		triangle.transform.rotate(-10);
		break;
	case 'w':
		triangle.transform.translate(0, 0.1f);
		break;
	case 'e':
		triangle.transform.rotate(10);
		break;
	case 'a':
		triangle.transform.translate(-0.1f, 0);
		break;
	case 's':
		triangle.transform.translate(0, -0.1f);
		break;
	case 'd':
		triangle.transform.translate(0.1f, 0);
		break;
	}
	glutPostRedisplay();
}

void onMouse(int button, int state ,int x, int y) {

}