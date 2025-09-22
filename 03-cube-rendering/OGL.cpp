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

void draw(void);
void update(void);
void resize(int, int);

void uninit();

// entry point function
int main(int argc, char **argv) {
	
	GLOWwindow* window = glowCreateWindow("Cube rendering!", 800, 600);
	window->glowReshapeCallback(resize);
	
	init();

	while (!(window->glowWindowShouldClose())) {
		draw();
		update();
    
		window->glowSwapBuffers();
	}

	uninit();
	window->glowDestroyWindow();

	return 0;
}

void init(void) {
  
    if (!gladLoadGL()) {
        exit(-1);
    }

    simpleShader = new GLOWshader("shaders/shader.vert", "shaders/shader.frag");

	// position
	float cubeVertices[] = {
		// front
		// triangle one
		1.0f,  1.0f,  1.0f, // top-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		1.0f, -1.0f,  1.0f, // bottom-right of front
		
		// triangle two
		1.0f, -1.0f,  1.0f, // bottom-right of front
		-1.0f,  1.0f,  1.0f, // top-left of front
		-1.0f, -1.0f,  1.0f, // bottom-left of front

		// right
		// triangle one
		1.0f,  1.0f, -1.0f, // top-right of right
		1.0f,  1.0f,  1.0f, // top-left of right
		1.0f, -1.0f, -1.0f, // bottom-right of right
		
		// triangle two
		1.0f, -1.0f, -1.0f, // bottom-right of right
		1.0f,  1.0f,  1.0f, // top-left of right
		1.0f, -1.0f,  1.0f, // bottom-left of right

		// back
		// triangle one
		1.0f,  1.0f, -1.0f, // top-right of back
		-1.0f,  1.0f, -1.0f, // top-left of back
		1.0f, -1.0f, -1.0f, // bottom-right of back
		
		// triangle two
		1.0f, -1.0f, -1.0f, // bottom-right of back
		-1.0f,  1.0f, -1.0f, // top-left of back
		-1.0f, -1.0f, -1.0f, // bottom-left of back

		// left
		// triangle one
		-1.0f,  1.0f,  1.0f, // top-right of left
		-1.0f,  1.0f, -1.0f, // top-left of left
		-1.0f, -1.0f,  1.0f, // bottom-right of left
		
		// triangle two
		-1.0f, -1.0f,  1.0f, // bottom-right of left
		-1.0f,  1.0f, -1.0f, // top-left of left
		-1.0f, -1.0f, -1.0f, // bottom-left of left

		// top
		// triangle one
		1.0f,  1.0f, -1.0f, // top-right of top
		-1.0f,  1.0f, -1.0f, // top-left of top
		1.0f,  1.0f,  1.0f, // bottom-right of top

		// triangle two
		1.0f,  1.0f,  1.0f, // bottom-right of top
		-1.0f,  1.0f, -1.0f, // top-left of top
		-1.0f,  1.0f,  1.0f, // bottom-left of top

		// bottom
		// triangle one
		1.0f, -1.0f,  1.0f, // top-right of bottom
		-1.0f, -1.0f,  1.0f, // top-left of bottom
		1.0f, -1.0f, -1.0f, // bottom-right of bottom
		
		// triangle two
		1.0f, -1.0f, -1.0f, // bottom-right of bottom
		-1.0f, -1.0f,  1.0f, // top-left of bottom
		-1.0f, -1.0f, -1.0f, // bottom-left of bottom
	};

	float cubeNormals[] = {
		// front surface
		// triangle one
		0.0f,  0.0f,  1.0f, // top-right of front
		0.0f,  0.0f,  1.0f, // top-left of front
		0.0f,  0.0f,  1.0f, // bottom-right of front
		
		// triangle two
		0.0f,  0.0f,  1.0f, // bottom-right of front
		0.0f,  0.0f,  1.0f, // top-left of front
		0.0f,  0.0f,  1.0f, // bottom-left of front

		// right surface
		// triangle one
		1.0f,  0.0f,  0.0f, // top-right of right
		1.0f,  0.0f,  0.0f, // top-left of right
		1.0f,  0.0f,  0.0f, // bottom-right of right

		// triangle two
		1.0f,  0.0f,  0.0f, // bottom-right of right
		1.0f,  0.0f,  0.0f, // top-left of right
		1.0f,  0.0f,  0.0f, // bottom-left of right

		// back surface
		// triangle one
		0.0f,  0.0f, -1.0f, // top-right of back
		0.0f,  0.0f, -1.0f, // top-left of back
		0.0f,  0.0f, -1.0f, // bottom-right of back

		// triangle two
		0.0f,  0.0f, -1.0f, // bottom-right of back
		0.0f,  0.0f, -1.0f, // top-left of back
		0.0f,  0.0f, -1.0f, // bottom-left of back

		// left surface
		// triangle one
		-1.0f,  0.0f,  0.0f, // top-right of left
		-1.0f,  0.0f,  0.0f, // top-left of left
		-1.0f,  0.0f,  0.0f, // bottom-right of left

		// triangle two
		-1.0f,  0.0f,  0.0f, // bottom-right of left
		-1.0f,  0.0f,  0.0f, // top-left of left
		-1.0f,  0.0f,  0.0f, // bottom-left of left

		// top surface
		// triangle one
		0.0f,  1.0f,  0.0f, // top-right of top
		0.0f,  1.0f,  0.0f, // top-left of top
		0.0f,  1.0f,  0.0f, // bottom-right of top

		// triangle two
		0.0f,  1.0f,  0.0f, // bottom-right of top
		0.0f,  1.0f,  0.0f, // top-left of top
		0.0f,  1.0f,  0.0f, // bottom-left of top

		// bottom surface
		// triangle one
		0.0f, -1.0f,  0.0f, // top-right of bottom
		0.0f, -1.0f,  0.0f, // top-left of bottom
		0.0f, -1.0f,  0.0f, // bottom-right of bottom
		
		// triangle two
		0.0f, -1.0f,  0.0f, // bottom-right of bottom
		0.0f, -1.0f,  0.0f, // top-left of bottom
		0.0f, -1.0f,  0.0f, // bottom-left of bottom
	};

	// VAO Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
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

void resize(int width, int height) {
    // code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = glm::perspective(glm::radians(40.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void draw(void) {
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleShader->use();

	// transformations
	glm::mat4 modelMatrix = glm::mat4(1.0);
	// modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, -5.0));

	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5, 0.5));

	modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

	// push above mvp vertex shader's mvp uniform
	simpleShader->setMat4("uModelViewProjectionMatrix", modelViewProjectionMatrix);
	simpleShader->setMat3("uNormalMatrix", glm::mat3(modelMatrix));
	simpleShader->setVec3("uLightDirection", glm::vec3(1.0f, 1.0f, 1.0f));
	simpleShader->setVec4("uColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 36);

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
