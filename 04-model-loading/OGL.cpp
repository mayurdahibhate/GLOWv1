#include <GLOW/shader.hpp>
#include <GLOW/window.hpp>
#include <GLOW/texture.hpp>
#include <GLOW/model.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum {
	AMC_ATTRIBUTE_POSITION,
	AMC_ATTRIBUTE_NORMAL
};

void init(void);

void draw(void);
void update(void);
void resize(int, int);

void uninit();

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_normal = 0;

glm::mat4 perspectiveProjectionMatrix;

GLOWshader *simpleShader = NULL;

uint32_t numVert = 0;
float angle = 0.0f;

// entry point function
int main(int argc, char **argv) {
	
	GLOWwindow* window = glowCreateWindow("Model loading!", 800, 600);
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

	float *model_positions = NULL;
	float *model_normals = NULL;

	numVert = loadModel("suzanne.obj", &model_positions, &model_normals, NULL);

	// VAO Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// VBO for position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVert, model_positions, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVert, model_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// enabling depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// set the clearColor() of window
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

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

	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, -5.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 viewMatrix = glm::mat4(1.0);
	glm::mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * viewMatrix * modelMatrix;

	// push above mvp vertex shader's mvp uniform
	simpleShader->setMat4("uModelViewProjectionMatrix", modelViewProjectionMatrix);
	simpleShader->setMat3("uNormalMatrix",  glm::mat3(transpose(inverse(modelMatrix))));
	simpleShader->setVec3("uLightDirection", glm::vec3(1.0f, 1.0f, 1.0f));
	simpleShader->setVec4("uColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, numVert);

	glBindVertexArray(0);
	glUseProgram(0);
}

void update(void) {
    angle = angle + 0.2f;

	if (angle >= 360.0f) {
		angle = angle - 360.0f;
	}
} 

void uninit(void) {
    glDeleteProgram(simpleShader->ID);

	// delete VBOs
    if (vbo_normal) {
        glDeleteBuffers(1, &vbo_normal);
        vbo_normal = 0;
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
