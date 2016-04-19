
uniform mediump mat4 ViewMatrix;
uniform mediump mat4 modelMatrixTerrain;
uniform mediump mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

attribute vec4 Position;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Bitangent;
attribute vec4 TexCoord;

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying mediump vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

void main()
{
//    mediump vec4 pos = ModelMatrix * Position;
//    mediump vec3 normal = normalize(NormalMatrix * Normal);
//    texCoordVarying = TexCoord;
//    
//    ambientVarying = vec4(Ka * Ia, 1.0);
//    
//    // TODO: calculate diffuse lighting
//    diffuseVarying = vec4(0.0);
//    mediump vec3 n = normal ;
//    mediump vec3 l = normalize(vec3(LightPos-pos)) ;
//    
////    vec3 Cs = vec3(0.0);
////    if( dot(n, l) >= 0.0 ) {
////        vec3 V = normalize(cameraPos-vertexPos); vec3 R = reflect( l, n ); Cs = pow( max( 0.0, dot( R, V ) ), Ns );
////    }
//    
//    lowp float intensity = dot(n,l) ;
//    lowp vec3 diffuse = Kd * clamp(intensity, 0.0, 1.0) * Id;
//    diffuseVarying = vec4(clamp(diffuse, 0.0, 1.0), 1.0);
//    
//    // TODO: If vertex is lit, calculate specular term in view space using the Blinn-Phong model
//    specularVarying = vec4(0.0);
//    
//    if (intensity > 0.0)
//    {
//        mediump vec3 eyeVec = normalize(vec3(EyePos-pos)) ;
//        mediump vec3 h = normalize(l + eyeVec)/* / (length(l + eyeVec)))*/;
//        
//        mediump vec3 specular = Ks * pow(dot(n,h),Ns) * Is;
//        specularVarying = vec4(clamp(specular, 0.0, 1.0), 1.0);
//    }
    
    texCoordVarying = TexCoord;
    posVarying = modelMatrixTerrain * Position;
    normalVarying = normalize(NormalMatrix * Normal);

    gl_Position = ProjectionMatrix * ViewMatrix * posVarying;
}
