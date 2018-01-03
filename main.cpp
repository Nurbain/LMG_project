
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <iostream>
#include <vector>
#include <fstream>

// System
#include <cstdio>

// Graphics
// - GLEW (always before "gl.h")
#include <GL/glew.h>
// - GL
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
// - GLUT
#include <GL/glut.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "AssetLoader.h"
#include "Model3D.h"
#include "SOIL.h"



/******************************************************************************
 ****************************** NAMESPACE SECTION *****************************
 ******************************************************************************/

/******************************************************************************
 ************************* DEFINE AND CONSTANT SECTION ************************
 ******************************************************************************/

// VBO (vertex buffer object) : used to store positions coordinates at each point
GLuint positionBuffer;
// VBO (vertex buffer object) : used to store normales at each point
GLuint normalBuffer;
// VBO (vertex buffer object) : used to store positions index
GLuint indexBuffer;
// VAO (vertex array object) : used to encapsulate several VBO
GLuint vertexArray;

// Mesh
int numberOfVertices_;
int numberOfIndices_;

Model3D model;

// Shader program
GLuint shaderProgram;

// Camera parameters
// - view
glm::vec3 _cameraEye;
glm::vec3 _cameraCenter;
glm::vec3 _cameraUp;
// - projection
float _cameraFovY;
float _cameraAspect;
float _cameraZNear;
float _cameraZFar;

// Mesh parameters
glm::vec3 _meshColor;
glm::vec3 _materialKd;
glm::vec3 _materialKs;
float _materialShininess;

// Light
glm::vec3 _lightPosition;
glm::vec3 _lightColor;


/**
 * Cube map
 */
// - mesh
GLuint mCubemapVertexArray;
GLuint mCubemapVertexBuffer;
GLuint mCubemapIndexBuffer;
// - shader
GLuint mCubeMapShaderProgram;
// - texture
GLuint texture;
// Data directory
std::string dataRepository;

/******************************************************************************
 ***************************** TYPE DEFINITION ********************************
 ******************************************************************************/

/******************************************************************************
***************************** METHOD DEFINITION ******************************
******************************************************************************/

/******************************************************************************
 ***************************** METHOD DEFINITION ******************************
 ******************************************************************************/

bool initialize();
bool checkExtensions();
bool initializeArrayBuffer();
bool initializeVertexArray();
bool initializeShaderProgram();
void initializeCamera();
bool finalize();

// Cubemap functions
bool initializeCubemap();
bool initializeCubemapGeometry();
bool initializeCubemapTextures();
bool initializeCubemapShader();

/******************************************************************************
 * Initialize cubemap
 ******************************************************************************/
bool initializeCubemap()
{
    bool statusOK = true;

    std::cout << "Initialize cubemap..." << std::endl;

    if ( statusOK )
    {
        statusOK = initializeCubemapGeometry();
    }

    if ( statusOK )
    {
        statusOK = initializeCubemapTextures();
    }

    if ( statusOK )
    {
        statusOK = initializeCubemapShader();
    }

    return statusOK;
}

/******************************************************************************
 * Initialize cubemap geometry
 ******************************************************************************/
bool initializeCubemapGeometry()
{
    bool result = true;

    std::cout << "- initialize geometry..." << std::endl;

    // Allocate GL resources
    glGenBuffers( 1, &mCubemapVertexBuffer );
    glGenBuffers( 1, &mCubemapIndexBuffer );
    glGenVertexArrays( 1, &mCubemapVertexArray );

    //----------------------------------------
    // Position buffer initialization
    // - geometry
    //----------------------------------------
    std::vector< GLfloat > vertexData =
    {
        // front vetices: z=1
        -1.f, -1.f, 1.f,
        1.f, -1.f, 1.f,
        1.f, 1.f, 1.f,
        -1.f, 1.f, 1.f,
        // rear vertices: z=-1
        -1.f, -1.f, -1.f,
        1.f, -1.f, -1.f,
        1.f, 1.f, -1.f,
        -1.f, 1.f, -1.f
    };
    glBindBuffer( GL_ARRAY_BUFFER, mCubemapVertexBuffer);
    glBufferData( GL_ARRAY_BUFFER, vertexData.size() * sizeof( GLfloat ), static_cast< const void* >( vertexData.data() ), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    //----------------------------------------
    // Index buffer initialization
    // - topology : faces
    //----------------------------------------
    std::vector< GLuint > indexData =
    {
        // Bottom face
        1, 0, 5,
        5, 4, 0,
        // Top face
        3, 2, 7,
        7, 6, 2,
        // Left face
        4, 0, 7,
        7, 3, 0,
        // Right face
        1, 5, 2,
        2, 6, 5,
        // Front face
        0, 1, 3,
        3, 2, 1,
        // Rear face
        5, 4, 6,
        6, 7, 4
    };
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mCubemapIndexBuffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof( GLuint ) , static_cast< const void* >( indexData.data() ), GL_STATIC_DRAW );

    // Reset GL state(s)
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    //----------------------------------------
    // Vertex array configuration
    // - main container of all vertex attributes
    //----------------------------------------
    glBindVertexArray( mCubemapVertexArray );
    // - position
    glEnableVertexAttribArray( 0/*attribute index*/ ); // same as in the shader program (see: "layout (location = 0)")
    glBindBuffer( GL_ARRAY_BUFFER, mCubemapVertexBuffer );
    glVertexAttribPointer( 0/*attribute index*/, 3/*nb components per vertex*/, GL_FLOAT/*type*/, GL_FALSE/*un-normalized*/, 0/*memory stride*/, static_cast< GLubyte* >( nullptr )/*byte offset from buffer*/ );
    // - required for indexed rendering (faces)
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mCubemapIndexBuffer );

    // Reset GL state(s)
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    return result;
}

/******************************************************************************
 * Initialize cubemap textures
 ******************************************************************************/
bool initializeCubemapTextures()
{
    bool result = true;

    //--------------------------------
    //--------------------------------
    //--------------------------------
    //return true;
    //--------------------------------
    //--------------------------------
    //--------------------------------

    std::cout << "-----  initialize textures..." << std::endl;

    // Initialize cubemap
    glGenTextures( 1, &texture );

    // Bind cubemap
    glActiveTexture( GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture );

    // Cubemap parameters (filtering, wrapping, etc...)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    // - wrapping: many modes available (repeat, clam, mirrored_repeat...)
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );


    // Set the 6 faces cubemap filenames !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // - be consistent with cubemap's internal OpenGL face ordering
    const std::string envmapRepository = dataRepository + std::string("/../LMG_project/Map/");
    std::cout << envmapRepository << std::endl;
    std::vector< std::string > envmapTextures( 6 );
    envmapTextures[ 0 ] = envmapRepository+"back.jpg";
    envmapTextures[ 1 ] = envmapRepository+"right.jpg";
    envmapTextures[ 2 ] = envmapRepository+"top.jpg";
    envmapTextures[ 3 ] = envmapRepository+"top.jpg";
    envmapTextures[ 4 ] = envmapRepository+"back.jpg";
    envmapTextures[ 5 ] = envmapRepository+"front.jpg";

    // Fille the cubemap texture
    // - load 6 faces individually with your image library (ex: SOIL) in textures
    // - then send data to GPU
    const GLint level = 0;
    const GLint internalFormat = GL_RGB;
    const GLint border = 0;
    const GLenum format = GL_RGB;
    const GLenum type = GL_UNSIGNED_BYTE;
    for ( size_t i = 0; i < 6; ++i )
    {
        // Load texture from file (and convert to RGB)
        int textureWidth;
        int textureHeight;

        const std::string& textureFilename = envmapTextures[ i ];
        std::cout << textureFilename.c_str() << std::endl;
        unsigned char* image = SOIL_load_image( textureFilename.c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB );
        //assert( image != nullptr );
        if(image == NULL){
            printf("----------------------- erreur chemin\n");
            exit(1);
        }
        // Upload data to device (GPU)
        const GLenum target =	GL_TEXTURE_CUBE_MAP_POSITIVE_X + i/*target*/;
        const GLsizei width = textureWidth;
        const GLsizei height = textureHeight;
        const GLvoid* pixels = static_cast< const GLvoid* >( image );
        glTexImage2D( target, level, internalFormat, width, height, border, format, type, pixels );

        // Free CPU memory
        SOIL_free_image_data( image );
    }

    // Reste GL state(s)
    // ...

    return result;
}

/******************************************************************************
* Initialize cubemap shader
******************************************************************************/
bool initializeCubemapShader()
{
    bool statusOK = true;

    std::cout << "- initialize shader program..." << std::endl;

    mCubeMapShaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    // Vertex shader
    const char* vertexShaderSource[] = {
    "#version 310 es                                                		   \n"
        "precision highp float;																								 \n"
        "// INPUT                                                              \n"
        "layout (location = 0) in vec3 position;                               \n"
        "                                                                      \n"
        "// UNIFORM                                                            \n"
        "uniform mat4 uModelViewProjectionMatrix;                              \n"
        "                                                                      \n"
        "// OUTPUT                                                             \n"
        "out vec3 pos;       		                                       	       \n"
        "// MAIN                                                               \n"
      "void main( void )                                                     \n"
    "{     																																 \n"
        " 		pos = position;																					 			   \n"
        "    // Send position to Clip-space                                    \n"
      "    gl_Position = uModelViewProjectionMatrix * vec4( position, 1.0 ); \n"
        "}                                                                     \n"
    };

    // Fragment shader
    const char* fragmentShaderSource[] = {
        "#version 310 es                                                		  \n"
        "precision highp float; 																							\n"
        "// INPUT                                                        			\n"
        "in vec3 pos;    		                                               		\n"
        "// UNIFORM                                                       	  \n"
        "uniform samplerCube skybox;              	                          \n"
        "// OUTPUT                                                          	\n"
        "layout (location = 0) out vec4 fragmentColor;                     		\n"
      "    	                                                                \n"
        "// MAIN                                                           		\n"
        "void main( void )                                                  	\n"
      "{                                                        	          \n"
      "    vec4 color =  texture(skybox,pos);                       				\n"
        "    fragmentColor = vec4( color.r, color.g, color.b, 1.0 );     	  	\n"
      "}                                                               		  \n"
    };

    // Load shader source
#if 1
    // Load from string
    glShaderSource( vertexShader, 1, vertexShaderSource, nullptr );
    glShaderSource( fragmentShader, 1, fragmentShaderSource, nullptr );
#else
    // TEST
    // Load from files
    const std::string vertexShaderFilename = "skyBox_vert.glsl";
    std::string vertexShaderFileContent;
    getFileContent( vertexShaderFilename, vertexShaderFileContent );
    const char* sourceCode = vertexShaderFileContent.c_str();
    glShaderSource( vertexShader, 1, &sourceCode, nullptr );
    glShaderSource( fragmentShader, 1, fragmentShaderSource, nullptr );
#endif

    glCompileShader( vertexShader );
    glCompileShader( fragmentShader );

    GLint compileStatus;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compileStatus );
    if ( compileStatus == GL_FALSE )
    {
        std::cout << "Error: vertex shader "<< std::endl;

        GLint logInfoLength = 0;
        glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetShaderInfoLog( vertexShader, logInfoLength, &length, infoLog );
            std::cout << infoLog << std::endl;
        }
    }

    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compileStatus );
    if ( compileStatus == GL_FALSE )
    {
        std::cout << "Error: fragment shader "<< std::endl;

        GLint logInfoLength = 0;
        glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetShaderInfoLog( fragmentShader, logInfoLength, &length, infoLog );
            std::cout << infoLog << std::endl;
        }
    }

    glAttachShader( mCubeMapShaderProgram, vertexShader );
    glAttachShader( mCubeMapShaderProgram, fragmentShader );

    glLinkProgram( mCubeMapShaderProgram );

    // Check linking status
    GLint linkStatus = 0;
    glGetProgramiv( mCubeMapShaderProgram, GL_LINK_STATUS, &linkStatus );
    if ( linkStatus == GL_FALSE )
    {
        // LOG
        // ...

        GLint logInfoLength = 0;
        glGetProgramiv( mCubeMapShaderProgram, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
            // Return information log for program object
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetProgramInfoLog( mCubeMapShaderProgram, logInfoLength, &length, infoLog );

            // LOG
            std::cout << "\nGsShaderProgram::link() - link ERROR" << std::endl;
            std::cout << infoLog << std::endl;

            delete[] infoLog;
        }

        return false;
    }

    return statusOK;
}

/******************************************************************************
 * Procdural mesh
 ******************************************************************************/
void waves( std::vector< glm::vec3 >& points, std::vector< glm::vec3 >& normals, std::vector< GLuint >& triangleIndices, int nb )
{
    // Position and normal arrays
    points.resize( nb * nb );
    normals.resize( nb * nb );
    for ( int j = 0; j < nb; ++j )
    {
        for ( int i = 0; i < nb; ++i )
        {
            // Current data index
            const int k = j * nb + i;

            // Current position
            float x = 6.0f / nb * j - 3.000001f;
            float y = 6.0f / nb * i - 3.000001f;

            // Position
            // analytic function: sinus (with altitude attenuation)
            // - altitude (use polar coordinates)
            const float r = std::sqrt( x * x + y * y );
//          const float h = 0.4f*std::sin(M_PI/2.0+r*7);
            const float h = 0.4f * ( 1 - r / 5 ) * std::sin( glm::pi< float >() / 2.0 + r * 5 );
            // - store position
//          points[ k ] = { x, y, h };
            points[ k ] = { x, h, y };

            // Normal
            // - derivative of analytic function (use polar coordinates)
            //const float dh = 7*0.4f*std::cos(M_PI/2.0+r*7);
            const float dh = -0.4 / 5 * std::sin( glm::pi< float >() / 2.0 + r * 5 ) + 0.4f * ( 1 - r / 5 ) * 5 * std::cos( glm::pi< float >() / 2.0 + r * 5 );
            // - derivative is the tangent, need to retrieve normal from tangent (easy in 2D)
            const glm::vec3 n = { -x / r * dh, -y / r * dh, 1 };
            // - store normal
            normals[ k ]= glm::normalize( n );
        }
    }

    // Index array
    triangleIndices.reserve( 6 * ( nb - 1 ) * ( nb - 1 ) );
    for ( int j = 1; j < nb; ++j )
        for ( int i = 1; i < nb; ++i )
        {
            // Current data index
            const int k = j * nb + i;
            // triangle
            triangleIndices.push_back( k );
            triangleIndices.push_back( k - nb );
            triangleIndices.push_back( k - nb - 1 );
            // triangle
            triangleIndices.push_back( k );
            triangleIndices.push_back( k - nb - 1 );
            triangleIndices.push_back( k - 1 );
        }
}

/******************************************************************************
 * Helper function used to load shader source code from files
 *
 * @param pFilename ...
 *
 * @return ...
 ******************************************************************************/
bool getFileContent( const std::string& pFilename, std::string& pFileContent )
{
    std::ifstream file( pFilename.c_str(), std::ios::in );
    if ( file )
    {
        // Initialize a string to store file content
        file.seekg( 0, std::ios::end );
        pFileContent.resize( file.tellg() );
        file.seekg( 0, std::ios::beg );

        // Read file content
        file.read( &pFileContent[ 0 ], pFileContent.size() );

        // Close file
        file.close();

        return true;
    }
    else
    {
        // LOG
        // ...
    }

    return false;
}

/******************************************************************************
 * Initialize all
 ******************************************************************************/
bool initialize()
{
    std::cout << "Initialize all..." << std::endl;

    bool statusOK = true;

    if ( statusOK )
    {
        statusOK = checkExtensions();
    }

    if ( statusOK )
    {
        statusOK = initializeArrayBuffer();
    }

    if ( statusOK )
    {
        statusOK = initializeVertexArray();
    }

    if ( statusOK )
    {
        statusOK = initializeShaderProgram();
    }

    if ( statusOK )
    {
            statusOK = initializeCubemap();
    }

    initializeCamera();

    return statusOK;
}

/******************************************************************************
 * Initialize the camera
 ******************************************************************************/
void initializeCamera()
{
    // User parameters
    // - view
    _cameraEye = glm::vec3( 0.f, 2.f, 3.f );
    _cameraCenter = glm::vec3( 0.f, 0.f, 0.f );
    _cameraUp = glm::vec3( 0.f, 1.f, 0.f );
    // - projection
    _cameraFovY = 45.f;
    _cameraAspect = 1.f;
    _cameraZNear = 0.1f;
    _cameraZFar = 100.f;
}

/******************************************************************************
 * Finalize all
 ******************************************************************************/
bool finalize()
{
    bool statusOK = true;

    std::cout << "Finalize all..." << std::endl;

    return statusOK;
}

/******************************************************************************
 * Check GL extensions
 ******************************************************************************/
bool checkExtensions()
{
    bool statusOK = true;

    std::cout << "Check GL extensions..." << std::endl;

    return statusOK;
}

/******************************************************************************
 * Initialize array buffer
 ******************************************************************************/
bool initializeArrayBuffer()
{
    bool statusOK = true;

    std::cout << "Initialize array buffer..." << std::endl;

    // In this example, we want to display one triangle

    // Buffer of positions on CPU (host)
    std::vector< glm::vec3 > points;
    std::vector< glm::vec3 > normals;
    std::vector< GLuint > triangleIndices;
    //const int nb = 100;

    //Ici on prends les données #1
    //waves( points, normals, triangleIndices, nb );

    points = model.vertices[0];
    triangleIndices = model.indices[0];
    normals = model.normals[0];

    //Puis on envoie dans le VBO
    numberOfVertices_ = static_cast< int >( points.size() );
    numberOfIndices_ = static_cast< int >( triangleIndices.size() );

    // Position buffer
    glGenBuffers( 1, &positionBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, positionBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), points.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

      // Position buffer
    glGenBuffers( 1, &normalBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, normalBuffer);
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), normals.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Index buffer
    // - this buffer is used to separate topology from positions: send points + send toplogy (triangle: 3 vertex indices)
    glGenBuffers( 1, &indexBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, numberOfIndices_ * sizeof( GLuint ), triangleIndices.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // Mesh parameter(s)
    _meshColor = glm::vec3( 0.f, 1.f, 0.f );

    return statusOK;
}

/******************************************************************************
 * Initialize vertex array
 ******************************************************************************/
bool initializeVertexArray()
{
    bool statusOK = true;

    std::cout << "Initialize vertex array..." << std::endl;

    // Create a vertex array to encapsulate all VBO
    // - generate a VAO ID
    glGenVertexArrays( 1, &vertexArray );

    // - bind VAO as current vertex array (in OpenGL state machine)
    glBindVertexArray( vertexArray );

    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, positionBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    glVertexAttribPointer( 0/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( 0/*index of the generic vertex attribute*/ );

    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, normalBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    glVertexAttribPointer( 1/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( 1/*index of the generic vertex attribute*/ );

    // Index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer );

    // - unbind VAO (0 is the default resource ID in OpenGL)
    glBindVertexArray( 0 );
    // - unbind VBO (0 is the default resource ID in OpenGL)
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return statusOK;
}

/******************************************************************************
 * Initialize shader program
 ******************************************************************************/
bool initializeShaderProgram()
{
    bool statusOK = true;

    std::cout << "Initialize shader program..." << std::endl;

    shaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    // Vertex shader
    const char* vertexShaderSource[] = {
        "#version 300 es                             \n"
        "                                              \n"
        "// INPUT                                      \n"
        "layout (location = 0) in vec3 position;     \n"
        "layout (location = 1) in vec3 normal;       \n"
        "                                              \n"
        "// UNIFORM                                    \n"
        "// - camera                                   \n"
        "uniform mat4 viewMatrix;                      \n"
        "uniform mat4 projectionMatrix;                \n"
        "// - 3D model                                 \n"
        "uniform mat4 modelMatrix;                     \n"
        "uniform mat3 normalMatrix;                    \n"
        "// - material                                 \n"
        "uniform vec3 materialKd;                      \n"
        "uniform vec3 materialKs;                      \n"
        "uniform float materialShininess;              \n"
        "// - light                                    \n"
        "uniform vec3 lightPosition;                   \n"
        "uniform vec3 lightColor;                      \n"
        "// - animation                               \n"
        "uniform float time;                          \n"
        "                                              \n"
        "// OUTPUT                                     \n"
        "out vec4 vertexColor;                               \n"
        "                                              \n"
        "// MAIN                                       \n"
        "void main( void )                             \n"
        "{                                             \n"
        "// Transform data to Eye-space, because this is the space where OpenGL does lighting traditionally \n"
        "// - vertex position                                \n"
        "    vec4 eyePosition = viewMatrix * modelMatrix * vec4( position, 1 );                 \n"
        "// - normal                                \n"
        "    vec3 eyeNormal = normalize( normalMatrix * normal );                               \n"
        "// - light position [already expressed in Object or World space : it depends of what you want]                    \n"
        //"    vec4 eyeLightPosition = viewMatrix * modelMatrix * vec4( lightPosition, 1 );       \n"
        "    vec4 eyeLightPosition = viewMatrix * vec4( lightPosition, 1 );       \n"
        "                                                                                       \n"
        "// Compute diffuse lighting coefficient                                                                                       \n"
        "// - light direction in Eye-space                                                                                       \n"
        "    vec3 L = normalize( eyeLightPosition.xyz - eyePosition.xyz );                      \n"
        "    float diffuse = max( 0.0, dot( eyeNormal, L ) );                                   \n"
        "    vertexColor = vec4( lightColor, 1.0 ) * vec4( materialKd, 1 ) * diffuse;                 \n"
        //"    vertexColor = vec4( lightColor, 1.0 ) * vec4( materialKd, 1 );                 \n"
        "                                                                                       \n"
        "#if 0                                                                                 \n"
        "    // Use animation                                                                   \n"
        "    float amplitude = 1.0;                                                             \n"
        "    float frequency = 0.5;                                                             \n"
        "    float height = amplitude * sin( 2.0 * 3.141592 * frequency * ( time * 0.001 ) );   \n"
        "    vec3 pos = vec3( position.x, position.y + height, position.z );                    \n"
        "    // Send position to Clip-space                                                     \n"
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( pos, 1.0 );      \n"
        "#else                                                                                  \n"
        "    // Send position to Clip-space                                                     \n"
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( position, 1.0 ); \n"
        "#endif                                                                                 \n"
        "}                                                                                      \n"
    };

    // Fragment shader
    const char* fragmentShaderSource[] = {
        "#version 300 es                                  \n"
        "precision highp float;                           \n"
        "                                               \n"
        "                                               \n"
        "// INPUT                                       \n"
        "in vec4 vertexColor;                                 \n"
        "                                               \n"
        "// UNIFORM                                     \n"
        "uniform vec3 meshColor;                        \n"
        "                                               \n"
        "// OUTPUT                                      \n"
        "layout( location = 0 ) out vec4 fragmentColor;     \n"
        "                                                   \n"
        "// MAIN                                        \n"
        "void main( void )                              \n"
        "{                                                  \n"
        "    fragmentColor = vertexColor;                     \n"
        "}                                                  \n"
    };

    // Load shader source
#if 1
    // Load from string
    glShaderSource( vertexShader, 1, vertexShaderSource, nullptr );
    glShaderSource( fragmentShader, 1, fragmentShaderSource, nullptr );
#else
    // TEST
    // Load from files
    const std::string vertexShaderFilename = "vertexShader.vert";
    std::string vertexShaderFileContent;
    getFileContent( vertexShaderFilename, vertexShaderFileContent );
    const char* sourceCode = vertexShaderFileContent.c_str();
    glShaderSource( vertexShader, 1, &sourceCode, nullptr );
    glShaderSource( fragmentShader, 1, fragmentShaderSource, nullptr );
#endif

    glCompileShader( vertexShader );
    glCompileShader( fragmentShader );

    GLint compileStatus;
    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &compileStatus );
    if ( compileStatus == GL_FALSE )
    {
        std::cout << "Error: vertex shader "<< std::endl;

        GLint logInfoLength = 0;
        glGetShaderiv( vertexShader, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetShaderInfoLog( vertexShader, logInfoLength, &length, infoLog );
            std::cout << infoLog << std::endl;
        }
    }

    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compileStatus );
    if ( compileStatus == GL_FALSE )
    {
        std::cout << "Error: fragment shader "<< std::endl;

        GLint logInfoLength = 0;
        glGetShaderiv( fragmentShader, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetShaderInfoLog( fragmentShader, logInfoLength, &length, infoLog );
            std::cout << infoLog << std::endl;
        }
    }

    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );

    glLinkProgram( shaderProgram );

    return statusOK;
}

/******************************************************************************
 * Callback to display the scene
 ******************************************************************************/
void display( void )
{
    // Timer info
    const int currentTime = glutGet( GLUT_ELAPSED_TIME );

    // Enable the Z-test in the OpenGL fixed pipeline
    glEnable( GL_DEPTH_TEST );

    //--------------------
    // START frame
    //--------------------
    // Clear the color buffer (of the main framebuffer)
    // - color used to clear
    glClearColor( 0.f, 0.f, 0.f, 0.f );
    glClearDepth( 1.f );
    // - clear the "color" framebuffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Retrieve camera parameters
    const glm::mat4 viewMatrix = glm::lookAt( _cameraEye, _cameraCenter, _cameraUp );
    const glm::mat4 projectionMatrix = glm::perspective( _cameraFovY, _cameraAspect, _cameraZNear, _cameraZFar );

    GLint uniformLocation;

    //--------------------------------------------------------------------------------
    // Cubemap
    //--------------------------------------------------------------------------------

    // Activation de la cubemap
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture );

    // Set shader program
    glUseProgram( mCubeMapShaderProgram );

    // Model view projection matrix
    uniformLocation = glGetUniformLocation( mCubeMapShaderProgram, "uModelViewProjectionMatrix" );
    if ( uniformLocation >= 0 )
    {
            glm::mat4 modelMatrix = glm::mat4( 1.f );
            const float scale = 5.f; // TODO: modify this to scale your cubemap size !!!!
            modelMatrix = glm::scale( modelMatrix, glm::vec3( scale, scale, scale ) );
            glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
            glUniformMatrix4fv( uniformLocation, 1/*count*/, GL_FALSE/*transpose*/, glm::value_ptr( MVP ) );
    }

    uniformLocation = glGetUniformLocation( mCubeMapShaderProgram, "skybox" );
    if ( uniformLocation >= 0 )
    {printf("ok\n");
            glUniform1i(uniformLocation, 0);
    }

    // Modify GL state(s)
    // ...
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Draw command
    const GLsizei nbCubemapIndices = 6/*nb faces*/ * 2/*2 triangles per face*/ * 3/*nb indices per triangle*/;
    glBindVertexArray( mCubemapVertexArray );
    glDrawElements( GL_TRIANGLES/*mode*/, nbCubemapIndices/*count*/, GL_UNSIGNED_INT/*type*/, 0/*indices*/ );

    // Reset GL state(s)
    glUseProgram( 0 );
    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );


    //--------------------
    // Activate shader program
    //--------------------
    glUseProgram( shaderProgram );

    //--------------------
    // Send uniforms to GPU
    //--------------------


    // Retrieve 3D model / scene parameters
    glm::mat4 modelMatrix;
    const bool useMeshAnimation = true; // TODO: use keyboard to activate/deactivate
    if ( useMeshAnimation )
    {
        modelMatrix = glm::rotate( modelMatrix, static_cast< float >( currentTime ) * 0.001f, glm::vec3( 0.0f, 1.f, 0.f ) );
    }
    // Camera
    // - view matrix
    uniformLocation = glGetUniformLocation( shaderProgram, "viewMatrix" );
    if ( uniformLocation >= 0 )
    {
        glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
    }
    // - projection matrix
    uniformLocation = glGetUniformLocation( shaderProgram, "projectionMatrix" );
    if ( uniformLocation >= 0 )
    {
        glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
    }
    // Mesh
    // - model matrix
    uniformLocation = glGetUniformLocation( shaderProgram, "modelMatrix" );
    if ( uniformLocation >= 0 )
    {
        glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( modelMatrix ) );
    }
    // - normal matrix
    uniformLocation = glGetUniformLocation( shaderProgram, "normalMatrix" );
    if ( uniformLocation >= 0 )
    {
        glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( viewMatrix * modelMatrix ) ) );
        glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
    }
    // - mesh color
    uniformLocation = glGetUniformLocation( shaderProgram, "meshColor" );
    if ( uniformLocation >= 0 )
    {
        glUniform3fv( uniformLocation, 1, glm::value_ptr( _meshColor ) );
    }
    uniformLocation = glGetUniformLocation( shaderProgram, "materialKd" );
    if ( uniformLocation >= 0 )
    {
        _materialKd = glm::vec3( 0.f, 0.f, 1.f );
        glUniform3fv( uniformLocation, 1, glm::value_ptr( _materialKd ) );
    }
    uniformLocation = glGetUniformLocation( shaderProgram, "materialKs" );
    if ( uniformLocation >= 0 )
    {
        _materialKs = glm::vec3( 1.f, 1.f, 1.f );
        glUniform3fv( uniformLocation, 1, glm::value_ptr( _materialKs ) );
    }
    uniformLocation = glGetUniformLocation( shaderProgram, "materialShininess" );
    if ( uniformLocation >= 0 )
    {
        _materialShininess = 20.f;
        glUniform1f( uniformLocation, _materialShininess );
    }
    // - light
    uniformLocation = glGetUniformLocation( shaderProgram, "lightPosition" );
    if ( uniformLocation >= 0 )
    {
        _lightPosition = glm::vec3( 0.f, 2.f, 3.f );
        glUniform3fv( uniformLocation, 1, glm::value_ptr( _lightPosition ) );
    }
    // - light
    uniformLocation = glGetUniformLocation( shaderProgram, "lightColor" );
    if ( uniformLocation >= 0 )
    {
        _lightColor = glm::vec3( 1.f, 1.f, 1.f );
        glUniform3fv( uniformLocation, 1, glm::value_ptr( _lightColor ) );
    }
    // Animation
    uniformLocation = glGetUniformLocation( shaderProgram, "time" );
    if ( uniformLocation >= 0 )
    {
        glUniform1f( uniformLocation, static_cast< float >( currentTime ) );
    }

    //--------------------
    // Render scene
    //--------------------
    // Set GL state(s) (fixed pipeline)
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // - bind VAO as current vertex array (in OpenGL state machine)
    glBindVertexArray( vertexArray );
    // - draw command
    glDrawElements(
         GL_TRIANGLES,      // mode
         numberOfIndices_,  // count
         GL_UNSIGNED_INT,   // data type
         (void*)0           // element array buffer offset
        );
    // - unbind VAO (0 is the default resource ID in OpenGL)
    glBindVertexArray( 0 );
    // Reset GL state(s) (fixed pipeline)
    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    // Deactivate current shader program
    glUseProgram( 0 );

    //--------------------
    // END frame
    //--------------------
    // OpenGL commands are not synchrone, but asynchrone (stored in a "command buffer")
    glFlush();
    // Swap buffers for "double buffering" display mode (=> swap "back" and "front" framebuffers)
    glutSwapBuffers();
}

/******************************************************************************
 * Callback continuously called when events are not being received
 ******************************************************************************/
void idle( void )
{
    // Mark current window as needing to be redisplayed
    glutPostRedisplay();
}

/******************************************************************************
 * Main function
 ******************************************************************************/
int main( int argc, char** argv )
{
    std::cout << "Projet LMG" << std::endl;
    std::string programPath = argv[ 0 ];
    std::size_t found = programPath.find_last_of( "/\\" );

    dataRepository = programPath.substr( 0, found );
    model.loadMesh(dataRepository+"/../LMG_project/PhotoExemple/tigre.obj");
    // Initialize the GLUT library
    glutInit( &argc, argv );

    //glutInitContextVersion( 3, 3 );
    //glutInitContextProfile( GLUT_COMPATIBILITY_PROFILE );

    // Grahics window
    // - configure the main framebuffer to store rgba colors,
    // - activate double buffering (for fluid/smooth visualization)
    // - add a depth buffer
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    // - window size and position
    glutInitWindowSize( 640, 480 );
    glutInitWindowPosition( 50, 50 );
    // - create the window
    glutCreateWindow( "Projet LMG" );

    // Callbacks
    // - callback called when displaying window (user custom fonction pointer: "void f( void )")
    glutDisplayFunc( display );
    // - callback continuously called when events are not being received
    glutIdleFunc( idle );

    // Initialize the GLEW library
    // - mandatory to be able to use OpenGL extensions,
    //   because OpenGL core API is made of OpenGL 1 and other functions are null pointers (=> segmentation fault !)
    //   Currently, official OpenGL version is 4.5 (or 4.6)
    GLenum error = glewInit();
    if ( error != GLEW_OK )
    {
        fprintf( stderr, "Error: %s\n", glewGetErrorString( error ) );
        exit( -1 );
    }

    // Initialize all your resources (graphics, data, etc...)
    initialize();

    // Enter the GLUT main event loop (waiting for events: keyboard, mouse, refresh screen, etc...)
    glutMainLoop();

    // Clean all
    //finalize();
}
