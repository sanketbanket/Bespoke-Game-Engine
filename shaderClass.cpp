#include "shaderClass.h"

std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);

}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	
	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "VERTEX SHADER FAILED TO COMPILE \n" << infoLog << std::endl;
	};




	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	int success_2;
	char infoLog_2[512];

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success_2);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog_2);
		std::cout << "FRAGMENT SHADER FAILED TO COMPILE \n" << infoLog_2 << std::endl;
	};
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}
void Shader::Activate() {
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}

void Shader::Setvec3(const char* name, glm::vec3 vector) {
	glUniform3fv(glGetUniformLocation(ID, name), 1, glm::value_ptr(vector));
}
void Shader::Setmat4(const char* name, glm::mat4 matrix) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::Set1f(const char* name, float value) {
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::Set1i(const char* name, int value) {
	glUniform1i(glGetUniformLocation(ID, name), value);
}