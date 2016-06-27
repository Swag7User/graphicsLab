$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D fbo_texture;
uniform float speed;

varying vec4 texCoordVarying;


varying vec2 uv;

void main(void)
{
    float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;
    
    vec2 dir = 0.5 - uv;
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
    dir = dir/dist;
    
    vec4 color = texture2D(fbo_texture,uv);
    vec4 sum = color;
    
    for (int i = 0; i < 9; i++)
        sum += texture2D( fbo_texture, uv + dir * samples[i]);
    
    sum *= 1.0/11.0;
    float t = dist * speed;
    t = clamp( t ,0.0,1.0);
    
    gl_FragColor = mix( color, sum, t );
}