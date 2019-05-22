/*
HERNÁNDEZ PADILLA MARISOL MONSERRAT
HERNÁNDEZ VANEGAS ALEJANDRA ABIGAIL
PROYECTO: FERIA DE PUEBLO 2019-2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_texturizado.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include"Skybox.h"
#include"SpotLight.h"


extern GLfloat RUEDA_ROTATE;
extern GLfloat KILA_ACTIVATE;
extern GLfloat CAM_ACTIVATE;

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera_aux;
Camera camera;

GLfloat aux = 0.0f;
extern GLfloat auxk;
GLfloat aumento = 0.05f;
GLfloat flag;

int save = 1;
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
int auxskybox;
float hora;
float intensidad;

Texture aceroTexture;
Texture maderaTexture;
Texture plainTexture;
Texture kilahueaTexture;
Texture MetalTexture;
Texture FireTexture;
Texture CueroTexture;
Texture PlasticTexture;
/*Texturas colores:*/
Texture rosaTexture;
Texture azulTexture;
Texture amarilloTexture;
Texture verdeTexture;
Texture Carpa1Texture;
Texture Carpa2Texture;
Texture Carpa3Texture;
Texture pajaTexture;


//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];

//para declarar varias luces de tipo spotlight
SpotLight spotLights[MAX_SPOT_LIGHTS];


//Modelos
Model WC_M;
Model CC_M;
Model Faro_M;
Model Coke_M;
Model Cono_M;
Model Candy_M;
Model Apple_M;
Model Lollipop_M;
Model Popcorn_M;
 
//Skybox
Skybox skyboxd;
Skybox skyboxat;
Skybox skyboxan;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

Sphere sp = Sphere(1.0, 20, 20);
Sphere bola = Sphere(1.0, 20, 20);
static double limitFPS = 1.0 / 60.0;

void inputKeyframes(bool* keys);

///////////////////////////////KEYFRAMES/////////////////////
bool animacion = false;

//NEW// Keyframes
float posXbola = 4.0, posYbola = -1.65, posZbola = 9.0;
float movbola_x = 0.0f, movbola_y = 0.0f;
float girobola = 0;

#define MAX_FRAMES 17
int i_max_steps = 90;
int i_curr_steps = 17;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movbola_x;		//Variable para PosicionX
	float movbola_y;		//Variable para PosicionY
	float movbola_xInc;		//Variable para IncrementoX
	float movbola_yInc;		//Variable para IncrementoY
	float girobola;
	float girobolaInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 17;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movbola_x = movbola_x;
	KeyFrame[FrameIndex].movbola_y = movbola_y;
	KeyFrame[FrameIndex].girobola;

	FrameIndex++;
}

void resetElements(void)
{
	movbola_x = KeyFrame[0].movbola_x;
	movbola_y = KeyFrame[0].movbola_y;
	girobola = KeyFrame[0].girobola;
}

void interpolation(void)
{
	KeyFrame[playIndex].movbola_xInc = (KeyFrame[playIndex + 1].movbola_x - KeyFrame[playIndex].movbola_x) / i_max_steps;
	KeyFrame[playIndex].movbola_yInc = (KeyFrame[playIndex + 1].movbola_y - KeyFrame[playIndex].movbola_y) / i_max_steps;
	KeyFrame[playIndex].girobolaInc = (KeyFrame[playIndex + 1].girobola - KeyFrame[playIndex].girobola) / i_max_steps;
}

void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			movbola_x += KeyFrame[playIndex].movbola_xInc;
			movbola_y += KeyFrame[playIndex].movbola_yInc;
			girobola += KeyFrame[playIndex].girobolaInc;
			i_curr_steps++;
		}
	}
}

/* FIN KEYFRAMES*/

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-15.0f, 0.0f, -15.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		15.0f, 0.0f, -15.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-15.0f, 0.0f, 15.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		15.0f, 0.0f, 15.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};

	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.0f,	0.5f,		0.0f,	0.0f,	-1.0f,	//0
		0.5f, -0.5f,  0.5f,		1.0f,	0.5f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f,  0.5f,  0.5f,		1.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2
		-0.5f,  0.5f,  0.5f,	0.0f,	1.0f,		0.0f,	0.0f,	-1.0f,	//3
																			// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.0f,	0.68f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	1.0f,	0.68f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	1.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.0f,	1.0f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	0.0f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		1.0f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	0.98f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f,	0.98f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	1.0f,   0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	1.0f,	1.0f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	1.0f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.27f,	0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  0.5f,	0.48f,  0.0f,		0.0f,	1.0f,	0.0f,
		0.5f,  -0.5f,  -0.5f,	0.48f,	0.5f,		0.0f,	1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.27f,	0.5f,		0.0f,	1.0f,	0.0f,

		//UP
		//x		y		z		S		T
		-0.5f, 0.5f,  0.5f,	0.27f,	0.5f,		0.0f,	-1.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	0.48f,  0.5f,		0.0f,	-1.0f,	0.0f,
		0.5f, 0.5f,  -0.5f,	0.48f,	1.0f,		0.0f,	-1.0f,	0.0f,
		-0.5f, 0.5f,  -0.5f,	0.27f,	1.0f,		0.0f,	-1.0f,	0.0f,

	};

	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}

void piramideCuad()
{
	unsigned int pC_indices[] = {
		// front
		0, 1, 2,

		// right
		3, 4, 5,

		// back
		6, 7, 8,

		// left
		9, 10, 11,

		// bottom
		12, 13, 14,
		14, 15, 12,
	};

	GLfloat pC_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		0.0f,  0.5f,  0.0f,	0.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//0
		-0.5f, -0.5f,  0.5f,	1.0f,	0.0f,		0.0f,	0.0f,	-1.0f,	//1
		0.5f, -0.5f,  0.5f,	0.5f,	1.0f,		0.0f,	0.0f,	-1.0f,	//2

		// right
		//x		y		z		S		T
		0.0f,  0.5f,  0.0f,	    0.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//3
		0.5f, -0.5f,  0.5f,		1.0f,	0.0f,		-1.0f,	0.0f,	0.0f,	//4
		0.5f, -0.5f, -0.5f,		0.5f,	1.0f,		-1.0f,	0.0f,	0.0f,	//5

		// back
		0.0f,  0.5f,  0.0f,	0.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//6
		0.5f, -0.5f, -0.5f,	1.0f,	0.0f,		0.0f,	0.0f,	1.0f,	//7
		-0.5f, -0.5f, -0.5f,	0.5f,	1.0f,		0.0f,	0.0f,	1.0f,	//8

		// left
		//x		y		z		S		T
		0.0f,  0.5f,  0.0f,	0.0f,	0.0f,		1.0f,	0.0f,	0.0f,	//9
		-0.5f, -0.5f, -0.5f,	1.0f,   0.0f,		1.0f,	0.0f,	0.0f,	//10
		-0.5f, -0.5f,  0.5f,	0.5f,	1.0f,		1.0f,	0.0f,	0.0f,	//11

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.0f,	0.0f,		0.0f,	1.0f,	0.0f,	//12
		0.5f, -0.5f,  0.5f,	1.0f,   0.0f,		0.0f,	1.0f,	0.0f,	//13
		0.5f, -0.5f, -0.5f,	1.0f,	1.0f,		0.0f,	1.0f,	0.0f,	//14
		-0.5f, -0.5f, -0.5f,	0.0f,	1.0f,		0.0f,	1.0f,	0.0f,	//15

	};

	Mesh *pCuad = new Mesh();
	pCuad->CreateMesh(pC_vertices, pC_indices, 128, 18);
	meshList.push_back(pCuad);

}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearCubo();
	piramideCuad();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);
	camera_aux = camera;

	aceroTexture = Texture("Textures/acero.tga");
	aceroTexture.LoadTextureA();
	maderaTexture = Texture("Textures/madera.tga");
	maderaTexture.LoadTextureA();
	plainTexture = Texture("Textures/arena.tga");
	plainTexture.LoadTextureA();
	kilahueaTexture = Texture("Textures/kilahuea2.tga");
	kilahueaTexture.LoadTextureA();
	MetalTexture = Texture("Textures/Metal.tga");
	MetalTexture.LoadTextureA();
	FireTexture = Texture("Textures/Fire.tga");
	FireTexture.LoadTextureA();
	CueroTexture = Texture("Textures/Cuero.tga");
	CueroTexture.LoadTextureA();
	PlasticTexture = Texture("Textures/plastic.tga");
	PlasticTexture.LoadTextureA();
	rosaTexture = Texture("Textures/rosa.tga");
	rosaTexture.LoadTextureA();
	azulTexture = Texture("Textures/azul.tga");
	azulTexture.LoadTextureA();
	amarilloTexture = Texture("Textures/amarillo.tga");
	amarilloTexture.LoadTextureA();
	verdeTexture = Texture("Textures/verde.tga");
	verdeTexture.LoadTextureA();
	Carpa1Texture = Texture("Textures/carpa.tga");
	Carpa1Texture.LoadTextureA();
	Carpa2Texture = Texture("Textures/carpa2.tga");
	Carpa2Texture.LoadTextureA();
	Carpa3Texture = Texture("Textures/carpa3.tga");
	Carpa3Texture.LoadTextureA();
	pajaTexture = Texture("Textures/paja.tga");
	pajaTexture.LoadTextureA();

	
	//Modelos
	WC_M = Model();
	WC_M.LoadModel("Models/WC.obj");
	CC_M = Model();
	CC_M.LoadModel("Models/CC.obj");
	Faro_M = Model();
	Faro_M.LoadModel("Models/faro.obj");
	Coke_M = Model();
	Coke_M.LoadModel("Models/coke.obj");
	Cono_M = Model();
	Cono_M.LoadModel("Models/cono.obj");
	Candy_M = Model();
	Candy_M.LoadModel("Models/dulces.obj");
	Apple_M = Model();
	Apple_M.LoadModel("Models/manzana.obj");
	Lollipop_M = Model();
	Lollipop_M.LoadModel("Models/refripa.obj");
	Popcorn_M = Model();
	Popcorn_M.LoadModel("Models/popcorn.obj");

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
								 0.3f, 0.3f,
								 0.0f, 0.0f, -1.0f);
	

	//Skybox día
	std::vector<std::string> skyboxFacesd;
	skyboxFacesd.push_back("Textures/Skybox/sorbin_rt.tga");
	skyboxFacesd.push_back("Textures/Skybox/sorbin_lf.tga");
	skyboxFacesd.push_back("Textures/Skybox/sorbin_dn.tga");
	skyboxFacesd.push_back("Textures/Skybox/sorbin_up.tga");
	skyboxFacesd.push_back("Textures/Skybox/sorbin_bk.tga");
	skyboxFacesd.push_back("Textures/Skybox/sorbin_ft.tga");
	skyboxd = Skybox(skyboxFacesd);

	//Skybox atardecer
	std::vector<std::string> skyboxFacesat;
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_rt.tga");
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_lf.tga");
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_dn.tga");
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_up.tga");
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_bk.tga");
	skyboxFacesat.push_back("Textures/Skybox/sorbinat_ft.tga");
	skyboxat = Skybox(skyboxFacesat);

	//Skybox anochecer
	std::vector<std::string> skyboxFacesan;
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_rt.tga");
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_lf.tga");
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_dn.tga");
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_up.tga");
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_bk.tga");
	skyboxFacesan.push_back("Textures/Skybox/sorbinan_ft.tga");
	skyboxan = Skybox(skyboxFacesan);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	glm::vec3 posbola;

	//KEYFRAMES DECLARADOS INICIALES

	KeyFrame[0].movbola_x = 0.0f;
	KeyFrame[0].movbola_y = 0.0f;
	KeyFrame[0].girobola = 0;

	KeyFrame[1].movbola_x = 0.5f;
	KeyFrame[1].movbola_y = 0.25f;
	KeyFrame[1].girobola = 0;

	KeyFrame[2].movbola_x = 1.0f;
	KeyFrame[2].movbola_y = 0.0f;
	KeyFrame[2].girobola = 0;

	KeyFrame[3].movbola_x = 1.5f;
	KeyFrame[3].movbola_y = 0.25f;
	KeyFrame[3].girobola = 0;

	KeyFrame[4].movbola_x = 2.0f;
	KeyFrame[4].movbola_y = 0.0f;
	KeyFrame[4].girobola = 0.0f;

	KeyFrame[5].movbola_x = 2.5f;
	KeyFrame[5].movbola_y = 0.25f;
	KeyFrame[5].girobola = 0.0f;

	KeyFrame[6].movbola_x = 3.0f;
	KeyFrame[6].movbola_y = 0.0f;
	KeyFrame[6].girobola = 0;

	KeyFrame[7].movbola_x = 3.5f;
	KeyFrame[7].movbola_y = 0.25f;
	KeyFrame[7].girobola = 0;

	KeyFrame[8].movbola_x = 4.0f;
	KeyFrame[8].movbola_y = 0.0f;
	KeyFrame[8].girobola = 0;

	KeyFrame[16].movbola_x = 0.0f;
	KeyFrame[16].movbola_y = 0.0f;
	KeyFrame[16].girobola = 0;

	KeyFrame[15].movbola_x = 0.5f;
	KeyFrame[15].movbola_y = 0.25f;
	KeyFrame[15].girobola = 0;

	KeyFrame[14].movbola_x = 1.0f;
	KeyFrame[14].movbola_y = 0.0f;
	KeyFrame[14].girobola = 0;

	KeyFrame[13].movbola_x = 1.5f;
	KeyFrame[13].movbola_y = 0.25f;
	KeyFrame[13].girobola = 0;

	KeyFrame[12].movbola_x = 2.0f;
	KeyFrame[12].movbola_y = 0.0f;
	KeyFrame[12].girobola = 0.0f;

	KeyFrame[11].movbola_x = 2.5f;
	KeyFrame[11].movbola_y = 0.25f;
	KeyFrame[11].girobola = 0.0f;

	KeyFrame[10].movbola_x = 3.0f;
	KeyFrame[10].movbola_y = 0.0f;
	KeyFrame[10].girobola = 0;

	KeyFrame[9].movbola_x = 3.5f;
	KeyFrame[9].movbola_y = 0.25f;
	KeyFrame[9].girobola = 0;

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		//contador de luces puntuales y spotlight
		unsigned int pointLightCount = 0;
		unsigned int spotLightCount = 0;

		//Declaración de primer luz puntual
		//Faro uno
		pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
									intensidad, 1.0f,
									-12.5f, mainWindow.getfaro(), -11.8f,
									0.3f, 0.2f, 0.1f);
		pointLightCount++;

		//Faro dos
		pointLights[1] = PointLight(1.0f, 1.0f, 1.0f,
									intensidad, 1.0f,
									12.5f, mainWindow.getfaro(), -11.8f,
									0.3f, 0.2f, 0.1f);
		pointLightCount++;

		//Faro tres
		pointLights[2] = PointLight(1.0f, 1.0f, 1.0f,
									intensidad, 1.0f,
									0.0f, mainWindow.getfaro(), 9.0f,
									0.3f, 0.2f, 0.1f);
		pointLightCount++;

		//reflector
		spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f, //color
								  0.0f, 2.0f, //
								  0.0f, mainWindow.getreflector(), 0.0f, //posicion
								  0.0f, -15.0f, 0.0f, //direccion
								  1.0f, 0.0f, 0.0f, 
								  12.0f); //angulo de apertura
		spotLightCount++;

		//punta kilahuea 0.0f, 10.7f, 0.0f

		//Recibir eventos del usuario
		glfwPollEvents();

		while (auxskybox != 500000000)
		{
			auxskybox += 1;
		}
		if (auxskybox == 500000000)
		{
			auxskybox = 0;
			hora += 0.005;
			if (hora >= 24.0)
			{
				hora = 0.0;
			}
		}

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (hora >= 0.0 && hora <= 7.0)
		{
			skyboxd.DrawSkybox(camera.calculateViewMatrix(), projection);
			intensidad = 0;
		}
		else if (hora >= 7.0 && hora <= 20.0)
		{
			skyboxat.DrawSkybox(camera.calculateViewMatrix(), projection);
			intensidad += 0.001;
		}
		else if (hora >= 20.0 && hora <= 23.55)
		{
			skyboxan.DrawSkybox(camera.calculateViewMatrix(), projection);
			intensidad += 0.001;
		}

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		if (CAM_ACTIVATE == 0.0f && save == 0)
		{
			camera = camera_aux;
			save = 0;
		}
		else if (CAM_ACTIVATE == 1.0f && save == 0)
		{
			camera_aux = camera;
			save = 1;
		}
		else if (CAM_ACTIVATE == 2.0f && save == 0)
		{
			camera_aux = camera;
			save = 1;
		}
		else if (CAM_ACTIVATE == 3.0f && save == 0)
		{
			camera_aux = camera;
			save = 1;
		}
		else if (CAM_ACTIVATE == 4.0f && save == 0)
		{
			camera_aux = camera;
			save = 1;
		}

		/*Para el uso de esferas*/
		sp.init();
		bola.init();
		sp.load();
		bola.load();

		glm::mat4 model(1.0);
		/*--- Matrices Kilahuea ---*/
		glm::mat4 modelKila(1.0);
		glm::mat4 modelTemp2(1.0);
		glm::mat4 modelChair(1.0);
		glm::mat4 modelChairF(1.0);
		glm::mat4 modelChairT(1.0);
		glm::mat4 modelChairI(1.0);
		glm::mat4 modelChairD(1.0);
		glm::mat4 modelChairF2(1.0);
		glm::mat4 modelChairT2(1.0);
		glm::mat4 modelChairI2(1.0);
		glm::mat4 modelChairD2(1.0);
		glm::mat4 modelChairF3(1.0);
		glm::mat4 modelChairT3(1.0);
		glm::mat4 modelChairI3(1.0);
		glm::mat4 modelChairD3(1.0);
		glm::mat4 modelChairF31(1.0);
		glm::mat4 modelChairT31(1.0);
		glm::mat4 modelChairI31(1.0);
		glm::mat4 modelChairD31(1.0);
		glm::mat4 modelRotaKila(1.0);
		/*--- Matrices Rueda ---*/
		glm::mat4 modelRueda(1.0);
		glm::mat4 modelRotaRueda(1.0);
		glm::mat4 modelTemp(1.0);
		glm::mat4 eje1(1.0);
		glm::mat4 eje2(1.0);
		glm::mat4 eje3(1.0);
		glm::mat4 eje4(1.0);
		glm::mat4 eje5(1.0);
		glm::mat4 eje6(1.0);
		glm::mat4 eje7(1.0);
		glm::mat4 eje8(1.0);
		glm::mat4 RotaCanastilla(1.0);
		/*--- Matrices stands ---*/
		glm::mat4 modelA(1.0);
		glm::mat4 modelA2(1.0);
		glm::mat4 modelPaletas(1.0);
		glm::mat4 tempPaletas(1.0);
		glm::mat4 modelRefrescos(1.0);
		glm::mat4 tempRefrescos(1.0);
		glm::mat4 modelPalomitas(1.0);
		glm::mat4 tempPalomitas(1.0);

		/* ----		PLANO	---- */
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();

		/* ----		KILAHUEA	---- */
		
		//BASE DE KILAHUEA
		model = glm::mat4(1.0);
		modelKila = model = glm::translate(model, glm::vec3(4.5f, -1.85f, -11.8f));
		modelChair = model;
		modelTemp2 = model;
		model = glm::scale(model, glm::vec3(3.0f, 0.25f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		spotLights[2].SetPos(glm::vec3(0.0f, -1.5f, 0.0f));


		//ESTRUCTURA DEL KILAHUEA
		//PILAR 1:
		model = glm::translate(modelKila, glm::vec3(1.1f, 5.0f, 1.1f));
		model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//PILAR 2:
		model = glm::translate(modelKila, glm::vec3(1.1f, 5.0f, -1.1f));
		model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//PILAR 3:
		model = glm::translate(modelKila, glm::vec3(-1.1f, 5.0f, -1.1f));
		model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//PILAR 4:
		model = glm::translate(modelKila, glm::vec3(-1.1f, 5.0f, 1.1f));
		model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//UNIONES:
		/*------------------------------------------------------------------------------------------------------------*/
		model = glm::translate(modelKila, glm::vec3(0.0f, 1.0f, -1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 1.0f, -1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 3.0f, -1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 3.0f, -1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 5.0f, -1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 5.0f, -1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 7.0f, -1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 7.0f, -1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 9.0f, -1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 9.0f, -1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/*------------------------------------------------------------------------------------------------------------*/
		model = glm::translate(modelKila, glm::vec3(0.0f, 1.0f, 1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 1.0f, 1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 3.0f, 1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 3.0f, 1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 5.0f, 1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 5.0f, 1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 7.0f, 1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 7.0f, 1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 9.0f, 1.1f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(0.0f, 9.0f, 1.1f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(3.0f, 0.2f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/*------------------------------------------------------------------------------------------------------------*/
		model = glm::translate(modelKila, glm::vec3(-1.1f, 1.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 1.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 3.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 3.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 5.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 5.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 7.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 7.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 9.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(-1.1f, 9.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/*------------------------------------------------------------------------------------------------------------*/
		model = glm::translate(modelKila, glm::vec3(1.1f, 1.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 1.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 3.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 3.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 5.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 5.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 7.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 7.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 9.0f, 0.0f));
		model = glm::rotate(model, 40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelKila, glm::vec3(1.1f, 9.0f, 0.0f));
		model = glm::rotate(model, -40 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//PUNTA KILAHUEA:
		model = glm::translate(modelKila, glm::vec3(0.0f, 10.7f, 0.0f));
		model = glm::scale(model, glm::vec3(2.6f, 1.5f, 2.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//ASIENTOS KILAHUEA: 

		/* ROTACIÓN DEL KILAHUEA */
		modelRotaKila = glm::translate(modelTemp2, glm::vec3(0.0f, 1.13f, 0.0f));
		/*  T = mover
			G = parar */
		if (KILA_ACTIVATE == 1)
		{
			if (auxk > 8.0)
			{
				aumento = -0.05;
			}
			else if (auxk < 0.23)
			{
				aumento = 0.05;
			}
			auxk += aumento;
			/*else
			{
				auxk = 0.0;
			}*/
		}

		//Soporte de asiento delantero
		modelChairF = model = glm::translate(modelRotaKila, glm::vec3(0.0f, auxk, 1.27f));
		model = glm::scale(model, glm::vec3(2.47f, 2.0f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		FireTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Soporte de asiento trasero
		modelChairT = model = glm::translate(modelRotaKila, glm::vec3(0.0f, auxk, -1.27f));
		model = glm::scale(model, glm::vec3(2.47f, 2.0f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		FireTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Soporte de asiento izquierdo
		modelChairI = model = glm::translate(modelRotaKila, glm::vec3(-1.27f, auxk, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.43f, 2.0f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		FireTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Soporte de asiento derecho
		modelChairD = model = glm::translate(modelRotaKila, glm::vec3(1.27f, auxk, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.43f, 2.0f, 0.15f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		FireTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Respaldo asientos
		//delantero
		modelChairF2 = model = glm::translate(modelChairF, glm::vec3(0.0f, -0.233f, 0.15f));
		model = glm::scale(model, glm::vec3(2.3f, 1.2f, 0.154f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		CueroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//trasero
		modelChairT2 = model = glm::translate(modelChairT, glm::vec3(0.0f, -0.233f, -0.15f));
		model = glm::scale(model, glm::vec3(2.3f, 1.2f, 0.154f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		CueroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//izquierdo
		modelChairI2 = model = glm::translate(modelChairI, glm::vec3(-0.15f, -0.233f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 1.2f, 0.154f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		CueroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//derecho
		modelChairD2 = model = glm::translate(modelChairD, glm::vec3(0.15f, -0.233f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.3f, 1.2f, 0.154f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		CueroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base para asientos 
		//Frente
		modelChairF3 = model = glm::translate(modelChairF2, glm::vec3(0.6f, -0.573f, 0.18f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		modelChairF31 = model = glm::translate(modelChairF2, glm::vec3(-0.6f, -0.573f, 0.18f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Trasero
		modelChairT3 = model = glm::translate(modelChairT2, glm::vec3(0.6f, -0.573f, -0.18f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		modelChairT31 = model = glm::translate(modelChairT2, glm::vec3(-0.6f, -0.573f, -0.18f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Izquierdo
		modelChairI3 = model = glm::translate(modelChairI2, glm::vec3(-0.18f, -0.573f, 0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		modelChairI31 = model = glm::translate(modelChairI2, glm::vec3(-0.18f, -0.573f, -0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Derecho
		modelChairD3 = model = glm::translate(modelChairD2, glm::vec3(0.18f, -0.573f, 0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		modelChairD31 = model = glm::translate(modelChairD2, glm::vec3(0.18f, -0.573f, -0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Partes del asiento
		//Frente
		model = glm::translate(modelChairF3, glm::vec3(0.4f, 0.063f, 0.1f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF3, glm::vec3(-0.4f, 0.063f, 0.1f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF31, glm::vec3(0.4f, 0.063f, 0.1f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF31, glm::vec3(-0.4f, 0.063f, 0.1f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Trasero
		model = glm::translate(modelChairT3, glm::vec3(0.4f, 0.063f, -0.1f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT3, glm::vec3(-0.4f, 0.063f, -0.1f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT31, glm::vec3(0.4f, 0.063f, -0.1f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT31, glm::vec3(-0.4f, 0.063f, -0.1f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Izquierdo
		model = glm::translate(modelChairI3, glm::vec3(-0.1f, 0.063f, 0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI3, glm::vec3(-0.1f, 0.063f, -0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI31, glm::vec3(-0.1f, 0.063f, 0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI31, glm::vec3(-0.1f, 0.063f, -0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Derecho
		model = glm::translate(modelChairD3, glm::vec3(0.1f, 0.063f, 0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD3, glm::vec3(0.1f, 0.063f, -0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD31, glm::vec3(0.1f, 0.063f, 0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD31, glm::vec3(0.1f, 0.063f, -0.4f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.05f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		MetalTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Descansa Brazos
		//Frente
		model = glm::translate(modelChairF2, glm::vec3(0.99f, -0.3f, 0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(0.21f, -0.3f, 0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.21f, -0.3f, 0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.99f, -0.3f, 0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Trasero
		model = glm::translate(modelChairT2, glm::vec3(0.99f, -0.3f, -0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(0.21f, -0.3f, -0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.21f, -0.3f, -0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.99f, -0.3f, -0.3f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Izquierdo
		model = glm::translate(modelChairI2, glm::vec3(-0.3f, -0.3f, 0.99f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.3f, -0.3f, 0.21f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.3f, -0.3f, -0.21f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.3f, -0.3f, -0.99f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Derecho
		model = glm::translate(modelChairD2, glm::vec3(0.3f, -0.3f, 0.99f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.3f, -0.3f, 0.21f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.3f, -0.3f, -0.21f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.3f, -0.3f, -0.99f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Soportes del seguro
		//Frente (primer asiento a la derecha)
		model = glm::translate(modelChairF2, glm::vec3(0.775f, -0.161f, 0.25f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(0.6f, -0.28f, 0.4f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(0.425f, -0.161f, 0.25f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(0.775f, -0.0457f, 0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(0.425f, -0.0457f, 0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Frente (segundo asiento izquierdo)
		model = glm::translate(modelChairF2, glm::vec3(-0.775f, -0.161f, 0.25f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.6f, -0.28f, 0.4f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.425f, -0.161f, 0.25f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.775f, -0.0457f, 0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairF2, glm::vec3(-0.425f, -0.0457f, 0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Trasero
		//Trasero (primer asiento)
		model = glm::translate(modelChairT2, glm::vec3(0.775f, -0.161f, -0.25f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(0.6f, -0.28f, -0.4f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(0.425f, -0.161f, -0.25f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(0.775f, -0.0457f, -0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(0.425f, -0.0457f, -0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Trasero (segundo asiento)
		model = glm::translate(modelChairT2, glm::vec3(-0.775f, -0.161f, -0.25f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.6f, -0.28f, -0.4f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.425f, -0.161f, -0.25f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.775f, -0.0457f, -0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairT2, glm::vec3(-0.425f, -0.0457f, -0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Izquierdo (primer asiento)
		model = glm::translate(modelChairI2, glm::vec3(-0.25f, -0.161f, 0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.4f, -0.28f, 0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.25f, -0.161f, 0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.07f, -0.0457f, 0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.07f, -0.0457f, 0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Izquierdo (segundo asiento izquierdo)
		model = glm::translate(modelChairI2, glm::vec3(-0.25f, -0.161f, -0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.4f, -0.28f, -0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.25f, -0.161f, -0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.07f, -0.0457f, -0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairI2, glm::vec3(-0.07f, -0.0457f, -0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Derecho (primer asiento)
		model = glm::translate(modelChairD2, glm::vec3(0.25f, -0.161f, 0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.4f, -0.28f, 0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.25f, -0.161f, 0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.07f, -0.0457f, 0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.07f, -0.0457f, 0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Derecho (segundo asiento izquierdo)
		model = glm::translate(modelChairD2, glm::vec3(0.25f, -0.161f, -0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.4f, -0.28f, -0.6f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.25f, -0.161f, -0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -37 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.07f, -0.0457f, -0.775f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelChairD2, glm::vec3(0.07f, -0.0457f, -0.425f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.07f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		PlasticTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ---		RUEDA DE LA FORTUNA		---*/
		//BASE DE RUEDA DE LA FORTUNA
		model = glm::mat4(1.0);
		modelRueda = model = glm::translate(model, glm::vec3(-6.5f, -1.85f, -4.8f));
		model = glm::scale(model, glm::vec3(9.0f, 0.25f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//SOPORTES:
		model = glm::translate(modelRueda, glm::vec3(-1.5f, 2.5f, -0.3f));
		model = glm::rotate(model, 60 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(6.0f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelRueda, glm::vec3(1.5f, 2.5f, -0.3f));
		model = glm::rotate(model, -60 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(6.0f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ---		RUEDA DE LA FORTUNA		---*/
		//BASE DE RUEDA DE LA FORTUNA
		model = glm::mat4(1.0);
		modelRueda = model = glm::translate(model, glm::vec3(-6.5f, -1.85f, -4.8f));
		model = glm::scale(model, glm::vec3(9.0f, 0.25f, 3.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//SOPORTES:
		model = glm::translate(modelRueda, glm::vec3(-1.5f, 2.5f, -0.3f));
		model = glm::rotate(model, 60 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(6.0f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelRueda, glm::vec3(1.5f, 2.5f, -0.3f));
		model = glm::rotate(model, -60 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(6.0f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//CENTRO RUEDA
		modelTemp = model = glm::translate(modelRueda, glm::vec3(0.0f, 5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		sp.render();

		/* ROTACIÓN DE LA RUEDA */
		model = glm::translate(modelTemp, glm::vec3(0.0f, 0.0f, 0.0f));
		modelRotaRueda = model = glm::rotate(model, aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		/*  R = girar
		T = parar */
		if (RUEDA_ROTATE == 1) {
			if (aux < 360) {
				aux += 0.333;
			}
			else {
				aux = 0.0;
			}
		}
		// UNIONES RUEDA
		model = glm::translate(modelRotaRueda, glm::vec3(0.0f, -1.5f, 0.2f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(0.9f, -1.0f, 0.2f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(-0.9f, -1.0f, 0.2f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(1.2f, 0.0f, 0.2f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(-1.2f, 0.0f, 0.2f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(0.9f, 0.8f, 0.2f));
		model = glm::rotate(model, 135 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(-0.9f, 0.8f, 0.2f));
		model = glm::rotate(model, -135 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::translate(modelRotaRueda, glm::vec3(0.0f, 1.2f, 0.2f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.8f, 4.5f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		eje1 = model = glm::translate(modelRotaRueda, glm::vec3(1.2f, -3.1f, 0.2f));
		model = glm::rotate(model, 25 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje2 = model = glm::translate(modelRotaRueda, glm::vec3(-1.2f, -3.1f, 0.2f));
		model = glm::rotate(model, -25 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje3 = model = glm::translate(modelRotaRueda, glm::vec3(2.85f, -1.3f, 0.2f));
		model = glm::rotate(model, 70 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.1f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje4 = model = glm::translate(modelRotaRueda, glm::vec3(-2.85f, -1.3f, 0.2f));
		model = glm::rotate(model, -70 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.1f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje5 = model = glm::translate(modelRotaRueda, glm::vec3(2.85f, 1.2f, 0.2f));
		model = glm::rotate(model, -68 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje6 = model = glm::translate(modelRotaRueda, glm::vec3(-2.85f, 1.2f, 0.2f));
		model = glm::rotate(model, 68 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje7 = model = glm::translate(modelRotaRueda, glm::vec3(1.2f, 2.8f, 0.2f));
		model = glm::rotate(model, -23 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		eje8 = model = glm::translate(modelRotaRueda, glm::vec3(-1.2f, 2.8f, 0.2f));
		model = glm::rotate(model, 23 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(2.0f, 0.5f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//PRIMER CANASTILLA [RUEDA]
		//Eje1:
		eje1 = model = glm::translate(eje1, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE1 */
		model = glm::translate(eje1, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Rosa1
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//OCTAVA CANASTILLA [RUEDA]
		//Eje8:
		eje8 = model = glm::translate(eje8, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE8 */
		model = glm::translate(eje8, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Rosa2
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		rosaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//SEGUNDA CANASTILLA [RUEDA]
		//Eje2:
		eje2 = model = glm::translate(eje2, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE2 */
		model = glm::translate(eje2, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Azul1
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//SÉPTIMA CANASTILLA [RUEDA]
		//Eje7:
		eje7 = model = glm::translate(eje7, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE7 */
		model = glm::translate(eje7, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Azul2
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		azulTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//TERCER CANASTILLA [RUEDA]
		//Eje3:
		eje3 = model = glm::translate(eje3, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE3 */
		model = glm::translate(eje3, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Amarilla1
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//SEXTA CANASTILLA [RUEDA]
		//Eje6:
		eje6 = model = glm::translate(eje6, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE6 */
		model = glm::translate(eje6, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla Amarilla2
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		amarilloTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//CUARTA CANASTILLA [RUEDA]
		//Eje4:
		eje4 = model = glm::translate(eje4, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE4 */
		model = glm::translate(eje4, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla verde1
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//QUINTA CANASTILLA [RUEDA]
		//Eje5:
		eje5 = model = glm::translate(eje5, glm::vec3(0.0f, 0.0f, 0.25f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* ROTACIÓN DEL EJE5 */
		model = glm::translate(eje5, glm::vec3(0.0f, 0.0f, 0.0f));
		RotaCanastilla = model = glm::rotate(model, (-1) * aux * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//Canastilla verde2
		//Techo:
		modelTemp = model = glm::translate(RotaCanastilla, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f, 0.2f, 1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Soportes:
		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, 0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.6f, -0.5f, -0.6f));
		model = glm::scale(model, glm::vec3(0.05f, 0.8f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Base:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.25f, 0.0f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		//Asientos:
		model = glm::translate(modelTemp, glm::vec3(-0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.45f, -1.37f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.25f, 1.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Paredes:
		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, -0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.0f, -1.2f, 0.62f));
		model = glm::scale(model, glm::vec3(1.25f, 0.6f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(-0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(modelTemp, glm::vec3(0.62f, -1.2f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 0.6f, 1.25f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		verdeTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* --- ALGODONES DE AZÚCAR --- */ 
		//Base
		modelA2 = modelA = glm::translate(modelA, glm::vec3(0.0f, -1.9f, -9.0f));
		modelA = glm::scale(modelA, glm::vec3(1.0f, 0.15f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Palo:
		modelA = glm::translate(modelA2, glm::vec3(0.0f, 1.0f, 0.0f));
		modelA = glm::scale(modelA, glm::vec3(0.1f, 1.85f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelA));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		maderaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* --- STAND PALETAS --- */
		//Techo:
		tempPaletas = modelPaletas = glm::translate(modelPaletas, glm::vec3(8.0f, 1.05f, -3.0f));
		modelPaletas = glm::scale(modelPaletas, glm::vec3(3.5f, 1.0f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPaletas));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa1Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		
		//Paredes:
		model = glm::translate(tempPaletas, glm::vec3(0.0f, -1.75f, -1.75f));
		model = glm::scale(model, glm::vec3(3.5f, 2.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa1Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempPaletas, glm::vec3(-1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa1Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempPaletas, glm::vec3(1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa1Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//mesa:
		model = glm::translate(tempPaletas, glm::vec3(0.0f, -2.5f, 1.2f));
		model = glm::scale(model, glm::vec3(3.45f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* --- STAND REFRESCOS --- */
		//Techo:
		tempRefrescos = modelRefrescos = glm::translate(modelRefrescos, glm::vec3(-1.0f, 1.05f, 2.0f));
		modelRefrescos = glm::scale(modelRefrescos, glm::vec3(3.5f, 1.0f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelRefrescos));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//Paredes:
		model = glm::translate(tempRefrescos, glm::vec3(0.0f, -1.75f, -1.75f));
		model = glm::scale(model, glm::vec3(3.5f, 2.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempRefrescos, glm::vec3(-1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempRefrescos, glm::vec3(1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa2Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//mesa:
		model = glm::translate(tempRefrescos, glm::vec3(0.0f, -2.5f, 1.2f));
		model = glm::scale(model, glm::vec3(3.45f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		/* --- STAND PALOMITAS --- */
		//Techo:
		tempPalomitas = modelPalomitas = glm::translate(modelPalomitas, glm::vec3(-8.0f, 1.05f, 8.0f));
		modelPalomitas = glm::scale(modelPalomitas, glm::vec3(3.5f, 1.0f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPalomitas));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa3Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//Paredes:
		model = glm::translate(tempPalomitas, glm::vec3(0.0f, -1.75f, -1.75f));
		model = glm::scale(model, glm::vec3(3.5f, 2.5f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa3Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempPalomitas, glm::vec3(-1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa3Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		model = glm::translate(tempPalomitas, glm::vec3(1.75f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0f, 2.5f, 3.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		Carpa3Texture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//mesa:
		model = glm::translate(tempPalomitas, glm::vec3(0.0f, -2.5f, 1.2f));
		model = glm::scale(model, glm::vec3(3.45f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		aceroTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		//Modelo del WC
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(11.5f, -1.85f, 11.8f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		WC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.5f, -1.85f, -11.8f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		WC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-12.5f, -1.85f, -11.8f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Faro_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(12.5f, -1.85f, -11.8f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Faro_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.85f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Faro_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -0.2f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.05f, -0.4f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -0.6f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.05f, -0.8f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -1.0f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.05f, -1.2f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.15f, -1.4f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.05f, -1.6f, -9.08f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, 120 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		CC_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Coke_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.5f, -1.5f, -2.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -1.5f, -2.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.15f, -1.5f, -2.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.5f, -1.5f, -1.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -1.5f, -1.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(8.15f, -1.5f, -1.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.5f, -1.5f, -1.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.5f, -1.5f, -2.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Cono_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(7.1f, -0.08f, -1.7f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Candy_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.5f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.5f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.0f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-9.0f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-9.5f, -0.95f, 9.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-9.5f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-9.0f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.5f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-8.0f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.5f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-6.5f, -0.95f, 9.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Apple_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.0f, -2.3f, -3.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Lollipop_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.5f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.5f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.5f, -1.0f, 3.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Popcorn_M.RenderModel();

		//Bola de paja
		model = glm::mat4(1.0);
		posbola = glm::vec3(posXbola + movbola_x, posYbola + movbola_y, posZbola);
		model = glm::translate(model, posbola);
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, girobola * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pajaTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		bola.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 0;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;
			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movbola_x es: %f\n", movbola_x);
			printf("presiona K para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_K])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}

	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			movbola_x += 0.5f;
			printf("movbola_x es: %f\n", movbola_x);
			ciclo++;
			ciclo2 = 0;
			printf("reinicia con 2\n");
		}
	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
		}
	}
}