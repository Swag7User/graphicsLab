uniform highp mat4 ViewMatrix;
uniform highp mat4 modelMatrixTAL;
uniform highp mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrixTAL;

uniform mediump vec4 LightPos;
uniform mediump vec3 lightDir;
uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity

uniform sampler2D DiffuseMap;
uniform sampler2D BloomMap;


lowp vec4 ambient;
lowp vec4 diffuse;
lowp vec4 diffuse2;
lowp vec4 specular;
lowp vec4 texCoord;
mediump float intensity;


varying mediump vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

void main()
{
    
    mediump vec4 pos = modelMatrixTAL * posVarying;
    mediump vec3 normal = normalize(NormalMatrixTAL * normalVarying);
    mediump vec3 n = normal ;
    mediump vec3 l = normalize(vec3(LightPos-pos)) ;
    
    intensity = dot(n,l) ;
    
    
    // TODO: implement Phong Shading (per-fragment lighting)
    //ambient here
    ambient = vec4(Ka * Ia, 1.0);
    
    
    //diffuse here
    //highp vec3 l = normalize((LightPos - posVarying).xyz);
    highp vec3 diffLight = Kd * clamp(dot(normalVarying, l),0.0, 1.0) * Id;
    diffuse = vec4(clamp(diffLight, 0.0, 1.0), 1.0);
    //difuse2
    //   vec3 halfDir = normalize(lightDir + viewDir);
    //   float specAngle = max(dot(halfDir, normal), 0.0);
    //   specular = pow(specAngle, shininess);
    //TEST PUSH
    
    //Bling Phong shading here
    specular = vec4(0.0);
    if (dot(normalVarying, l) > 0.0)
    {
        highp vec3 eyeVec = normalize((EyePos - posVarying).xyz);
        highp vec3 h = normalize((l + eyeVec)/length(l+eyeVec));
        
        highp vec3 spec = Ks * pow(dot(normalVarying,h), Ns) * Is;
        specular = vec4(clamp(spec,0.0,1.0),1.0);
    }
    
    
    //    highp vec4 color = vec4(0.7,0.1,0.4,1); // TODO: read color from DiffuseMap
    
    mediump vec4 color2;
    
    if (intensity > 0.95){
        color2 = vec4(0.7,0.7,0.7,1.0);}
    else if (intensity > 0.5){
        color2 = vec4(0.6,0.6,0.6,1.0);}
    else if (intensity > 0.25){
        color2 = vec4(0.4,0.4,0.4,1.0);}
    else{
        color2 = vec4(0.2,0.2,0.2,1.0);}
    mediump vec4 color = texture2DProj(DiffuseMap,texCoordVarying);
    //texture2DProj(bloomcamo.jpg, texCoordVarying);
    
    gl_FragColor = (ambient + diffuse ) * (color*color2) + specular;
}