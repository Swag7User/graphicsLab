$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying highp vec2 uv;

uniform highp float isVertical;

varying highp vec2 v_blurTexCoords[14];

void main()
{
    
    vec4 pos = ProjectionMatrix * ModelViewMatrix * Position;
    gl_Position = vec4( pos.xy, 0.0, 1.0 );
    gl_Position = sign( gl_Position );
    uv = (vec2( pos.x, pos.y ) + vec2(1.0) ) / vec2(2.0);
    
    if(isVertical > 0.0){
        v_blurTexCoords[ 0] = uv.st + vec2(0.0, -0.028);
        v_blurTexCoords[ 1] = uv.st + vec2(0.0, -0.024);
        v_blurTexCoords[ 2] = uv.st + vec2(0.0, -0.020);
        v_blurTexCoords[ 3] = uv.st + vec2(0.0, -0.016);
        v_blurTexCoords[ 4] = uv.st + vec2(0.0, -0.012);
        v_blurTexCoords[ 5] = uv.st + vec2(0.0, -0.008);
        v_blurTexCoords[ 6] = uv.st + vec2(0.0, -0.004);
        v_blurTexCoords[ 7] = uv.st + vec2(0.0,  0.004);
        v_blurTexCoords[ 8] = uv.st + vec2(0.0,  0.008);
        v_blurTexCoords[ 9] = uv.st + vec2(0.0,  0.012);
        v_blurTexCoords[10] = uv.st + vec2(0.0,  0.016);
        v_blurTexCoords[11] = uv.st + vec2(0.0,  0.020);
        v_blurTexCoords[12] = uv.st + vec2(0.0,  0.024);
        v_blurTexCoords[13] = uv.st + vec2(0.0,  0.028);
    }
    else{
        v_blurTexCoords[ 0] = uv.st + vec2(-0.028, 0.0);
        v_blurTexCoords[ 1] = uv.st + vec2(-0.024, 0.0);
        v_blurTexCoords[ 2] = uv.st + vec2(-0.020, 0.0);
        v_blurTexCoords[ 3] = uv.st + vec2(-0.016, 0.0);
        v_blurTexCoords[ 4] = uv.st + vec2(-0.012, 0.0);
        v_blurTexCoords[ 5] = uv.st + vec2(-0.008, 0.0);
        v_blurTexCoords[ 6] = uv.st + vec2(-0.004, 0.0);
        v_blurTexCoords[ 7] = uv.st + vec2( 0.004, 0.0);
        v_blurTexCoords[ 8] = uv.st + vec2( 0.008, 0.0);
        v_blurTexCoords[ 9] = uv.st + vec2( 0.012, 0.0);
        v_blurTexCoords[10] = uv.st + vec2( 0.016, 0.0);
        v_blurTexCoords[11] = uv.st + vec2( 0.020, 0.0);
        v_blurTexCoords[12] = uv.st + vec2( 0.024, 0.0);
        v_blurTexCoords[13] = uv.st + vec2( 0.028, 0.0);
    }
    
    
}
