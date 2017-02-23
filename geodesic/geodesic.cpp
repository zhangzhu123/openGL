#include <iostream>
#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "oglShader.h"
#include "oglTexture.h"
#include "oglModel.h"
#include "oglMath.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement();

const GLuint WIDTH = 800, HEIGHT = 800;
bool keys[1024];

GLfloat camPosition[3] = { 0,0,0 };
GLfloat camForward[4] = { 0,0,-0.005,0 };
GLfloat camBack[4] = { 0,0,0.005 };
GLfloat camLeft[4] = { -0.005,0,0 };
GLfloat camRight[4] = { 0.005,0,0 };

const GLfloat camForwardC[4] = { 0,0,-0.1,0 };
const GLfloat camBackC[4] = { 0,0,0.1 };
const GLfloat camLeftC[4] = { -0.1,0,0 };
const GLfloat camRightC[4] = { 0.1,0,0 };

GLfloat camYaw = 0;
GLfloat camPitch = 0;

bool firstMouse = true;
GLfloat lastX, lastY;

GLint subdi = 1;

void getFPS()
{
	static float framesPerSecond = 0.0f;
	static float lastTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f;

	++framesPerSecond;

	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		std::cout << "FPS: " << framesPerSecond << std::endl;
		framesPerSecond = 0;
	}
}


int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.5);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OPENGLDEMO", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//get opengl version
	const GLubyte* OpenGLVersion = glGetString(GL_VERSION);
	std::cout << "Version: " << OpenGLVersion << std::endl;

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	Shader baseShader("E:\\opengltst\\oglDemo\\geodesic\\shader\\vertex.glsl",
		              "E:\\opengltst\\oglDemo\\geodesic\\shader\\tessControl.glsl",
		              "E:\\opengltst\\oglDemo\\geodesic\\shader\\tessEval.glsl",
		              "E:\\opengltst\\oglDemo\\geodesic\\shader\\geometry.glsl", 
		              "E:\\opengltst\\oglDemo\\geodesic\\shader\\fragment.glsl");

	Model baseModel;

	Texture baseTexture("E:/opengltst/oglDemo/oglBases/oglBases/texture/soil.jpg");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		// Render
		// Clear the colorbuffer
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, baseTexture.texture);
		//glUniform1i(glGetUniformLocation(baseShader.program, "ourTexture1"), 0);

		//generate model matrix
		GLfloat modelTM[16];
		GLfloat tv[3] = { 0, 0, 0 };
		oglTranslate(tv, modelTM);

		GLfloat modelRM[16];
		GLfloat axis[3] = { 0, 1, 0 };
		GLfloat theta = 0;
		oglRotateQuaternion(axis, theta, modelRM);

		GLfloat modelSM[16];
		GLfloat sv[3] = { 1, 1, 1 };
		oglScale(sv, modelSM);

		GLfloat modelMatric[16];
		oglModelMatrix(modelTM, modelRM, modelSM, modelMatric);

		GLint transformLoc = glGetUniformLocation(baseShader.program, "modelMatric");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, modelMatric);

		//generate view matrix
		GLfloat viewTM[16];
		GLfloat(&vmt)[3] = camPosition;
		oglTranslate(vmt, viewTM);

		GLfloat viewRM[16];
		GLfloat &vmtheta = camYaw;
		GLfloat &vmthetaP = camPitch;
		oglRotateEuler(camYaw, camPitch, 0, viewRM);

		GLfloat viewMatric[16];
		oglViewMatrix(viewTM, viewRM, viewMatric);
		GLint viewMatricLoc = glGetUniformLocation(baseShader.program, "viewMatrix");
		glUniformMatrix4fv(viewMatricLoc, 1, GL_FALSE, viewMatric);

		//generate Perspective matrix
		GLfloat perspMatric[16];
		GLfloat fov = 45;
		GLfloat aspect = 1;
		GLfloat nearPlane = 0.001;
		GLfloat farPlane = 1000;

		oglPerspectiveMatrix(fov, aspect, nearPlane, farPlane, perspMatric);
		GLint perspMatricLoc = glGetUniformLocation(baseShader.program, "perspMatrix");
		glUniformMatrix4fv(perspMatricLoc, 1, GL_FALSE, perspMatric);

		GLfloat modelView[16];
		oglMatrixMul(viewMatric, modelMatric, modelView);

		GLfloat transModelView[16];
		oglTranspositionMatrix(modelView, transModelView);

		GLfloat packed[9] = { transModelView[0], transModelView[1], transModelView[2],
			                  transModelView[4], transModelView[5], transModelView[6],
		                      transModelView[8], transModelView[9], transModelView[10] };
		glUniformMatrix3fv(glGetUniformLocation(baseShader.program, "NormalMatrix"), 1, 0, packed);
		glUniform1f(glGetUniformLocation(baseShader.program, "TessLevelInner"), subdi);
		glUniform1f(glGetUniformLocation(baseShader.program, "TessLevelOuter"), subdi);
		glUniform3f(glGetUniformLocation(baseShader.program, "AmbientMaterial"), 0.04f, 0.04f, 0.04f);
		glUniform3f(glGetUniformLocation(baseShader.program, "DiffuseMaterial"), 0, 0.75, 0.75);
		glUniform3f(glGetUniformLocation(baseShader.program, "LightPosition"), 0.25, 0.25, 1);
		// Draw our first triangle
		baseShader.use();
		glBindVertexArray(baseModel.VAO);
		glPatchParameteri(GL_PATCH_VERTICES, 3);
		//glDrawArrays(GL_PATCHES, 0, 36);
		glDrawElements(GL_PATCHES, baseModel.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		getFPS();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}


// Moves/alters the camera positions based on user input
void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W]) {
		camPosition[0] += camForward[0];
		camPosition[1] += camForward[1];
		camPosition[2] += camForward[2];
	}
	if (keys[GLFW_KEY_S]) {
		camPosition[0] += camBack[0];
		camPosition[1] += camBack[1];
		camPosition[2] += camBack[2];
	}
	if (keys[GLFW_KEY_A]) {
		camPosition[0] += camLeft[0];
		camPosition[1] += camLeft[1];
		camPosition[2] += camLeft[2];
	}
	if (keys[GLFW_KEY_D]) {
		camPosition[0] += camRight[0];
		camPosition[1] += camRight[1];
		camPosition[2] += camRight[2];
	}
	if (keys[GLFW_KEY_N]) {
		if (subdi >= 10)
		{
		    subdi = 100;
		}
		else 
		{
			subdi++;
		}
	}
	if (keys[GLFW_KEY_M]) {
		if (subdi <= 1)
		{
			subdi = 1;
		}
		else
		{
			subdi--;
		}
	}
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	//if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	//	glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	camYaw -= 0.1*xoffset;
	camPitch += 0.1*yoffset;

	GLfloat rotateMatric[16];
	oglRotateEuler(camYaw, camPitch, 0, rotateMatric);

	oglVectexMatrixMul(rotateMatric, camForwardC, camForward);
	oglVectexMatrixMul(rotateMatric, camBackC, camBack);
	oglVectexMatrixMul(rotateMatric, camLeftC, camLeft);
	oglVectexMatrixMul(rotateMatric, camRightC, camRight);

	lastX = xpos;
	lastY = ypos;
}