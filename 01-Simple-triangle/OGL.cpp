#include <GLOW/shader.hpp>
#include <GLOW/window.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum {
	AMC_ATTRIBUTE_POSITION,
	AMC_ATTRIBUTE_COLOR
};

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_color = 0;

glm::mat4 perspectiveProjectionMatrix;	

Shader *simpleShader = NULL;

void init(void);

void keyboard(unsigned int);
void draw(void);
void update(void);
void resize(int, int);

void uninit();

// entry point function
int main(int argc, char **argv) {
	acewmInitializeCallback(init);

	acewmKeyboardCallback(keyboard);
	acewmReshapeCallback(resize);

	acewmDisplayCallback(draw);
	acewmUpdateCallback(update);

	acewmCreateWindow("Hello Triangle!", 100, 100, 800, 600);

	acewmEventLoop();

	acewmUninitiiizeCallback(uninit);

	return 0;
}

void init(void) {
  
    if (!gladLoadGL()) {
        exit(-1);
    }

    simpleShader = new Shader("shaders/shader.vert", "shaders/shader.frag");

	const GLfloat trianglePosition[] = {
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	const GLfloat triangleColor[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// VAO Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(trianglePosition), trianglePosition, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// enabling depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// set the clearColor() of window
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// initialize perspective projection matrix
	perspectiveProjectionMatrix = glm::mat4(1.0);
}

void keyboard(unsigned int key) {
	
}

void resize(int width, int height) {
    // code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix
	perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);	
}

void draw(void) {
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleShader->use();

	// transformations
	glm::mat4 modelViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -5.0f));
	glm::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// push above mvp vertex shader's mvp uniform
    simpleShader->setMat4("uMVPmatrix", modelViewProjectionMatrix);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glUseProgram(0);

    acewmSwapBuffers();
}

void update(void) {
    // code
}

void uninit(void) {
    glDeleteProgram(simpleShader->ID);

	// delete VBOs
    if (vbo_color) {
        glDeleteBuffers(1, &vbo_color);
        vbo_color = 0;
    }

	if (vbo_position) {
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	// delete VAO
	if (vao) {
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}
