#pragma once

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void loadTextureFromFile(const char* path, GLuint *texture)
{
    unsigned char *data;
    int width = 0, height = 0;
    int nrChannels = 0;

    // stbi_set_flip_vertically_on_load(true);

    data = stbi_load(path, &width, &height, &nrChannels, 0);

    // create OpenGL texture
	glGenTextures(1, texture);

	// bind to the generated texture
	glBindTexture(GL_TEXTURE_2D, *texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 1 is good for performance, 4 is good for alignment!

	// set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// craate multiple mipmap images
    if (nrChannels == 3) {
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (void*)data);
    }
    else if (nrChannels == 4) {
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
    }

    glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

    free(data);
}
