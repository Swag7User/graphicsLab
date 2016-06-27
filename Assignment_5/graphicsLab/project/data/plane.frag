$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D fbo_texture;


varying vec4 texCoordVarying;


varying vec2 uv;

void main(void)
{
    vec4 color = texture2D(fbo_texture,uv);
    gl_FragColor = color;
}