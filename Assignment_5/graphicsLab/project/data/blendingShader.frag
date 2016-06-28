
varying mediump vec2 texCoordVarying;
varying mediump vec2 uv;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform highp float exposure;

void main()
{
    const mediump float gamma = 2.2;
    highp vec3 hdrColor = texture2D(scene, uv).rgb;
    highp vec3 bloomColor = texture2D(bloomBlur, uv).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    highp vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it
    //result = pow(result, vec3(1.0 / gamma));
    gl_FragColor = vec4(result, 1.0);
}