$B_SHADER_VERSION
#ifdef GL_ES
precision lowp float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;
varying vec4 texCoordVarying;

varying vec2 uv;

void main(void)
{
    
    vec4 pos = ProjectionMatrix * ModelViewMatrix * Position;

    gl_Position = vec4( pos.xy, 0.0, 1.0 );
    gl_Position = sign( gl_Position );
    uv = (vec2( pos.x, pos.y ) + vec2(1.0) ) / vec2(2.0);

    texCoordVarying = TexCoord;
    
    
}
