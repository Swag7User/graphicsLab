$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D fbo_texture;


varying vec4 texCoordVarying;


varying highp vec2 uv;

void main(void)
{
    highp vec4 color = texture2D(fbo_texture,uv);
    
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    highp float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.7){
        gl_FragColor = vec4(color.rgb, 1.0);
    }
    
}