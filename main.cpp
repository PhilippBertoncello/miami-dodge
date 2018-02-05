#include"stdio.h"
#include"source/engine2d.cpp"

geMesh triangle;
glm::mat4 view;

int main(int argc, char** argv) {

	initGLUT(argc, argv, "Miami Dodge", 480, 360);
	return(0);
}

void onInitEnd() {
	geBufferf compSource;
	geBufferf vSource;
	geBufferf cSource;
	float computeSource[] = {
		-0.5, -0.5,
		0.5, -0.5,
		0.5, -0.5,
		0, 0.5,
		0, 0.5,
		-0.5, -0.5,
		1.0f,0.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f
	};
	float vertexSource[] = {
		-0.5, -0.5,
		0.5, -0.5,
		0.5, -0.5,
		0, 0.5,
		0, 0.5,
		-0.5, -0.5
	};
	float colorSource[] = {
		1.0f,0.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f,1.0f,
		0.0f,1.0f,0.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,
		0.0f,0.0f,1.0f,1.0f,
		1.0f,0.0f,0.0f,1.0f
	};
	compSource.buf = computeSource;
	vSource.buf = vertexSource;
	cSource.buf = colorSource;
	geBufferf indexbuffer;
	indexbuffer.buf = (float*)malloc(sizeof(float) * 4);
	gewritemeshfile("assets/mesh/tri.mesh", 0, GL_LINES, 6, 2, 6, 4, vSource, cSource, indexbuffer);
	view = glm::mat4(1.0f);
	
	triangle.loadfile("assets/mesh/tri.mesh");
	//triangle.load(&vSource, 12, &cSource, 24, GL_LINES);
	triangle.init("source/shader/vertex.vsh", "source/shader/fragment.fsh");
	// gewritemeshfile("assets/mesh/tri.mesh", 0, __GE_MESH_MODE_LINE, triangle.vertexdatasize / 2,
	// 	2, triangle.colordatasize / 4, 4, triangle.vertexdata, triangle.colordata, indexbuffer);
	triangle.setViewmodelUniform(view);
	onDraw();
}

void onDraw() {
	triangle.setViewmodelUniform(view);
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	triangle.draw();
			
	glutSwapBuffers();
	printf("Updated screen\n");
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

void onMouse(int button, int state, int x, int y) {

}