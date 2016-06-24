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


varying highp vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

varying highp vec3 wsEyePosition;
varying highp vec3 wsInterpolatedEye;
varying highp vec3 wsInterpolatedNormal;

varying highp float dist;
varying highp vec4 vVertex;

void main()
{

    highp vec4 pos = modelMatrixTAL * posVarying;
    mediump vec3 normal = normalize(NormalMatrixTAL * normalVarying);
    highp vec3 n = normal ;
    highp vec3 l = normalize(vec3(LightPos-pos)) ;
    
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
    //uiiuugjefejjfifiuerguifernfnrifgnregebg
    
    highp vec3 wsNormal = normalize(wsInterpolatedNormal);
    highp vec3 wsEye = normalize(vec3(vVertex));
    highp vec2 selector;
    selector.x = (1.0 + dot(wsNormal,wsEye))/2.0;
    
    highp float xxx = length(EyePos+vVertex);
    highp float sel = 1.0-(log(xxx/-1.0))/(log(1.0/-1.0));
    
    selector.y = (xxx/10.0);
    
    //selector.y = dist/10.0;
    
    
    //    highp vec4 color = vec4(0.7,0.1,0.4,1); // TODO: read color from DiffuseMap
    
    highp vec4 color = texture2D(DiffuseMap,selector.st);
    //texture2DProj(bloomcamo.jpg, texCoordVarying);
    
    gl_FragColor = (color) + specular;
}