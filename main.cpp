
/******************************************************************************
 ******************************* INCLUDE SECTION ******************************
 ******************************************************************************/

// STL
#include <iostream>
#include <vector>
#include <fstream>

// System
#include <cstdio>
#include <cmath>

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
#include <glm/gtx/rotate_vector.hpp>

//SOIL
#include "SOIL.h"

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "AssetLoader.h"
#include "Model3D.h"
#include "SkyBox.h"
#include "HeigthMap.h"
#include "Picking.h"



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

// VBO (vertex buffer object) : used to store positions coordinates at each point
std::vector<GLuint> positionBuffers;
// VBO (vertex buffer object) : used to store normales at each point
std::vector<GLuint> normalBuffers;
// VBO (vertex buffer object) : used to store positions index
std::vector<GLuint> indexBuffers;
std::vector<GLuint> textureCoordinateBuffers;
// VAO (vertex array object) : used to encapsulate several VBO
std::vector<GLuint> vertexArrays;



// Mesh
int numberOfVertices_;
int numberOfIndices_;

// Model3D
Model3D model;
//Texture
GLuint mModelShaderProgram;


//SkyBox
SkyBox CubeMap;

HeigthMap terrain;

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

float Speed = 0.1f;

float yaw = 0;
float pitch = 0;
float roll = 0;

glm::mat4 viewMatrix =glm::mat4(1.0f);

bool isMousePressed = false;
glm::vec2 mouseLastPosition;

// Mesh parameters
glm::vec3 _meshColor;
glm::vec3 _materialKd;
glm::vec3 _materialKs;
float _materialShininess;

// Light
glm::vec3 _lightPosition;
glm::vec3 _lightColor;

// Data directory
std::string dataRepository;

int meshSelect = -1;
int scaleMode = 0;
int rotateMode = 0;
int translateMode = 0;
float xAxe = 0, yAxe=0, zAxe=0;
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
            statusOK = CubeMap.initializeCubemap();
    }

    if ( statusOK )
    {
            statusOK = terrain.initializeHeigthMap();
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
    _cameraEye = glm::vec3( 0.f, 0.f, 1.f );
    _cameraCenter = glm::vec3( 0.f/* gauche en neg*/,0.f, 0.f /* up */);
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
* Initialize model shader
******************************************************************************/
bool initializeModelTShader(){
    bool statusOK = true;

    std::cout << "- initialize model shader program ..." << std::endl;

    mModelShaderProgram	= glCreateProgram();

    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    // Vertex shader
    const char* vertexShaderSource[] = {
         "#version 300 es                                   \n"
         "                                             \n"
        " // INPUT                                     \n"
        "layout (location = 0) in vec3 position; \n"
        "in vec2 textureCoord;\n"
        "in vec3 normal;                              \n"
        "                                             \n"
        " // UNIFORM                                   \n"
        " // - camera                                  \n"
        " uniform mat4 viewMatrix;                     \n"
        " uniform mat4 projectionMatrix;               \n"
        " // - 3D model                                \n"
        " uniform mat4 modelMatrix;                    \n"
        "                                             \n"
        " // OUTPUT                                    \n"
        " out vec3 pos;                                 \n"
        " out vec2 uv;\n"
        " out vec3 norm;\n"
        "                                             \n"
        " // MAIN                                      \n"
        "void main( void )                             \n"
        "{                                             \n"
        "    // Send position to Clip-space                                                     \n"
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( position, 1.0 ); \n"
        "	 pos = position;          \n"
        "    uv = textureCoord; \n"
        "    norm = normal;\n"
        "}                                                                                      \n"
    };


    // Fragment shader
    const char* fragmentShaderSource[] = {
         "#version 300 es                                 \n"
         "                                             \n"
        " // INPUT                                     \n"
        " in vec3 pos;                                             \n"
        " in vec2 uv;\n"
        " in vec3 norm;\n"
        "                                             \n"
        " // UNIFORM                                   \n"
        " uniform vec3 modelMeshColor;                      \n"
        "uniform sampler2D meshTexture;\n"

        "const vec3 diffuseColor = vec3(0.7,0.7,0.7);\n"
        "const vec3 ambientColor=vec3(0.05,0.05,0.05);\n"
        "const vec3 specularColor=vec3(1.0,1.0,1.0);\n"
        "                                             \n"
        " // OUTPUT                                    \n"
        "layout (location = 0) out vec4 fragmentColor;                     		\n"
        "                                             \n"
        " // MAIN                                      \n"
        "void main( void )                             \n"
        "{                                             \n"
        " vec3 normal = normalize(norm);\n"
        " vec4 color = texture(meshTexture,uv)*vec4(diffuseColor,1.0)*vec4(modelMeshColor,1.0);      \n"
        " fragmentColor = vec4( color.r, color.g, color.b, 1.0 );     	  \n"
        "}                                             \n"
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

    glAttachShader( mModelShaderProgram, vertexShader );
    glAttachShader( mModelShaderProgram, fragmentShader );

    glLinkProgram( mModelShaderProgram );

    // Check linking status
    GLint linkStatus = 0;
    glGetProgramiv( mModelShaderProgram, GL_LINK_STATUS, &linkStatus );
    if ( linkStatus == GL_FALSE )
    {
        // LOG
        // ...

        GLint logInfoLength = 0;
        glGetProgramiv( mModelShaderProgram, GL_INFO_LOG_LENGTH, &logInfoLength );
        if ( logInfoLength > 0 )
        {
        // Return information log for program object
            GLchar* infoLog = new GLchar[ logInfoLength ];
            GLsizei length = 0;
            glGetProgramInfoLog( mModelShaderProgram, logInfoLength, &length, infoLog );

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
 * Initialize array buffer
 ******************************************************************************/
bool initializeArrayBuffer()
{
    bool statusOK = true;

    std::cout << "Initialize array buffer..." << std::endl;
    positionBuffers.resize(model.nb_mesh);
    normalBuffers.resize(model.nb_mesh);
    indexBuffers.resize(model.nb_mesh);
    textureCoordinateBuffers.resize(model.nb_mesh);

    for(int i=0;i<model.nb_mesh;i++){
        // In this example, we want to display one triangle

        // Buffer of positions on CPU (host)
        std::vector< glm::vec3 > points;
        std::vector< glm::vec3 > normals;
        std::vector < glm::vec2 > textures;
        std::vector< GLuint > triangleIndices;
        //const int nb = 100;

        //Ici on prends les données #1
        //waves( points, normals, triangleIndices, nb );


        points = model.vertices[i];
        triangleIndices = model.indices[i];
        normals = model.normals[i];
        textures = model.textures[i];

        //Puis on envoie dans le VBO
        numberOfVertices_ = static_cast< int >( points.size() );
        numberOfIndices_ = static_cast< int >( triangleIndices.size() );
        std::cout << "vertices : " << numberOfVertices_ <<" indices : " << numberOfIndices_ << std::endl;

        // Position buffer
        glGenBuffers( 1, &positionBuffers[i] );
        // buffer courant a manipuler
        glBindBuffer( GL_ARRAY_BUFFER, positionBuffers[i] );
        // definit la taille du buffer et le remplit
        glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), points.data(), GL_STATIC_DRAW );
        // buffer courant : rien
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

          // Position buffer
        glGenBuffers( 1, &normalBuffers[i] );
        // buffer courant a manipuler
        glBindBuffer( GL_ARRAY_BUFFER, normalBuffers[i]);
        // definit la taille du buffer et le remplit
        glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), normals.data(), GL_STATIC_DRAW );
        // buffer courant : rien
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

        // Texture coordinates buffer
           glGenBuffers( 1, &textureCoordinateBuffers[i] );
           // buffer courant a manipuler
           glBindBuffer( GL_ARRAY_BUFFER, textureCoordinateBuffers[i] );
           // definit la taille du buffer et le remplit
           glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec2 ), textures.data(), GL_STATIC_DRAW );
           // buffer courant : rien
           glBindBuffer( GL_ARRAY_BUFFER, 0 );


        // Index buffer
        // - this buffer is used to separate topology from positions: send points + send toplogy (triangle: 3 vertex indices)
        glGenBuffers( 1, &indexBuffers[i] );
        // buffer courant a manipuler
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffers[i] );
        // definit la taille du buffer et le remplit
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, numberOfIndices_ * sizeof( GLuint ), triangleIndices.data(), GL_STATIC_DRAW );
        // buffer courant : rien
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

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

    vertexArrays.resize(model.nb_mesh);

    for(int i=0;i<model.nb_mesh;i++){
        // Create a vertex array to encapsulate all VBO
        // - generate a VAO ID
        glGenVertexArrays( 1, &vertexArrays[i] );

        // - bind VAO as current vertex array (in OpenGL state machine)
        glBindVertexArray( vertexArrays[i] );

        // - bind VBO as current buffer (in OpenGL state machine)
        glBindBuffer( GL_ARRAY_BUFFER, positionBuffers[i] );
        // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
        glVertexAttribPointer( 0/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
        // - enable or disable a generic vertex attribute array
        glEnableVertexAttribArray( 0/*index of the generic vertex attribute*/ );


        // - bind VBO as current buffer (in OpenGL state machine)
        glBindBuffer( GL_ARRAY_BUFFER, normalBuffers[i] );
        // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
        glVertexAttribPointer( 1/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
        // - enable or disable a generic vertex attribute array
        glEnableVertexAttribArray( 1/*index of the generic vertex attribute*/ );

        // - bind VBO as current buffer (in OpenGL state machine)
        glBindBuffer( GL_ARRAY_BUFFER, textureCoordinateBuffers[i] );
        // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
        glVertexAttribPointer( 2/*index of the generic vertex attribute: VBO index (not its ID!)*/, 2/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
        // - enable or disable a generic vertex attribute array
        glEnableVertexAttribArray( 2/*index of the generic vertex attribute*/ );


        // Index buffer
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffers[i] );

        // - unbind VAO (0 is the default resource ID in OpenGL)
        glBindVertexArray( 0 );
        // - unbind VBO (0 is the default resource ID in OpenGL)
        glBindBuffer( GL_ARRAY_BUFFER, 0 );

    }

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
                "layout (location = 2) in vec2 tex;     \n"
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
                "out vec2 uv;\n"
                "                                              \n"
                "// MAIN                                       \n"
                "void main( void )                             \n"
                "{                                             \n"
                //" vec4 diffuse_color = texture(diffuseTex,vec2(tex.s,1.f-tex.t));\n"
                " uv = tex;\n"
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
        "}                                                                                    \n"
    };

    // Fragment shader
    const char* fragmentShaderSource[] = {
        "#version 300 es                                  \n"
        "precision highp float;                           \n"
        "                                               \n"
        "                                               \n"
        "// INPUT                                       \n"
        "in vec4 vertexColor;                                 \n"
        "in vec2 uv;\n"
        "                                               \n"
        "// UNIFORM                                     \n"
        "uniform vec3 meshColor;                        \n"
        "uniform sampler2D diffuseTex; \n"
        "                                               \n"
        "// OUTPUT                                      \n"
        "layout( location = 0 ) out vec4 fragmentColor;     \n"
        "                                                   \n"
        "// MAIN                                        \n"
        "void main( void )                              \n"
        "{                                                  \n"
        //" vec4 diffuse_color = vec4(vec2(uv.s,1.f-uv.t),0,1);\n"
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

    //--------------------------------------------------------------------------------
    // START frame
    //--------------------------------------------------------------------------------
    // Clear the color buffer (of the main framebuffer)
    // - color used to clear
    glClearColor( 0.f, 0.f, 0.f, 0.f );
    glClearDepth( 1.f );
    // - clear the "color" framebuffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //--------------------------------------------------------------------------------
    // Camera
    //--------------------------------------------------------------------------------
    // Retrieve camera parameters
    const glm::mat4 projectionMatrix = glm::perspective( _cameraFovY, _cameraAspect, _cameraZNear, _cameraZFar );

    glm::mat4 matrixRoll  = glm::mat4(1.0f);
    glm::mat4 matrixPitch = glm::mat4(1.0f);
    glm::mat4 matrixYaw   = glm::mat4(1.0f);

    matrixRoll  = glm::rotate(matrixRoll,  roll,  glm::vec3(0.0f, 0.0f, 1.0f));
    matrixPitch = glm::rotate(matrixPitch, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    matrixYaw   = glm::rotate(matrixYaw,  yaw,    glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 rotate = matrixRoll * matrixPitch * matrixYaw;
    glm::mat4 translate = glm::mat4(1.0f);
    translate = glm::translate(translate, -_cameraEye);

    viewMatrix = rotate * translate;

    GLint uniformLocation;

    // Retrieve 3D model / scene parameters
    glm::mat4 modelMatrix;
    const bool useMeshAnimation = false; // TODO: use keyboard to activate/deactivate
    if ( useMeshAnimation )
    {
        modelMatrix = glm::rotate( modelMatrix, static_cast< float >( currentTime ) * 0.001f, glm::vec3( 0.0f, 1.f, 0.f ) );
    }


    //--------------------------------------------------------------------------------
    // Cubemap
    //--------------------------------------------------------------------------------

    // Activation de la cubemap
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap.texture );


    // Set shader program
    glUseProgram( CubeMap.mCubeMapShaderProgram );

    // Model view projection matrix
    uniformLocation = glGetUniformLocation( CubeMap.mCubeMapShaderProgram, "uModelViewProjectionMatrix" );
    if ( uniformLocation >= 0 )
    {
            glm::mat4 modelMatrix = glm::mat4( 1.f );
            modelMatrix = glm::scale( modelMatrix, glm::vec3( CubeMap.scale, CubeMap.scale, CubeMap.scale ) );
            glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
            glUniformMatrix4fv( uniformLocation, 1/*count*/, GL_FALSE/*transpose*/, glm::value_ptr( MVP ) );
    }

    uniformLocation = glGetUniformLocation( CubeMap.mCubeMapShaderProgram, "skybox" );
    if ( uniformLocation >= 0 )
    {
            glUniform1i(uniformLocation, 0);
    }

    // Modify GL state(s)
    // ...
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Draw command
    const GLsizei nbCubemapIndices = 6/*nb faces*/ * 2/*2 triangles per face*/ * 3/*nb indices per triangle*/;
    glBindVertexArray( CubeMap.mCubemapVertexArray );
    glDrawElements( GL_TRIANGLES/*mode*/, nbCubemapIndices/*count*/, GL_UNSIGNED_INT/*type*/, 0/*indices*/ );

    // Reset GL state(s)
    glUseProgram( 0 );
    glBindVertexArray( 0 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );


    //--------------------------------------------------------------------------------
    // Heigthmap
    //--------------------------------------------------------------------------------

    glUseProgram( terrain.mHeigthMapShaderProgram );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        //--------------------
        // Send uniforms to GPU
        //--------------------
        // Retrieve 3D model / scene parameters
        // Animation
        uniformLocation = glGetUniformLocation( terrain.mHeigthMapShaderProgram, "time" );
        //std::cout << uniformLocation << std::endl;
        if ( uniformLocation >= 0 )
        {
            glUniform1f( uniformLocation, static_cast< float >( currentTime ) );
        }

        uniformLocation = glGetUniformLocation( terrain.mHeigthMapShaderProgram, "viewMatrix" );
        if ( uniformLocation >= 0 )
        {
            glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( viewMatrix ) );
        }
        // - projection matrix
        uniformLocation = glGetUniformLocation( terrain.mHeigthMapShaderProgram, "projectionMatrix" );
        if ( uniformLocation >= 0 )
        {
            glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( projectionMatrix ) );
        }
        // Mesh
        // - model matrix
        uniformLocation = glGetUniformLocation( terrain.mHeigthMapShaderProgram, "modelMatrix" );
        if ( uniformLocation >= 0 )
        {
            glm::mat4 modelMatrix_heigth = glm::scale( modelMatrix, glm::vec3( CubeMap.scale, CubeMap.scale, CubeMap.scale ) );
            glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix_heigth ) );
        }
        // - normal matrix
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "normalMatrix" );
        if ( uniformLocation >= 0 )
        {
            glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( viewMatrix * modelMatrix ) ) );
            glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( normalMatrix ) );
        }
        // - mesh color
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "meshColor" );
        if ( uniformLocation >= 0 )
        {
            glUniform3fv( uniformLocation, 1, glm::value_ptr( _meshColor ) );
        }
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "materialKd" );
        if ( uniformLocation >= 0 )
        {
            _materialKd = glm::vec3( 0.f, 0.f, 1.f );
            glUniform3fv( uniformLocation, 1, glm::value_ptr( _materialKd ) );
        }
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "materialKs" );
        if ( uniformLocation >= 0 )
        {
            _materialKs = glm::vec3( 1.f, 1.f, 1.f );
            glUniform3fv( uniformLocation, 1, glm::value_ptr( _materialKs ) );
        }
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "materialShininess" );
        if ( uniformLocation >= 0 )
        {
            _materialShininess = 20.f;
            glUniform1f( uniformLocation, _materialShininess );
        }
        // - light
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "lightPosition" );
        if ( uniformLocation >= 0 )
        {
            _lightPosition = glm::vec3( 0.f, 10.f, 0.f );
            glUniform3fv( uniformLocation, 1, glm::value_ptr( _lightPosition ) );
        }
        // - light
        uniformLocation = glGetUniformLocation(  terrain.mHeigthMapShaderProgram, "lightColor" );
        if ( uniformLocation >= 0 )
        {
            _lightColor = glm::vec3( 1.f, 1.f, 1.f );
            glUniform3fv( uniformLocation, 1, glm::value_ptr( _lightColor ) );
        }

        //--------------------
        // Render scene
        //--------------------

        // Set GL state(s) (fixed pipeline)
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        // - bind VAO as current vertex array (in OpenGL state machine)
        glBindVertexArray( terrain.mHeigthMapVertexArray );

        /*glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, terrain.texture );*/
        // - draw command
        glDrawElements(
             GL_TRIANGLES,      // mode
             terrain.numberOfIndices_,  // count
             GL_UNSIGNED_INT,   // data type
             (void*)0           // element array buffer offset
        );

        // - unbind VAO (0 is the default resource ID in OpenGL)
        glBindVertexArray( 0 );

        // Reset GL state(s) (fixed pipeline)
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Deactivate current shader program
        glUseProgram( 0 );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);


    //--------------------------------------------------------------------------------
    // Activate shader program
    //--------------------------------------------------------------------------------
    for(int i=0;i<model.nb_mesh;i++){
        glUseProgram( shaderProgram );



        //--------------------------------------------------------------------------------
        // Send uniforms to GPU
        //--------------------------------------------------------------------------------

        // - view matrix
        uniformLocation = glGetUniformLocation( shaderProgram, "diffuseTex" );
        if ( uniformLocation >= 0 )
        {
            if(model.AllTexture[i][0].size() > 0){

                glUniform1i(uniformLocation, 0);
            }

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
            glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, glm::value_ptr( model.transform[i] ) );
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
            if(i==model.selectedModel)
                _materialKd = glm::vec3( 0.f, 1.f, 0.f );
            else
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

        //--------------------------------------------------------------------------------
        // Render scene
        //--------------------------------------------------------------------------------
        // Set GL state(s) (fixed pipeline)
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        // - bind VAO as current vertex array (in OpenGL state machine)
        glBindVertexArray( vertexArrays[i] );

        glActiveTexture(GL_TEXTURE0); // active proper texture unit before binding

        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, model.AllTexture[i][0][0].id);

        // - draw command
        glDrawElements(
             GL_TRIANGLES,      // mode
             model.indices[i].size(),  // count
             GL_UNSIGNED_INT,   // data type
             (void*)0           // element array buffer offset
            );
        // - unbind VAO (0 is the default resource ID in OpenGL)
        glBindVertexArray( 0 );
        // Reset GL state(s) (fixed pipeline)
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        // Deactivate current shader program
        glUseProgram( 0 );
    }


    //--------------------------------------------------------------------------------
    // END frame
    //--------------------------------------------------------------------------------
    // OpenGL commands are not synchrone, but asynchrone (stored in a "command buffer")
    glFlush();
    // Swap buffers for "double buffering" display mode (=> swap "back" and "front" framebuffers)
    glutSwapBuffers();
}


/******************************************************************************
 * Callback for KeyBoardEvent
 ******************************************************************************/
void keyboard_CB(unsigned char key, int x, int y)
{
    switch(key){

    case 'z':
        _cameraEye += -Speed * glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
    break;

    case 's':
        _cameraEye += Speed * glm::vec3(viewMatrix[0][2], viewMatrix[1][2], viewMatrix[2][2]);
    break;

    case 'q':
        _cameraEye += -Speed * glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        break;

    case 'd':
        _cameraEye += Speed * glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    break;

    case ' ':
        _cameraEye += Speed * glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    break;

    case 'x':
        _cameraEye += -Speed * glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    break;

    case '\t':
        if(model.nb_mesh-1 == meshSelect || meshSelect < 0){
            meshSelect = 0;
            model.setSelect(meshSelect);
        }else{
            meshSelect++;
            model.setSelect(meshSelect);
        }
        std::cout << "mesh "<< meshSelect << std::endl;
        break;


    case 'e':
        if(scaleMode==0 && meshSelect != -1){
            scaleMode=1;
            rotateMode =0;
            translateMode=0;
        }
        std::cout << "Mode Scale "<< meshSelect << std::endl;
        break;

    case 'r':
        if(rotateMode==0 && meshSelect != -1){
            scaleMode=0;
            rotateMode =1;
            translateMode=0;
        }
        std::cout << "Mode Rotate "<< meshSelect << std::endl;
        break;

    case 't':
        if(translateMode==0 && meshSelect != -1){
            scaleMode=0;
            rotateMode =0;
            translateMode=1;
        }
        std::cout << "Mode Translate " << std::endl;
        break;

    case 'i':
            if(xAxe == 0){
                xAxe = 1;
                std::cout << "Axe X actif " << std::endl;
            }else{
                xAxe= 0;
                std::cout << "Axe X desactif " << std::endl;
            }
        break;

    case 'o':
        if(yAxe == 0){
            yAxe = 1;
            std::cout << "Axe Y actif " << std::endl;
        }else{
            yAxe= 0;
            std::cout << "Axe Y desactif " << std::endl;
        }

        break;
    case 'p':
        if(zAxe == 0){
            zAxe = 1;
            std::cout << "Axe Z actif " << std::endl;
        }else{
            std::cout << "Axe Z desactif " << std::endl;
            zAxe= 0;
        }
        break;

    case '+':
        if(meshSelect != -1){
            glm::mat4 trans;
            if(translateMode == 1){
                trans = glm::translate(model.transform[model.selectedModel], glm::vec3(xAxe,yAxe,zAxe));
            }else if (rotateMode ==1){
                trans = glm::rotate(model.transform[model.selectedModel], glm::radians(10.f) ,glm::vec3(xAxe,yAxe,zAxe));
            }else if (scaleMode ==1){
                trans = glm::scale(model.transform[model.selectedModel], glm::vec3(1+xAxe/2,1+yAxe/2,1+zAxe/2));
            }
            model.transform[meshSelect] = trans;
        }
        break;

    case '-':
        if(meshSelect != -1){
            glm::mat4 trans;
            if(translateMode == 1){
                trans = glm::translate(model.transform[model.selectedModel], glm::vec3(-xAxe,-yAxe,-zAxe));
            }else if (rotateMode ==1){
                trans = glm::rotate(model.transform[model.selectedModel], glm::radians(-10.f) ,glm::vec3(xAxe,yAxe,zAxe));
            }else if (scaleMode ==1){
                trans = glm::scale(model.transform[model.selectedModel], glm::vec3(1-xAxe/2,1-yAxe/2,1-zAxe/2));
            }
            model.transform[meshSelect] = trans;
        }
        break;
    }


    glutPostRedisplay();
}



void special_CB(int key, int x, int y)
{
    switch (key) {

    case 100:
        std::cout << "Gauche"  << std::endl;
        break;

    case 101:
        std::cout << "top"  << std::endl;
        break;

    case 102:
        std::cout << "droite " << std::endl;
        break;

    case 103:
        std::cout << "bas "  << std::endl;
        break;

    default:
        std::cout << "key " << key << std::endl;
        break;
    }
}

void mouse_CB(int button, int state, int x, int y){
        if (state == GLUT_UP)
          isMousePressed = false;

    if((button==GLUT_LEFT_BUTTON)&&(state==GLUT_DOWN)){

        //Camera stuff
        isMousePressed = true;
        mouseLastPosition.x = x;
        mouseLastPosition.y = y;
    }
/*
        GLint m_viewport[4];

        glGetIntegerv( GL_VIEWPORT, m_viewport );
        std::cout << "click " << x << ":" << y << std::endl;

        glm::vec3 ray_origin;
        glm::vec3 ray_direction;
        const glm::mat4 viewMatrix = glm::lookAt( _cameraEye, _cameraCenter, _cameraUp );
        const glm::mat4 projectionMatrix = glm::perspective( _cameraFovY, _cameraAspect, _cameraZNear, _cameraZFar );
        Picking::ScreenPosToWorldRay(
            x, y,
            m_viewport[2], m_viewport[3],
            viewMatrix,
            projectionMatrix,
            ray_origin,
            ray_direction
        );
        bool isSelected=false;
        float previous_distance;
        for(int i=0; i<model.nb_mesh; i++){

            float intersection_distance; // Output of TestRayOBBIntersection()

            if ( Picking::TestRayOBBIntersection(
                ray_origin,
                ray_direction,
                model.aabb_min[i],
                model.aabb_max[i],
                model.transform[i],
                intersection_distance) && (!isSelected || intersection_distance < previous_distance)
            ){
                model.setSelect(i);
                std::cout << "mesh " << i << std::endl;
                previous_distance = intersection_distance;
                isSelected = true;
            }
        }
        if(!isSelected)
            model.setSelect(-1);
    }*/

    //ZOOM
    if(button==3){
        if(_cameraFovY >= 1.0f && _cameraFovY <= 45.0f)
          _cameraFovY -= 0.05;
        if(_cameraFovY <= 1.0f)
          _cameraFovY = 1.0f;
        if(_cameraFovY >= 45.0f)
          _cameraFovY = 45.0f;
    }else if(button==4){
        if(_cameraFovY >= 1.0f && _cameraFovY <= 45.0f)
          _cameraFovY += 0.05;
        if(_cameraFovY <= 1.0f)
          _cameraFovY = 1.0f;
        if(_cameraFovY >= 45.0f)
          _cameraFovY = 45.0f;
    }

    glutPostRedisplay();
}

void mouseMove(int x, int y){
    if (isMousePressed == false)
      return;

    glm::vec2 mouse_delta = glm::vec2(x, y) - mouseLastPosition;

    const float mouse_Sensitivity = 0.01f;

    yaw   += mouse_Sensitivity * mouse_delta.x;
    pitch += mouse_Sensitivity * mouse_delta.y;

    mouseLastPosition = glm::vec2(x, y);

    glutPostRedisplay();
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

    //Repository de la skyBox
    CubeMap.ImgRepository = dataRepository+"/../LMG_project/Map/";

    terrain.ImgRepository = dataRepository+"/../LMG_project/HeigthMap/chili.jpg";

    //Load le mesh 3D
    model.loadMesh(dataRepository+"/../LMG_project/Model3D/meute.obj");

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

    //Event KeyBoard and Mouse
    glutKeyboardFunc(keyboard_CB);
    glutSpecialFunc(special_CB);
    glutMouseFunc(mouse_CB);
    glutMotionFunc(mouseMove);
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
