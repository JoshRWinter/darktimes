#include <win/Win.hpp>

#ifdef WIN_USE_OPENGL

#define WIN_GL_EXTENSION_STORAGE
#include <win/gl/GL.hpp>

namespace win
{

#if defined WINPLAT_LINUX

static void *getproc(const char *name)
{
	void *address = (void *) glXGetProcAddress((const unsigned char *) name);
	if (address == NULL)
		win::bug(std::string("Could not get extension \"") + name + "\"");

	return address;
}

#elif defined WINPLAT_WINDOWS
static void *getproc(const char *name)
{
	void *address = (void*)wglGetProcAddress(name);
	if(address == NULL)
	{
		MessageBox(NULL, (std::string("Missing opengl func: ") + name).c_str(), "Missing Opengl extension", MB_ICONEXCLAMATION);
		std::abort();
	}

	return address;
}
#endif

void gl_check_error()
{
	const char *errname;
	const auto err = glGetError();

	switch (err)
	{
		case GL_NO_ERROR:
			errname = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			errname = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errname = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errname = "GL_INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			errname = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			errname = "GL_STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			errname = "GL_OUT_OF_MEMORY";
			break;
		default:
			errname = "Unknown gl error";
			break;
	}
	if (err != GL_NO_ERROR)
		win::bug("GL error " + std::string(errname) + " (" + std::to_string(err) + ")");
}

static void *getproc(const char*);
void load_gl_extensions()
{
	glCreateShader = (decltype(glCreateShader))getproc("glCreateShader");
	glShaderSource = (decltype(glShaderSource))getproc("glShaderSource");
	glCompileShader = (decltype(glCompileShader))getproc("glCompileShader");
	glGetShaderiv = (decltype(glGetShaderiv))getproc("glGetShaderiv");
	glGetShaderInfoLog = (decltype(glGetShaderInfoLog))getproc("glGetShaderInfoLog");
	glGetProgramiv = (decltype(glGetProgramiv))getproc("glGetProgramiv");
	glGetProgramInfoLog = (decltype(glGetProgramInfoLog))getproc("glGetProgramInfoLog");
	glAttachShader = (decltype(glAttachShader))getproc("glAttachShader");
	glDetachShader = (decltype(glDetachShader))getproc("glDetachShader");
	glLinkProgram = (decltype(glLinkProgram))getproc("glLinkProgram");
	glDeleteShader = (decltype(glDeleteShader))getproc("glDeleteShader");
	glCreateProgram = (decltype(glCreateProgram))getproc("glCreateProgram");
	glUseProgram = (decltype(glUseProgram))getproc("glUseProgram");
	glDeleteProgram = (decltype(glDeleteProgram))getproc("glDeleteProgram");

	glGenVertexArrays = (decltype(glGenVertexArrays))getproc("glGenVertexArrays");
	glBindVertexArray = (decltype(glBindVertexArray))getproc("glBindVertexArray");
	glDeleteVertexArrays = (decltype(glDeleteVertexArrays))getproc("glDeleteVertexArrays");

	glGenBuffers = (decltype(glGenBuffers))getproc("glGenBuffers");
	glBindBuffer = (decltype(glBindBuffer))getproc("glBindBuffer");
	glDeleteBuffers = (decltype(glDeleteBuffers))getproc("glDeleteBuffers");

	glBufferData = (decltype(glBufferData))getproc("glBufferData");
	glBufferStorage = (decltype(glBufferStorage))getproc("glBufferStorage");
	glMapBufferRange = (decltype(glMapBufferRange))getproc("glMapBufferRange");

	glVertexAttribDivisor = (decltype(glVertexAttribDivisor))getproc("glVertexAttribDivisor");
	glVertexAttribPointer = (decltype(glVertexAttribPointer))getproc("glVertexAttribPointer");
	glEnableVertexAttribArray = (decltype(glEnableVertexAttribArray))getproc("glEnableVertexAttribArray");

	glGetUniformBlockIndex = (decltype(glGetUniformBlockIndex))getproc("glGetUniformBlockIndex");
	glUniformBlockBinding = (decltype(glUniformBlockBinding))getproc("glUniformBlockBinding");
	glGetUniformLocation = (decltype(glGetUniformLocation))getproc("glGetUniformLocation");
	glUniformMatrix4fv = (decltype(glUniformMatrix4fv))getproc("glUniformMatrix4fv");
	glUniform1f = (decltype(glUniform1f))getproc("glUniform1f");
	glUniform2f = (decltype(glUniform2f))getproc("glUniform2f");
	glUniform4f = (decltype(glUniform4f))getproc("glUniform4f");
	glUniform1i = (decltype(glUniform1i))getproc("glUniform1i");
	glUniform2i = (decltype(glUniform2i))getproc("glUniform2i");

	glDrawElementsInstanced = (decltype(glDrawElementsInstanced))getproc("glDrawElementsInstanced");
	glDrawElementsBaseVertex = (decltype(glDrawElementsBaseVertex))getproc("glDrawElementsBaseVertex");
	glMultiDrawElementsIndirect = (decltype(glMultiDrawElementsIndirect))getproc("glMultiDrawElementsIndirect");

#if defined WINPLAT_LINUX
	glXSwapIntervalEXT = (decltype(glXSwapIntervalEXT))getproc("glXSwapIntervalEXT");
#elif defined WINPLAT_WINDOWS
	wglSwapIntervalEXT = (decltype(wglSwapIntervalEXT))getproc("wglSwapIntervalEXT");
	glTexImage3D = (decltype(glTexImage3D))getproc("glTexImage3D");
#endif
}

GLuint load_gl_shaders(const std::string &vertex, const std::string &fragment)
{
	const char *const vertex_cstr = vertex.c_str();
	const char *const fragment_cstr = fragment.c_str();
	const int vertex_cstr_len = vertex.length();
	const int fragment_cstr_len = fragment.length();

	const unsigned vshader = glCreateShader(GL_VERTEX_SHADER);
	const unsigned fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vshader, 1, &vertex_cstr, &vertex_cstr_len);
	glShaderSource(fshader, 1, &fragment_cstr, &fragment_cstr_len);

	glCompileShader(vshader);
	glCompileShader(fshader);

	int success = 1;
	char buffer[2000] = "";
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		glGetShaderInfoLog(vshader, 2000, NULL, buffer);
		win::bug(std::string("vertex shader:\n") + buffer);
	}
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if(success == 0)
	{
		glGetShaderInfoLog(fshader, 2000, NULL, buffer);
		win::bug(std::string("fragment shader:\n") + buffer);
	}
	unsigned program = glCreateProgram();
	glAttachShader(program, vshader);
	glAttachShader(program, fshader);
	glLinkProgram(program);
	GLint linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		glGetProgramInfoLog(program, 2000, NULL, buffer);
		win::bug(std::string("linker:\n") + buffer);
	}
	glDetachShader(program, vshader);
	glDetachShader(program, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	return program;
}

const char *v = "#version 330 core\nvoid main(){}";
GLuint load_gl_shaders(Stream vertex, Stream fragment)
{
	return load_gl_shaders(vertex.read_all_as_string(), fragment.read_all_as_string());
}

}

#endif
