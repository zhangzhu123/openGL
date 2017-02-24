#include <iostream>
#include <windows.h>
#include <ctime>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include "oglShader.h"
#include "oglTexture.h"
#include "oglModel.h"
#include "oglMath.h"

//tst
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void Do_Movement();

const GLuint WIDTH = 512, HEIGHT = 512;
bool keys[1024];

GLfloat camPosition[3] = { 0,0,5 };
GLfloat camForward[4]  = { 0,0,-0.005,0 };
GLfloat camBack[4]     = { 0,0,0.005 };
GLfloat camLeft[4]     = { -0.005,0,0 };
GLfloat camRight[4]    = { 0.005,0,0 };

const GLfloat camForwardC[4] = { 0,0,-0.1,0 };
const GLfloat camBackC[4]    = { 0,0,0.1 };
const GLfloat camLeftC[4]    = { -0.1,0,0 };
const GLfloat camRightC[4]   = { 0.1,0,0 };

GLfloat camYaw = 0;
GLfloat camPitch = 0;

bool firstMouse = true;
GLfloat lastX, lastY;

GLfloat pvmMatrix[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

int timeNum = 0;

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

void getEyeRay(GLfloat m[16], GLfloat x, GLfloat y, GLfloat eye[3], GLfloat(&ray)[3])
{
	GLfloat tempV[4] = { x, y, 0, 1 };
	GLfloat solve[4];
	oglVectexMatrixMul(m, tempV, solve);

	srand(timeNum);
	GLfloat xrNum = 2*(rand() % 1000 / 1000.0)-1;
	//timeNum += rand();
	//std::cout << "xrNum" << xrNum << std::endl;
	srand(timeNum);
	GLfloat yrNum = 2*(rand() % 1000 / 1000.0)-1;

	ray[0] = solve[0] - eye[0]; //+ xrNum*0.01;
	ray[1] = solve[1] - eye[1]; //+ yrNum*0.01;
	ray[2] = solve[2] - eye[2]; //+ yrNum*0.01;

	timeNum++;
	//ray[2] = 0.5;

	//ray[0] = tempV[0] - eye[0];
	//ray[1] = tempV[1] - eye[1];
	//ray[2] = tempV[2] - eye[2];
}

void getJitterMatrix(GLfloat(&m)[16])
{
	//srand(timeNum);
	//GLfloat xrNum = rand() % 1000 / 1000.0;
	//timeNum += rand();
	//std::cout << "xrNum" << xrNum << std::endl;
	//srand(timeNum);
	//GLfloat yrNum = rand() % 1000 / 1000.0;
	//timeNum += rand();
	//std::cout << "yrNum" << yrNum << std::endl;
	//GLfloat v[3] = { xrNum * 2 - 1, yrNum * 2 - 1, 0 };
	//v[0] = v[0] / 512.0;
	//v[1] = v[1] / 512.0;
	//v[2] = v[2] / 512.0;

	//GLfloat tm[16];
	//oglTranslate(v, tm);

	//GLfloat tm2[16];
	//oglMatrixMul(tm, pvmMatrix, tm2);

	//oglTranspositionMatrix(tm2, m);
	oglTranspositionMatrix(pvmMatrix, m);
	//timeNum++;
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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "ZZ", nullptr, nullptr);
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
	//int width, height;
	//glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, 512, 512);
	

	//Shader baseShader("E:/opengltst/oglDemo/pathTracing/shader/vertex.glsl", "E:/opengltst/oglDemo/pathTracing/shader/fragment.glsl");

	//Model baseModel;

	//Texture baseTexture("E:/opengltst/oglDemo/oglBases/oglBases/texture/soil.jpg");

	Shader pathTracingShader("E:/opengltst/oglDemo/pathTracing/shader/pathTracing.vs", "E:/opengltst/oglDemo/pathTracing/shader/pathTracing.frag");

#pragma region "post frame"
	Shader postShader("E:/opengltst/oglDemo/pathTracing/shader/postShader.vs", "E:/opengltst/oglDemo/pathTracing/shader/postShader.frag");

	GLfloat quadVertices[] = {
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	GLuint textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 512, 512, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//GLuint bakebuffer;
	//glGenFramebuffers(1, &bakebuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, bakebuffer);

	GLuint bakeColorbuffer;
	glGenTextures(1, &bakeColorbuffer);
	glBindTexture(GL_TEXTURE_2D, bakeColorbuffer);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 3000, 3000, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bakeColorbuffer, 0);

	//GLuint rbo;
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 3000, 3000);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

#pragma endregion	
    
	float sampleCount = 0.0;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		Do_Movement();

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, 3000, 3000);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pathTracingShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bakeColorbuffer);
		glUniform1i(glGetUniformLocation(pathTracingShader.program, "texture"), 0);

		////generate model matrix
		//GLfloat modelTM[16];
		//GLfloat tv[3] = { 0, 0, 0 };
		//oglTranslate(tv, modelTM);

		//GLfloat modelRM[16];
		//GLfloat axis[3] = { 0, 1, 0 };
		//GLfloat theta = 0;
		//oglRotateQuaternion(axis, theta, modelRM);

		//GLfloat modelSM[16];
		//GLfloat sv[3] = { 1, 1, 1 };
		//oglScale(sv, modelSM);

		//GLfloat modelMatric[16];
		//oglModelMatrix(modelTM, modelRM, modelSM, modelMatric);
		GLfloat modelTM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

		////generate view matrix
		//GLfloat viewTM[16];
		//GLfloat(&vmt)[3] = camPosition;
		//oglTranslate(vmt, viewTM);

		//GLfloat viewRM[16];
		//GLfloat &vmtheta = camYaw;
		//GLfloat &vmthetaP = camPitch;
		//oglRotateEuler(camYaw, camPitch, 0, viewRM);

		//GLfloat viewMatric[16];
		//oglViewMatrix(viewTM, viewRM, viewMatric);
		GLfloat viewTM[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

		//generate Perspective matrix
		GLfloat perspMatric[16];
		GLfloat fov = 60;
		GLfloat aspect = 1;
		GLfloat nearPlane = 0.001;
		GLfloat farPlane = 1000;

		oglPerspectiveMatrix(fov, aspect, nearPlane, farPlane, perspMatric);
		
		for (int i = 0; i < 16; i++)
		{
			pvmMatrix[i] = perspMatric[i];
		}


		GLfloat eye[3] = { 0, 0, 0 };
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "eye"), 1, eye);

		GLfloat light[3] = { 0.4, 0.5, -0.6 };
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "light"), 1, light);

		glUniform1f(glGetUniformLocation(pathTracingShader.program, "glossiness"), 0);

		GLfloat tm00[16];
		GLfloat ray00[3];
		getJitterMatrix(tm00);
		//for (int i = 0; i < 16; i++)
		//{
		//	std::cout << tm00[i] << " ";
		//}
		//std::cout << std::endl;
		getEyeRay(tm00, -1, -1, eye, ray00);
		//std::cout << "ray00: " << ray00[0] << ray00[1] << ray00[2] << std::endl;
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "ray00"), 1, ray00);

		GLfloat tm01[16];
		GLfloat ray01[3];
		getJitterMatrix(tm01);
		getEyeRay(tm01, -1, 1, eye, ray01);
		//std::cout << "ray01: " << ray01[0] << ray01[1] << ray01[2] << std::endl;
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "ray01"), 1, ray01);

		GLfloat tm10[16];
		GLfloat ray10[3];
		getJitterMatrix(tm10);
		getEyeRay(tm01, 1, -1, eye, ray10);
		//std::cout << "ray10: " << ray10[0] << ray10[1] << ray10[2] << std::endl;
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "ray10"), 1, ray10);

		GLfloat tm11[16];
		GLfloat ray11[3];
		getJitterMatrix(tm11);
		getEyeRay(tm01, 1, 1, eye, ray11);
		//std::cout << "ray11: " << ray11[0] << ray11[1] << ray11[2] << std::endl;
		glUniform3fv(glGetUniformLocation(pathTracingShader.program, "ray11"), 1, ray11);

		float textureWeight = sampleCount / (sampleCount + 1);
		//std::cout << "textureWeight: " << textureWeight<<std::endl;
		glUniform1f(glGetUniformLocation(pathTracingShader.program, "textureWeight"), textureWeight);

		GLfloat timeSinceStart = timeNum*0.001;
		glUniform1f(glGetUniformLocation(pathTracingShader.program, "timeSinceStart"), timeSinceStart);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, bakeColorbuffer);
		//glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 512, 512);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 0, 0, 512, 512, 0);
		glBindTexture(GL_TEXTURE_2D, 0);


		//// Render
		//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		////glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//glViewport(0, 0, 3000, 3000);

		//glEnable(GL_DEPTH_TEST);
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//baseShader.use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, baseTexture.texture);
		//glUniform1i(glGetUniformLocation(baseShader.program, "ourTexture1"), 0);

		////generate model matrix
		//GLfloat modelTM[16];
		//GLfloat tv[3] = { 0, 0, 0 };
		//oglTranslate(tv, modelTM);

		//GLfloat modelRM[16];
		//GLfloat axis[3] = { 0, 1, 0 };
		//GLfloat theta = 0;
		//oglRotateQuaternion(axis, theta, modelRM);

		//GLfloat modelSM[16];
		//GLfloat sv[3] = { 1, 1, 1 };
		//oglScale(sv, modelSM);

		//GLfloat modelMatric[16];
		//oglModelMatrix(modelTM, modelRM, modelSM, modelMatric);

		//GLint transformLoc = glGetUniformLocation(baseShader.program, "modelMatric");
		//glUniformMatrix4fv(transformLoc, 1, GL_FALSE, modelMatric);

		////generate view matrix
		//GLfloat viewTM[16];
		//GLfloat (&vmt)[3] = camPosition;
		//oglTranslate(vmt, viewTM);

		//GLfloat viewRM[16];
		//GLfloat &vmtheta = camYaw;
		//GLfloat &vmthetaP = camPitch;
		//oglRotateEuler(camYaw, camPitch,0, viewRM);

		//GLfloat viewMatric[16];
		//oglViewMatrix(viewTM, viewRM, viewMatric);
		//GLint viewMatricLoc = glGetUniformLocation(baseShader.program, "viewMatrix");
		//glUniformMatrix4fv(viewMatricLoc, 1, GL_FALSE, viewMatric);

		////generate Perspective matrix
		//GLfloat perspMatric[16];
		//GLfloat fov = 45;
		//GLfloat aspect = 1;
		//GLfloat nearPlane = 0.001;
		//GLfloat farPlane = 1000;

		//oglPerspectiveMatrix(fov, aspect, nearPlane, farPlane, perspMatric);
		//GLint perspMatricLoc = glGetUniformLocation(baseShader.program, "perspMatrix");
		//glUniformMatrix4fv(perspMatricLoc, 1, GL_FALSE, perspMatric);

		//// Draw our first triangle
		//
		//glBindVertexArray(baseModel.VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		////glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		//post frame
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		postShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		//glBindTexture(GL_TEXTURE_2D, bakeColorbuffer);
		glUniform1i(glGetUniformLocation(postShader.program, "screenTexture"), 0);
		//glUniform1f(glGetUniformLocation(postShader.program, "exposure"), 1);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		sampleCount++;
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