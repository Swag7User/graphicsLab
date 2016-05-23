
uniform highp mat4 ViewMatrix;
uniform highp mat4 modelMatrixTerrain;
uniform highp mat4 ProjectionMatrix;

uniform mediump mat3 NormalMatrix;

uniform mediump vec4 LightPos;
uniform mediump vec4 LightPos2;

uniform mediump vec4 EyePos;

uniform lowp vec3 Ka;   // ambient material coefficient
uniform lowp vec3 Kd;   // diffuse material coefficient
uniform lowp vec3 Ks;   // specular material coefficient

uniform mediump float Ns;   // specular material exponent (shininess)

uniform lowp vec3 Ia;   // ambient light intensity
uniform lowp vec3 Id;   // diffuse light intensity
uniform lowp vec3 Is;   // specular light intensity
uniform lowp vec3 PosPlane;
uniform lowp vec3 PosZEP;


uniform sampler2D DiffuseMap;
uniform sampler2D SpecularMap;
//uniform float bias;

lowp vec4 ambient;
lowp vec4 diffuse;
lowp vec4 diffuse2;
lowp vec4 specular;
lowp vec4 texCoord;

varying mediump vec4 posVarying;       // pos in world space
varying mediump vec3 normalVarying;    // normal in world space

varying lowp vec4 ambientVarying;
varying lowp vec4 diffuseVarying;
varying lowp vec4 specularVarying;
varying lowp vec4 texCoordVarying;

uniform highp vec3 wsEyePosition;
varying highp vec3 wsInterpolatedEye;
varying highp vec3 wsInterpolatedNormal;

varying highp float dist;
varying highp vec4 vVertex;

lowp vec4 fogColor;

struct fogParameters{
    
    highp float fDensity;
    highp float fStart;
    highp float fEnd;
    highp vec4 fogColor;
    int iFogEquation;
} fogParams;





void main()
{
    
    //fogParameters
    fogParams.fDensity = 0.04;
    fogParams.fStart = 10.0;
    fogParams.fEnd = 20.0;
    fogParams.fogColor = vec4(0.9, 0.3, 0.3, 0.01);
    fogParams.iFogEquation = 2;
    
    
    // TODO: implement Phong Shading (per-fragment lighting)
    //ambient here
    ambient = vec4(Ka * Ia, 1.0);
    //bias=1.0;
    //diffuse here
    highp vec3 l = normalize((LightPos - posVarying).xyz);
    highp vec3 diffLight = Kd * clamp(dot(normalVarying, l),0.0, 1.0) * Id;
    diffuse = vec4(clamp(diffLight, 0.0, 1.0), 1.0);
    //difuse2
    highp vec3 l2 = normalize((LightPos2 - posVarying).xyz);
    highp vec3 diffLight2 = Kd * clamp(dot(normalVarying, l),0.0, 1.0) * Id;
    diffuse2 = vec4(clamp(diffLight2, 0.0, 1.0), 1.0);
    
    
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
    highp float no=1.0;
    highp float f=20000.0;
    //highp vec4 color3 = texture2DProj(DiffuseMap,texCoordVarying,((posVarying.z-PosPlane.z)+(posVarying.x-PosPlane.x))/100.0); // TODO: read color from DiffuseMap
    
    //uiiuugjefejjfifiuerguifernfnrifgnregebg
    
    highp vec3 wsNormal = normalize(wsInterpolatedNormal);
    highp vec3 wsEye = normalize(wsInterpolatedEye);
    highp vec2 selector;
    selector.x = (1.0 + dot(wsNormal,wsEye))/2.0;
    
    highp float xxx = length(EyePos+vVertex);
    highp float sel = 1.0-(log(xxx/-1.0))/(log(1.0/-1.0));
    
    selector.y = ((xxx/4500.0)-(1.0))*(-1.0);
    bool happend=false;
    
    //    highp vec4 color = vec4(0.7,0.1,0.4,1); // TODO: read color from DiffuseMap
    highp vec4 color;
    highp vec4 color2;
    highp vec4 color3;
    
    highp float deltax=abs((PosPlane.x) - (posVarying.x));
    highp float deltaz=abs((PosPlane.z) - (posVarying.z));
    highp float deltadelta=abs(deltax-deltaz);
    highp float deltaLength=sqrt(deltax*deltax*deltaz*deltaz);
    
    highp float deltaxzep=abs((PosZEP.x) - (posVarying.x));
    highp float deltazzep=abs((PosZEP.z) - (posVarying.z));
    highp float deltadeltazep=abs(deltaxzep-deltazzep);
    highp float deltaLengthzep=sqrt(deltaxzep*deltaxzep*deltazzep*deltazzep);
    
    if(deltax< 9.0  && deltaz < 9.0 && deltaLength<25.0){
        
        color3 = texture2D(DiffuseMap, selector.st);
        color2 = texture2D(SpecularMap, texCoordVarying.st);
        happend=true;
        color = vec4(0.05,0.05,0.05,1.0);
        gl_FragColor =  (color+(color2*color3*0.6));
    }
    else if(deltaxzep< 20.0  && deltazzep < 20.0 && deltaLengthzep<100.0){
        color3 = texture2D(DiffuseMap, selector.st);
        color2 = texture2D(SpecularMap, texCoordVarying.st);
        
        color = vec4(0.05,0.05,0.05,1.0);
        gl_FragColor =  (color+(color2*color3*0.6));
        happend=true;
    }
    else{
    color = texture2D(DiffuseMap, selector.st);
    color2 = texture2D(SpecularMap, texCoordVarying.st);
    gl_FragColor =  ((color)+(color2*0.1));
        happend=false;
    }
    //highp vec4 color2 = texture2DProj(SpecularMap,texCoordVarying,((posVarying.z-PosPlane.z)+(posVarying.x-PosPlane.x))/100.0);

    
    //highp float depth=texture2D(DiffuseMap,texCoordVarying.xy).r;
    //highp vec4 blure = texture2DProj(DiffuseMap,texCoordVarying,bias);
    //depth=(2.0*no)/(f+no-depth*(f-no));
    //color*=depth;
    //color=(2.0*no)/(f+n)
    
    //gl_FragColor = (color*0.666 + color2*0.333) ;
    
    
    highp float fFogCoord = abs(EyePos.y/EyePos.w);
    
    highp float fogFactor;
    
    highp float fResult = 0.0;
    
    fResult = exp(-fogParams.fDensity*fFogCoord);
    
    fogFactor = fResult;
    
    
    highp float depth = deltaLength;
    highp float fogFactorIntensity = (1.0-(1.0/(100.0/deltadelta)));
    
    
    //if(!happend){
    
  //      gl_FragColor = mix(gl_FragColor,fogParams.fogColor, fogFactorIntensity*fogParams.);
  // }
    
    
    
    
}
