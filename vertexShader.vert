//#version 330 core
#version 130

// INPUT
in vec3 position;

// UNIFORM
// - camera
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
// - 3D model
uniform mat4 modelMatrix;
// - animation
uniform float time;

// OUTPUT

// MAIN
void main( void )
{
#if 1
    // Use animation
    float amplitude = 1.0;
    float frequency = 0.5;
    float height = amplitude * sin( 2.0 * 3.141592 * frequency * ( time * 0.001 ) );
    vec3 pos = vec3( position.x, position.y + height, position.z );
	// Send position to Clip-space
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( pos, 1.0 );
#else
    // Send position to Clip-space
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4( position, 1.0 );
#endif
}
