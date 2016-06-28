$B_SHADER_VERSION
#ifdef GL_ES
precision highp float;
#endif

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

attribute vec4 Position;
attribute vec4 TexCoord;

varying vec4 texCoordVarying;
varying vec2 uv;

uniform float isVertical;

varying vec2 v_blurTexCoords[14];

void main()
{
    texCoordVarying = TexCoord;
    vec4 pos = ProjectionMatrix * ViewMatrix * Position;
    
    gl_Position = vec4( pos.xy, 0.0, 1.0 );
    gl_Position = sign( gl_Position );
    uv = (vec2( pos.x, pos.y ) + vec2(1.0) ) / vec2(2.0);
    
    if(isVertical > 0.0){
        v_blurTexCoords[ 0] = texCoordVarying.st + vec2(0.0, -0.028);
        v_blurTexCoords[ 1] = texCoordVarying.st + vec2(0.0, -0.024);
        v_blurTexCoords[ 2] = texCoordVarying.st + vec2(0.0, -0.020);
        v_blurTexCoords[ 3] = texCoordVarying.st + vec2(0.0, -0.016);
        v_blurTexCoords[ 4] = texCoordVarying.st + vec2(0.0, -0.012);
        v_blurTexCoords[ 5] = texCoordVarying.st + vec2(0.0, -0.008);
        v_blurTexCoords[ 6] = texCoordVarying.st + vec2(0.0, -0.004);
        v_blurTexCoords[ 7] = texCoordVarying.st + vec2(0.0,  0.004);
        v_blurTexCoords[ 8] = texCoordVarying.st + vec2(0.0,  0.008);
        v_blurTexCoords[ 9] = texCoordVarying.st + vec2(0.0,  0.012);
        v_blurTexCoords[10] = texCoordVarying.st + vec2(0.0,  0.016);
        v_blurTexCoords[11] = texCoordVarying.st + vec2(0.0,  0.020);
        v_blurTexCoords[12] = texCoordVarying.st + vec2(0.0,  0.024);
        v_blurTexCoords[13] = texCoordVarying.st + vec2(0.0,  0.028);
    }
    else{
        v_blurTexCoords[ 0] = texCoordVarying.st + vec2(-0.028, 0.0);
        v_blurTexCoords[ 1] = texCoordVarying.st + vec2(-0.024, 0.0);
        v_blurTexCoords[ 2] = texCoordVarying.st + vec2(-0.020, 0.0);
        v_blurTexCoords[ 3] = texCoordVarying.st + vec2(-0.016, 0.0);
        v_blurTexCoords[ 4] = texCoordVarying.st + vec2(-0.012, 0.0);
        v_blurTexCoords[ 5] = texCoordVarying.st + vec2(-0.008, 0.0);
        v_blurTexCoords[ 6] = texCoordVarying.st + vec2(-0.004, 0.0);
        v_blurTexCoords[ 7] = texCoordVarying.st + vec2( 0.004, 0.0);
        v_blurTexCoords[ 8] = texCoordVarying.st + vec2( 0.008, 0.0);
        v_blurTexCoords[ 9] = texCoordVarying.st + vec2( 0.012, 0.0);
        v_blurTexCoords[10] = texCoordVarying.st + vec2( 0.016, 0.0);
        v_blurTexCoords[11] = texCoordVarying.st + vec2( 0.020, 0.0);
        v_blurTexCoords[12] = texCoordVarying.st + vec2( 0.024, 0.0);
        v_blurTexCoords[13] = texCoordVarying.st + vec2( 0.028, 0.0);
    }
    
    
}
