#include "HeigthMap.h"

bool HeigthMap::initializeHeigthMap(){
    bool statusOK = true;

    std::cout << "Initialize Heigth map..." << std::endl;

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
        statusOK = initializeMaterial();
    }

    if ( statusOK )
    {
        statusOK = initializeShaderProgram();
    }

    return statusOK;
}

/******************************************************************************
 * Initialize array buffer
 ******************************************************************************/
bool HeigthMap::initializeArrayBuffer()
{
    bool statusOK = true;

    std::cout << "Initialize array buffer..." << std::endl;

    // In this example, we want to display one triangle

    // Buffer of positions on CPU (host)
    std::vector< glm::vec3 > points;
    std::vector< glm::vec2 > textureCoordinates;
    std::vector< GLuint > triangleIndices;

    // Positions
    points.push_back( glm::vec3( -1.f, -1.f,1 ) );
    points.push_back( glm::vec3( 1.f, -1.f,1 ) );
    points.push_back( glm::vec3( 1.f, -1.f,-1 ) );
    points.push_back( glm::vec3( -1.f,-1.f,-1 ) );

    // Texture coordinates
    textureCoordinates.push_back( glm::vec2(0.f,0.f));
    textureCoordinates.push_back( glm::vec2(1.f,0.f));
    textureCoordinates.push_back( glm::vec2(1.f,1.f));
    textureCoordinates.push_back( glm::vec2(0.f,1.f));

    // Indices
    // - 1st face
    triangleIndices.push_back( 0 );
    triangleIndices.push_back( 1 );
    triangleIndices.push_back( 2);
    // - 2nd face
    triangleIndices.push_back( 0 );
    triangleIndices.push_back( 2 );
    triangleIndices.push_back( 3 );

    // Store useful variables (GPU memory allocation, rendering, etc...)
    numberOfVertices_ = static_cast< int >( points.size() );
    numberOfIndices_ = static_cast< int >( triangleIndices.size() );

    // Allocate memory on device (i.e. GPU)

    // Position buffer
    glGenBuffers( 1, &mHeigthMapVertexBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapVertexBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), points.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Index buffer
    // - this buffer is used to separate topology from positions: send points + send toplogy (triangle: 3 vertex indices)
    glGenBuffers( 1, &mHeigthMapIndexBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mHeigthMapIndexBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, numberOfIndices_ * sizeof( GLuint ), triangleIndices.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // Texture coordinates buffer
    glGenBuffers( 1, &mHeigthMapTextureCoordinateBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapTextureCoordinateBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec2 ), textureCoordinates.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return statusOK;
}

/******************************************************************************
 * Initialize vertex array
 ******************************************************************************/
bool HeigthMap::initializeVertexArray()
{
    bool statusOK = true;

    std::cout << "Initialize vertex array..." << std::endl;

    // Create a vertex array to encapsulate all VBO
    // - generate a VAO ID
    glGenVertexArrays( 1, &mHeigthMapVertexArray );

    // - bind VAO as current vertex array (in OpenGL state machine)
    glBindVertexArray( mHeigthMapVertexArray );

    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapVertexBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    const GLuint positionBufferIndex = 0; // (checkout input "layout" in your shaders)
    glVertexAttribPointer( positionBufferIndex/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( positionBufferIndex/*index of the generic vertex attribute*/ );

    // Index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mHeigthMapIndexBuffer );

    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapTextureCoordinateBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    const GLuint textureBufferIndex = 1; // (checkout input "layout" in your shaders)
    glVertexAttribPointer( textureBufferIndex/*index of the generic vertex attribute: VBO index (not its ID!)*/, 2/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( textureBufferIndex/*index of the generic vertex attribute*/ );

    // - unbind VAO (0 is the default resource ID in OpenGL)
    glBindVertexArray( 0 );
    // - unbind VBO (0 is the default resource ID in OpenGL)
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return statusOK;
}

/******************************************************************************
* Initialize shader program
******************************************************************************/
bool HeigthMap::initializeMaterial()
{
    bool statusOK = true;

    int textureWidth;
    int textureHeight;
    std::cout << ImgRepository << std::endl;
    unsigned char* image = SOIL_load_image( ImgRepository.c_str(), &textureWidth,
    &textureHeight, 0, SOIL_LOAD_RGB );
    std::cout << "h : " << textureHeight << " w : " << textureWidth << std::endl;


    glGenTextures(1,&texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);

    // - Filetring: use linear interpolation
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    // - wrapping: many modes available (repeat, clam, mirrored_repeat...)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    #if 1
    glTexImage2D(GL_TEXTURE_2D/*target*/,
                    0/*level*/,
                    GL_RGB/*internal format*/,
                    textureWidth, textureHeight, // les dimensions de l’image lue
                    0/*border*/,
                    GL_RGB/*format*/,
                    GL_UNSIGNED_BYTE/*type*/,
                    image/*pixels => le contenu de l’image chargée*/);
    #endif
    SOIL_free_image_data(image);


    return statusOK;
}

/******************************************************************************
 * Initialize shader program
 ******************************************************************************/
bool HeigthMap::initializeShaderProgram()
{
    bool statusOK = true;

    std::cout << "Initialize shader program..." << std::endl;

    mHeigthMapShaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    // Vertex shader
    const char* vertexShaderSource[] = {
        "#version 300 es                             \n"
        //"#version 130                                  \n"
        "                                              \n"
        " // INPUT                                     \n"
        " // - vertex attributes                       \n"
        " in vec3 position;                            \n"
        " in vec2 textureCoordinate;                         \n"
        "                                              \n"
        " // UNIFORM                                   \n"
        " // - animation                               \n"
        " uniform float time;                          \n"
        "// UNIFORM                                    \n"
        "// - camera                                   \n"
        "uniform mat4 viewMatrix;                      \n"
        "uniform mat4 projectionMatrix;                \n"
        "// - 3D model                                 \n"
        "uniform mat4 modelMatrix;                     \n"
        "                                              \n"
        " // OUTPUT                                    \n"
        " out vec2 uv;                                 \n"
        "                                              \n"
        " // MAIN                                      \n"
        "void main( void )                             \n"
        "{                                             \n"
        "    // Send position to Clip-space            \n"
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( position, 1.0 ); \n"
        "    uv = textureCoordinate;                                          \n"
        "}                                             \n"
    };

    // Fragment shader
    const char* fragmentShaderSource[] = {
        "#version 300 es                             \n"
        "precision highp float;                           \n"
        "                                              \n"
        " // INPUT                                     \n"
        " in vec2 uv;                                  \n"
        "                                              \n"
        " // UNIFORM                                   \n"
        " // - mesh                                    \n"
        " uniform vec3 meshColor;                      \n"
        " // - animation                               \n"
        " uniform float time;                          \n"
        " // - material 							   \n"
        " uniform sampler2D meshTexture;			   \n"
        "                                              \n"
        " // OUTPUT                                    \n"
        " out vec4 fragmentColor;                      \n"
        "                                              \n"
        " // MAIN                                      \n"
        "void main( void )                             \n"
        "{                                             \n"
        "    vec4 color = texture(meshTexture,vec2(uv.s,1.0-uv.t));                  \n"
        "    // Use animation                          \n"
        "    fragmentColor = color;   \n"
        "    gl_FragCoord.y = uv.s;   \n"
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

    glAttachShader( mHeigthMapShaderProgram, vertexShader );
    glAttachShader( mHeigthMapShaderProgram, fragmentShader );

    glLinkProgram( mHeigthMapShaderProgram );

    return statusOK;
}
