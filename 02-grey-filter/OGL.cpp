#include <GLOW/shader.hpp>
#include <GLOW/window.hpp>
#include <GLOW/texture.hpp>

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
GLuint desertTexture = 0;	

GLOWshader *simpleShader = NULL;

void init(void);

void keyboard(unsigned int);
void draw(void);
void update(void);
void resize(int, int);

void uninit();

// entry point function
int main(int argc, char **argv) {

	GLOWwindow* window = glowCreateWindow("Grey filter!", 800, 600);
	window->glowReshapeCallback(resize);
	
	init();

	while (!(window->glowWindowShouldClose())) {
		draw();
		update();
    
		window->glowSwapBuffers();
	}

	uninit();
	window->glowDestroyWindow();
}

void init(void) {
  
    if (!gladLoadGL()) {
        exit(-1);
    }

    simpleShader = new GLOWshader("shaders/shader.vert", "shaders/shader.frag");

	loadTextureFromFile("desert.png", &desertTexture);
	
	const float rectangle_position[] = {
		// first triangle
		1.0f, 1.0f, 0.0f,  // right top
		-1.0f, 1.0f, 0.0f, // left top
		-1.0f, -1.0f, 0.0, // left bottom

		// second triangle
		-1.0f, -1.0f, 0.0f, // left bottom
		1.0f, -1.0f, 0.0f,  // right bottom
		1.0f, 1.0f, 0.0f	// right top
	};

	const float rectangle_texcoords[] = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	// VAO Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_position), rectangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_texcoords), rectangle_texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// enabling depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_TEXTURE0);

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
	glm::mat4 modelViewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	// push above mvp vertex shader's mvp uniform
    simpleShader->setMat4("uMVPmatrix", modelViewProjectionMatrix);
	simpleShader->setInt("uTexture", 0);

	glBindTexture(GL_TEXTURE_2D, desertTexture);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
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
