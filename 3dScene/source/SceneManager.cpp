///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager *pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	// initialize the texture collection
	for (int i = 0; i < 16; i++)
	{
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	// free the allocated objects
	m_pShaderManager = NULL;
	if (NULL != m_basicMeshes)
	{
		delete m_basicMeshes;
		m_basicMeshes = NULL;
	}

	// free the allocated OpenGL textures
	DestroyGLTextures();
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glGenTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(true);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

 /***********************************************************
  *  LoadSceneTextures()
  *
  *  This method is used for preparing the 3D scene by loading
  *  the shapes, textures in memory to support the 3D scene
  *  rendering
  ***********************************************************/
void SceneManager::LoadSceneTextures()
{
	
	bool loaded = false;
	loaded = CreateGLTexture("debug\\textures\\wood.jpg", "wood");
	if (!loaded) {
		std::cout << "Failed to load wood texture" << std::endl;
	}
	
	loaded = CreateGLTexture("debug\\textures\\ceramic.jpg", "ceramic");
	if (!loaded) {
		std::cout << "Failed to load ceramic texture" << std::endl;
	}
	loaded = CreateGLTexture("debug\\textures\\fabric.jpg", "fabric");
	if (!loaded) {
		std::cout << "Failed to load fabric texture" << std::endl;
	}
	loaded = CreateGLTexture("debug\\textures\\glass.jpg", "glass");
	if (!loaded) {
		std::cout << "Failed to load glass texture" << std::endl;
	}
	loaded = CreateGLTexture("debug\\textures\\wall.jpg", "wall");
	if (!loaded) {
		std::cout << "Failed to load glass2 texture" << std::endl;
	}



	// after the texture image data is loaded into memory, the
	// loaded textures need to be bound to texture slots - there
	// are a total of 16 available slots for scene textures
	BindGLTextures();
}
void SceneManager::DefineObjectMaterials()
{
// WOOD MATERIAL (for table)
OBJECT_MATERIAL woodMaterial;
woodMaterial.ambientColor = glm::vec3(0.3f, 0.25f, 0.2f);  // Warm brown ambient
woodMaterial.ambientStrength = 0.4f;                        // Higher ambient to show wood grain
woodMaterial.diffuseColor = glm::vec3(0.5f, 0.4f, 0.3f);    // Medium brown diffuse
woodMaterial.specularColor = glm::vec3(0.2f, 0.2f, 0.2f);  // Low specular
woodMaterial.shininess = 10.0f;                             // Soft highlights
woodMaterial.tag = "wood";
m_objectMaterials.push_back(woodMaterial);

// CERAMIC MATERIAL (for cup)
OBJECT_MATERIAL creamCeramic;
creamCeramic.ambientColor = glm::vec3(0.95f, 0.92f, 0.85f); // Warm cream ambient
creamCeramic.ambientStrength = 0.4f;                        // Slightly higher ambient
creamCeramic.diffuseColor = glm::vec3(0.96f, 0.93f, 0.86f); // Soft cream diffuse 
creamCeramic.specularColor = glm::vec3(0.9f, 0.9f, 0.88f);  // Bright but warm specular
creamCeramic.shininess = 96.0f;                             // Glossy but not mirror-like
creamCeramic.tag = "ceramic";
m_objectMaterials.push_back(creamCeramic);

// FABRIC MATERIAL (for chairs)
OBJECT_MATERIAL fabricMaterial;
fabricMaterial.ambientColor = glm::vec3(0.4f, 0.2f, 0.1f);
fabricMaterial.ambientStrength = 0.5f;
fabricMaterial.diffuseColor = glm::vec3(0.6f, 0.4f, 0.3f);
fabricMaterial.specularColor = glm::vec3(0.1f, 0.1f, 0.1f);
fabricMaterial.shininess = 5.0f;
fabricMaterial.tag = "fabric";
m_objectMaterials.push_back(fabricMaterial);

// METAL MATERIAL (for chandelier and chain)
OBJECT_MATERIAL metalMaterial;
metalMaterial.ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
metalMaterial.ambientStrength = 0.3f;
metalMaterial.diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
metalMaterial.specularColor = glm::vec3(0.8f, 0.8f, 0.8f);
metalMaterial.shininess = 128.0f;
metalMaterial.tag = "metal";
m_objectMaterials.push_back(metalMaterial);

// Add to DefineObjectMaterials()
OBJECT_MATERIAL glassMaterial;
glassMaterial.ambientColor = glm::vec3(0.8f, 0.8f, 0.8f);
glassMaterial.ambientStrength = 0.5f;
glassMaterial.diffuseColor = glm::vec3(0.9f, 0.9f, 0.9f);
glassMaterial.specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
glassMaterial.shininess = 128.0f;
glassMaterial.tag = "glass";
m_objectMaterials.push_back(glassMaterial);
}

/***********************************************************
 *  SetupSceneLights()
 *
 *  This method is called to add and configure the light
 *  sources for the 3D scene.  There are up to 4 light sources.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// this line of code is NEEDED for telling the shaders to render 
	// the 3D scene with custom lighting - to use the default rendered 
	// lighting then comment out the following line
	m_pShaderManager->setBoolValue(g_UseLightingName, true);
	// Main key light - bright and slightly to the right/front


	// SUN - Bright directional light simulating sunlight (coming from upper right)
	m_pShaderManager->setVec3Value("lightSources[0].position", 0.0f, 15.0f, 0.0f); // High and to the side
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 1.0f, 0.95f, 0.9f); // Slightly warm white
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.8f, 0.8f, 0.8f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 128.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.08f);

	// Key light - complements the sunlight (from opposite side)
	m_pShaderManager->setVec3Value("lightSources[1].position", 0.0f, 15.0f, 4.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.6f, 0.6f, 0.6f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.4f, 0.4f, 0.4f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 128.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.4f);

	// Fill light - soft ambient illumination
	m_pShaderManager->setVec3Value("lightSources[2].position", 0.0f, 7.0f, 0.0f);
	m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.5f, 0.5f, 0.5f);
	m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.3f, 0.3f, 0.3f);
	m_pShaderManager->setVec3Value("lightSources[2].specularColor", 0.1f, 0.1f, 0.1f);
	m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.2f);

	// Back light - rim lighting
	m_pShaderManager->setVec3Value("lightSources[3].position", 0.0f, 6.0f, -6.0f);
	m_pShaderManager->setVec3Value("lightSources[3].ambientColor", 0.05f, 0.05f, 0.05f);
	m_pShaderManager->setVec3Value("lightSources[3].diffuseColor", 0.4f, 0.4f, 0.4f);
	m_pShaderManager->setVec3Value("lightSources[3].specularColor", 0.2f, 0.2f, 0.2f);
	m_pShaderManager->setFloatValue("lightSources[3].focalStrength", 32.0f);
	m_pShaderManager->setFloatValue("lightSources[3].specularIntensity", 0.03f);
	}

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene 
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// only one instance of a particular mesh needs to be
	// loaded in memory no matter how many times it is drawn
	// in the rendered 3D scene

		// load the textures for the 3D scene
	LoadSceneTextures(); 
	// define the materials that will be used for the objects
// in the 3D scene
	DefineObjectMaterials();
	// add and define the light sources for the 3D scene
	SetupSceneLights();

	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();  
	m_basicMeshes->LoadTorusMesh();    
	m_basicMeshes->LoadSphereMesh();
	m_basicMeshes->LoadBoxMesh();  // For chair seats and table legs
	m_basicMeshes->LoadConeMesh(); // For chandelier
	
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by 
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Declare transformation variables
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

    // ------------------ TABLE LEGS ------------------

    
    // Front legs (closer to camera)
    scaleXYZ = glm::vec3(0.2f, 2.0f, 0.2f); // Thicker legs
    positionXYZ = glm::vec3(-2.0f, 0.0f, -2.0f); // Front left
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
    m_basicMeshes->DrawBoxMesh();

    positionXYZ = glm::vec3(2.0f, 0.0f, -2.0f); // Front right
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
    m_basicMeshes->DrawBoxMesh();

    // Back legs
    positionXYZ = glm::vec3(-2.0f, 0.0f, 2.0f); // Back left
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
    m_basicMeshes->DrawBoxMesh();

    positionXYZ = glm::vec3(2.0f, 0.0f, 2.0f); // Back right
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
    m_basicMeshes->DrawBoxMesh();

    // ------------------ TABLE TOP (RESTING ON LEGS) ------------------
    scaleXYZ = glm::vec3(5.5f, 0.2f, 4.5f); // Slightly larger than leg spread
    positionXYZ = glm::vec3(0.0f, 1.0f, 0.0f); // sits on top of legs
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
    m_basicMeshes->DrawBoxMesh();

    // ------------------ STOOLS ( ON NEAR SIDE) ------------------

    // Stool 1 (right)
    scaleXYZ = glm::vec3(0.8f, 1.2f, 0.8f);
    positionXYZ = glm::vec3(1.5f, -0.4f, 3.5f); // Near side, right
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	SetShaderTexture("fabric");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("fabric");
    m_basicMeshes->DrawBoxMesh();

    // Padded seat
    scaleXYZ = glm::vec3(0.9f, 0.15f, 0.9f);
    positionXYZ = glm::vec3(1.5f, 0.3f, 3.5f); // On top of stool
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	SetShaderTexture("fabric");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("fabric");
    m_basicMeshes->DrawBoxMesh();

    // Stool 2 (left)
    scaleXYZ = glm::vec3(0.8f, 1.2f, 0.8f);
    positionXYZ = glm::vec3(-1.5f, -0.4f, 3.5f); // Near side, left
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("fabric");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("fabric");
    m_basicMeshes->DrawBoxMesh();

    // Padded seat
    scaleXYZ = glm::vec3(0.9f, 0.15f, 0.9f);
    positionXYZ = glm::vec3(-1.5f, 0.3f, 3.5f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("fabric");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("fabric");
    m_basicMeshes->DrawBoxMesh();

    // ------------------ CUP ------------------
    
    // Cup base (torus)
    scaleXYZ = glm::vec3(0.3f, 0.3f, 0.3f);
    positionXYZ = glm::vec3(0.0f, 1.15f, 0.0f); // On table surface
    SetTransformations(scaleXYZ, 90.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("ceramic");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("ceramic");
    m_basicMeshes->DrawTorusMesh();

    // Cup body (cylinder)
    scaleXYZ = glm::vec3(0.35f, 0.5f, 0.35f);
    positionXYZ = glm::vec3(0.0f, 1.15f, 0.0f); // Above base
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("ceramic");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("ceramic");
    m_basicMeshes->DrawCylinderMesh();

    // Cup handle (torus)
    scaleXYZ = glm::vec3(0.3f, 0.2f, 0.3f);
    positionXYZ = glm::vec3(0.2f, 1.35f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("ceramic");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("ceramic");
    m_basicMeshes->DrawTorusMesh();

    // ------------------ CHANDELIER ------------------

    
    // Chain (cylinder)
    scaleXYZ = glm::vec3(0.05f, 1.0f, 0.1f);
    positionXYZ = glm::vec3(0.0f, 7.5f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("glass");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("ceramic");
    m_basicMeshes->DrawCylinderMesh();

    // Light (inverted cone)
    scaleXYZ = glm::vec3(1.0f, 0.8f, 1.0f);
    positionXYZ = glm::vec3(0.0f, 7.0f, 0.0f);
    SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wood");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("ceramic");
    m_basicMeshes->DrawConeMesh();



	// ------------------ FLOOR ------------------
	SetShaderColor(0.96f, 0.93f, 0.86f, 1.0f); // Cream color
	scaleXYZ = glm::vec3(20.0f, 0.1f, 20.0f);  // Large floor area
	positionXYZ = glm::vec3(0.0f, -0.1f, 0.0f); // Slightly below origin
	SetTransformations(scaleXYZ, 180.0f, 0.0f, 0.0f, positionXYZ);
	SetShaderTexture("wall");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawBoxMesh(); // Using box for thickness

	// ------------------ WALLS ------------------
	// Back wall
	SetShaderColor(0.96f, 0.93f, 0.86f, 1.0f);
	scaleXYZ = glm::vec3(20.0f, 10.0f, 0.1f);
	positionXYZ = glm::vec3(0.0f, 5.0f, -10.0f);
	SetTransformations(scaleXYZ, 0.0f, 180.0f, 0.0f, positionXYZ);
	SetShaderTexture("wall");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawBoxMesh();

	// Left wall
	scaleXYZ = glm::vec3(0.1f, 10.0f, 20.0f);
	positionXYZ = glm::vec3(-10.0f, 5.0f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 180.0f, positionXYZ);
	SetShaderTexture("wall");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawBoxMesh();

	// Right wall
	positionXYZ = glm::vec3(10.0f, 5.0f, 0.0f);
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 180.0f, positionXYZ);
	SetShaderTexture("wall");
	SetTextureUVScale(3.0f, 3.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawBoxMesh();

}
