#include "HeigthMap.h"

void HeigthMap::plane( std::vector< glm::vec3 >& points,std::vector< glm::vec3 >& normals,std::vector< GLuint >& triangleIndices, int nb )
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

            int x_tex = (float)j/(float)nb * (this->textureWidth-1);
            int y_tex = (float)i/(float)nb * (this->textureHeight-1);

            // Position
            const float h = (float)image[x_tex+y_tex*(this->textureHeight-1)]/255.f -1;
            //const float h = -1;
            // - store position
            points[ k ] = { x, h, y };
        }
    }

    // Index array
    triangleIndices.reserve( 6 * ( nb - 1 ) * ( nb - 1 ) );
    for ( int j = 1; j < nb; ++j )
        for ( int i = 1; i < nb; ++i )
        {
            glm::vec3 normal_face;
            // Current data index
            const int k = j * nb + i;
            // triangle
            triangleIndices.push_back( k );
            triangleIndices.push_back( k - nb );
            triangleIndices.push_back( k - nb - 1 );

            normal_face = glm::normalize(glm::cross(points[k-nb-1]-points[k],points[k-nb]-points[k]));
            normals[k] += normal_face;
            normals[k-nb] += normal_face;
            normals[k-nb-1] += normal_face;

            // triangle
            triangleIndices.push_back( k );
            triangleIndices.push_back( k - nb - 1 );
            triangleIndices.push_back( k - 1 );

            normal_face = glm::normalize(glm::cross(points[k-1]-points[k],points[k-nb-1]-points[k]));
            normals[k] += normal_face;
            normals[k-1] += normal_face;
            normals[k-nb-1] += normal_face;
        }
    for ( int j = 1; j < nb; ++j )
        for ( int i = 1; i < nb; ++i )
        {
            const int k = j * nb + i;
            glm::normalize(normals[k]);
        }
}

bool HeigthMap::initializeHeigthMap(){
    bool statusOK = true;

    std::cout << "Initialize Heigth map..." << std::endl;

    if ( statusOK )
    {
        statusOK = initializeMaterial();
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
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > textureCoordinates;
    std::vector< GLuint > triangleIndices;

    plane(points,normals,triangleIndices,1081);

#if 0
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
#endif

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

    glGenBuffers( 1, &mHeigthMapNormalBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapNormalBuffer);
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec3 ), normals.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

#if 0
    // Texture coordinates buffer
    glGenBuffers( 1, &mHeigthMapTextureCoordinateBuffer );
    // buffer courant a manipuler
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapTextureCoordinateBuffer );
    // definit la taille du buffer et le remplit
    glBufferData( GL_ARRAY_BUFFER, numberOfVertices_ * sizeof( glm::vec2 ), textureCoordinates.data(), GL_STATIC_DRAW );
    // buffer courant : rien
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
#endif

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

    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapNormalBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    glVertexAttribPointer( 1/*index of the generic vertex attribute: VBO index (not its ID!)*/, 3/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( 1/*index of the generic vertex attribute*/ );

    // Index buffer
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mHeigthMapIndexBuffer );

#if 0
    // - bind VBO as current buffer (in OpenGL state machine)
    glBindBuffer( GL_ARRAY_BUFFER, mHeigthMapTextureCoordinateBuffer );
    // - specify the location and data format of the array of generic vertex attributes at index​ to use when rendering
    const GLuint textureBufferIndex = 1; // (checkout input "layout" in your shaders)
    glVertexAttribPointer( textureBufferIndex/*index of the generic vertex attribute: VBO index (not its ID!)*/, 2/*nb elements in the attribute: (x,y,z)*/, GL_FLOAT/*type of data*/, GL_FALSE/*normalize data*/, 0/*stride*/, 0/*offset in memory*/ );
    // - enable or disable a generic vertex attribute array
    glEnableVertexAttribArray( textureBufferIndex/*index of the generic vertex attribute*/ );
#endif
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


    std::cout << ImgRepository << std::endl;
    image = SOIL_load_image( ImgRepository.c_str(), &textureWidth,
    &textureHeight, 0, SOIL_LOAD_L );
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

    #if 0
    glTexImage2D(GL_TEXTURE_2D/*target*/,
                    0/*level*/,
                    GL_RGB/*internal format*/,
                    textureWidth, textureHeight, // les dimensions de l’image lue
                    0/*border*/,
                    GL_RGB/*format*/,
                    GL_UNSIGNED_BYTE/*type*/,
                    image/*pixels => le contenu de l’image chargée*/);
    #endif
    //SOIL_free_image_data(image);


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
        "float heigth = position.y+1.0;                                       \n"
        "vec4 color = vec4(1,0,0,1);                                       \n"
        "    if(heigth<0.3)                                          \n"
        "       color = vec4(0,0,heigth,1);                                       \n"
        "    if(heigth>=0.3 && heigth < 0.6)                                          \n"
        "       color = vec4(0,heigth,0,1);                                       \n"
        "    if(heigth>=0.6)                                          \n"
        "       color = vec4(heigth,heigth,heigth,1);                  \n"
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
        "    vertexColor = vec4( lightColor, 1.0 ) * color * diffuse;                 \n"
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
        "}                                             \n"
    };

    // Fragment shader
    const char* fragmentShaderSource[] = {
        "#version 300 es                             \n"
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

    glAttachShader( mHeigthMapShaderProgram, vertexShader );
    glAttachShader( mHeigthMapShaderProgram, fragmentShader );

    glLinkProgram( mHeigthMapShaderProgram );

    return statusOK;
}
