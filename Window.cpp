#include "Window.h"

extern GLfloat RUEDA_ROTATE = 0.0;
extern GLfloat KILA_ACTIVATE = 0.0;
extern GLfloat CAM_ACTIVATE = 0.0;

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	faro = -7.5f;
	reflector = 10.0f;

	//translate rueda de la fortuna
	//-6.5f, -1.85f, -4.8f
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	/*
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	*/

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas

							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		const char* key_name = glfwGetKeyName(GLFW_KEY_D, 0);
		printf("se presiono la tecla: %s\n", key_name);
	}

	/*ACTIVA Y DESACTIVA ROTACIÓN DE LA RUEDA*/
	if (key == GLFW_KEY_R)
	{
		RUEDA_ROTATE = 1.0;
	}
	if (key == GLFW_KEY_F)
	{
		RUEDA_ROTATE = 0.0;
	}

	/*ACTIVA Y DESACTIVA MOVIMIENTO DEL KILAHUEA*/
	if (key == GLFW_KEY_T)
	{
		KILA_ACTIVATE = 1.0;
	}
	if (key == GLFW_KEY_G)
	{
		KILA_ACTIVATE = 0.0;
	}

	/*ACTIVA LA CÁMARA*/
	if (key == GLFW_KEY_M)//Posición
	{
		CAM_ACTIVATE = 0.0;
	}
	if (key == GLFW_KEY_U)//Arriba
	{
		CAM_ACTIVATE = 1.0;
	}
	if (key == GLFW_KEY_V)//Rueda
	{
		CAM_ACTIVATE = 2.0;
	}
	if (key == GLFW_KEY_B)//Kila
	{
		CAM_ACTIVATE = 3.0;
	}
	if (key == GLFW_KEY_Y)//Kila juego
	{
		CAM_ACTIVATE = 4.0;
	}

	//ACTIVA LAS LUCES POINTLIGHT (PRENDE Y APAGA)
	if (key == GLFW_KEY_P)//Prender
	{
		theWindow->faro += 60.0f;
		theWindow->reflector += 60.0f;

	}
	if (key == GLFW_KEY_O)//Apagar
	{
		theWindow->faro -= 60.0f;
		theWindow->reflector-= 60.0f;


	}


	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
