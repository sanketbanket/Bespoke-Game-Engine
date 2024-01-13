#include "textureClass.h"

Texture::Texture(const char* imageFile, int texNumber) {
	int widthImg, heightImg, numColChannel;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(imageFile, &widthImg, &heightImg, &numColChannel, 0);
	if (!bytes) {
		std::cout << "Failed to load image" << std::endl;
	}
	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + texNumber);
	glBindTexture(GL_TEXTURE_2D, ID);
	
	if (numColChannel == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (numColChannel == 4) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
	unit = texNumber;
};


void Texture::Bind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::Delete() {
	glDeleteTextures(1, &ID);
}