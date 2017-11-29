//#version 330 core
#version 130

// INPUT

// UNIFORM
uniform vec3 meshColor;

// OUTPUT
out vec4 fragmentColor;

// MAIN
void main( void )
{
    fragmentColor = vec4( meshColor, 1.0 );
}
            