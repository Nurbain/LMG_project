#include "SkyBox.h"

SkyBox::SkyBox(){

}


bool SkyBox::initializeCubemap(){
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

bool SkyBox::initializeCubemapGeometry(){
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

bool SkyBox::initializeCubemapTextures(){
    bool result = true;

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


    // - be consistent with cubemap's internal OpenGL face ordering
    const std::string envmapRepository = ImgRepository;
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

bool SkyBox::initializeCubemapShader(){
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

