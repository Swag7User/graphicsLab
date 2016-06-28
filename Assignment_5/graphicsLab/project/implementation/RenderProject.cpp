#include "RenderProject.h"
#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif

vmml::Matrix4f lookAt(vmml::Vector3f eye, vmml::Vector3f target, vmml::Vector3f up)
{
    vmml::Vector3f zaxis = vmml::normalize(eye - target);
    vmml::Vector3f xaxis = -vmml::normalize(vmml::cross<3>(up, zaxis));
    vmml::Vector3f yaxis = -vmml::cross<3>(zaxis, xaxis);
    
    vmml::Matrix4f view;
    view.set_row(0, vmml::Vector4f(xaxis.x(), xaxis.y(), xaxis.z(), -vmml::dot(xaxis, eye)));
    view.set_row(1, vmml::Vector4f(yaxis.x(), yaxis.y(), yaxis.z(), -vmml::dot(yaxis, eye)));
    view.set_row(2, vmml::Vector4f(zaxis.x(), zaxis.y(), zaxis.z(), -vmml::dot(zaxis, eye)));
    view.set_row(3, vmml::Vector4f(0, 0, 0, 1.0));
    
    return view;
}

vmml::Matrix4f perspective(float fovy, float aspect, float zNear, float zFar)
{
    float f = 1.0 / tanf((fovy * (M_PI_F / 180.0)) / 2.0);
    
    vmml::Matrix4f perspective;
    perspective.set_row(0, vmml::Vector4f(f / aspect, 0, 0, 0));
    perspective.set_row(1, vmml::Vector4f(0, f, 0, 0));
    perspective.set_row(2, vmml::Vector4f(0, 0, (zFar + zNear) / (zNear - zFar), (2.0 * zFar * zNear) / (zNear - zFar)));
    perspective.set_row(3, vmml::Vector4f(0, 0, -1.0, 0));
    
    return perspective;
}

vmml::Matrix4f projectionMatrix;
vmml::Matrix4f defaultTranslation= vmml::create_translation(vmml::Vector3f(2700.0f, 0.0f, 2700.0f));
vmml::Matrix4f modelMatrixTerrain = defaultTranslation*vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 0.0f));
;
vmml::Matrix4f modelMatrixW = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 10.0f)) * vmml::create_scaling(vmml::Vector3f(10.f));
vmml::Matrix4f modelMatrixTAL = defaultTranslation*vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 0.f)) * vmml::create_scaling(vmml::Vector3f(1.f))*vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_X)*vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
vmml::Matrix4f modelMatrixZep = defaultTranslation*vmml::create_translation(vmml::Vector3f(0.0f, 100.0f, 500.f))*vmml::create_scaling(vmml::Vector3f(0.1f,0.1f,0.1f));
vmml::Matrix4f modelMatrixSKY = defaultTranslation*vmml::create_translation(modelMatrixTerrain.get_translation()) * vmml::create_scaling(vmml::Vector3f(1.f));
vmml::Matrix4f modelMatrixCL = vmml::create_translation(modelMatrixTerrain.get_translation())*vmml::create_translation(vmml::Vector3f(129.0f, 720.0f, 290.f)) * vmml::create_scaling(vmml::Vector3f(50.f));
vmml::Matrix4f modelMatrixCL2 =vmml::create_translation(modelMatrixTerrain.get_translation())*vmml::create_translation(vmml::Vector3f(0.0f, 700.0f, 0.0f)) * vmml::create_scaling(vmml::Vector3f(40.f));
vmml::Matrix4f modelMatrixCL3 =vmml::create_translation(modelMatrixTerrain.get_translation())*vmml::create_translation(vmml::Vector3f(330.0f, 690.0f, 120.f)) * vmml::create_scaling(vmml::Vector3f(60.f));
vmml::Matrix4f modelMatrixCL4 =vmml::create_translation(modelMatrixTerrain.get_translation())*vmml::create_translation(vmml::Vector3f(-330.0f, 700.0f, -400.f)) * vmml::create_scaling(vmml::Vector3f(35.f));
vmml::Matrix4f modelMatrixCL5 =vmml::create_translation(modelMatrixTerrain.get_translation())*vmml::create_translation(vmml::Vector3f(-300.0f, 640.0f, 470.f)) * vmml::create_scaling(vmml::Vector3f(70.f));
vmml::AABBf hit_TAL;
vmml::AABBf hit_Zep;
vmml::AABBf hit_Zep_size;
//vmml::AABBf hit_Zep;
vmml::AABBf hit_Terrain;
vmml::AABBf hit_Terrain_size;
ShaderPtr guyShader;
ShaderPtr TALShader;
ShaderPtr ZepShader;
ShaderPtr SKYShader;
ShaderPtr CLShader;
ShaderPtr CL2Shader;
ShaderPtr CL3Shader;
ShaderPtr CL4Shader;
ShaderPtr CL5Shader;
ShaderPtr WShader;
ShaderPtr spriteW;
ShaderPtr blurShader;
ShaderPtr planeShader;
ShaderPtr bloomShader;
ShaderPtr brightnessShader;
ShaderPtr blendingShader;
PropertiesPtr guyProperties;
PropertiesPtr TALProperties;
PropertiesPtr ZepProperties;
PropertiesPtr SKYProperties;
PropertiesPtr CLProperties;
PropertiesPtr CL2Properties;
PropertiesPtr CL3Properties;
PropertiesPtr CL4Properties;
PropertiesPtr CL5Properties;
PropertiesPtr WProperties;
bool player_won=false;
bool player_lost=false;
GLint defaultFBO;

double _time = 0;
double _pitchSum = 0;
double _rollSum = 0;
vmml::Vector3f _initialAircraftOrientation(0.0f, 0.0f, -1.0f);
vmml::Vector3f _cameraOffset(0.0f, 1.0f, 0.0f);
vmml::Vector3f _newAircraftPosition(2700.0f, 0.0f, 2700.0f);
vmml::Vector3f _eyePos(0.0f, 0.0f, 0.0f);
float _speed = 2.0f;

//CMMotionManager *cmMotionManager=CMMotionManager();
float angle=0.f;


int square_max=4;
int line_max=500;
int square_count=0;
int line_count=0;
int turning_counter=0;
int turning_max=50;
int counterW=1;
int counterWmax=100;
float boostb = 1;

bool is_turning=false;


/* Initialize the Project */
void RenderProject::init()
{
    bRenderer::loadConfigFile("config.json");	// load custom configurations replacing the default values in Configuration.cpp
    
    // let the renderer create an OpenGL context and the main window
    if(Input::isTouchDevice())
        bRenderer().initRenderer(true);										// full screen on iOS
    else
        bRenderer().initRenderer(1920, 1080, false, "Assignment 5");		// windowed mode on desktop
    //bRenderer().initRenderer(View::getScreenWidth(), View::getScreenHeight(), true);		// full screen using full width and height of the screen

    // start main loop
    bRenderer().runRenderer();
}

/* This function is executed when initializing the renderer */
void RenderProject::initFunction()
{
    // get OpenGL and shading language version
    bRenderer::log("OpenGL Version: ", glGetString(GL_VERSION));
    bRenderer::log("Shading Language Version: ", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // initialize variables
    _offset = 0.0f;
    _randomOffset = 0.0f;
    _cameraSpeed = 40.0f;
    _running = true; _lastStateSpaceKey = bRenderer::INPUT_UNDEFINED;
    _viewMatrixHUD = Camera::lookAt(vmml::Vector3f(0.0f, 0.0f, 0.25f), vmml::Vector3f::ZERO, vmml::Vector3f::UP);
    projectionMatrix = perspective(60.0, bRenderer().getView()->getAspectRatio(), 1.0, 100000.0);
    
    // set shader versions (optional)
    bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
    bRenderer().getObjects()->setShaderVersionES("#version 100");
    
    // load materials and shaders before loading the model
    guyShader = bRenderer().getObjects()->loadShaderFile("guy", 0, false, false, false, false, false);				// load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    TALShader = bRenderer().getObjects()->loadShaderFile("TAL", 0, false, false, false, false, false);
    planeShader = bRenderer().getObjects()->loadShaderFile("plane", 0, false, false, false, false,false);
    ZepShader = bRenderer().getObjects()->loadShaderFile("Zep", 0, false, false, false, false, false);
    SKYShader = bRenderer().getObjects()->loadShaderFile("SKY", 0, false, false, false, false, false);
    CLShader = bRenderer().getObjects()->loadShaderFile("CL", 0, false, false, false, false, false);
    CL2Shader = bRenderer().getObjects()->loadShaderFile("CL2", 0, false, false, false, false, false);
    CL3Shader = bRenderer().getObjects()->loadShaderFile("CL3", 0, false, false, false, false, false);
    CL4Shader = bRenderer().getObjects()->loadShaderFile("CL4", 0, false, false, false, false, false);
    CL5Shader = bRenderer().getObjects()->loadShaderFile("CL5", 0, false, false, false, false, false);
    WShader = bRenderer().getObjects()->loadShaderFile("W", 0, false, false, false, false, false);

    
    // create additional properties for a model
    guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    TALProperties = bRenderer().getObjects()->createProperties("TALProperties");
    ZepProperties = bRenderer().getObjects()->createProperties("ZepProperties");
    SKYProperties = bRenderer().getObjects()->createProperties("SKYProperties");
    CLProperties = bRenderer().getObjects()->createProperties("CLProperties");
    CL2Properties = bRenderer().getObjects()->createProperties("CL2Properties");
    CL3Properties = bRenderer().getObjects()->createProperties("CL3Properties");
    CL4Properties = bRenderer().getObjects()->createProperties("CL4Properties");
    CL5Properties = bRenderer().getObjects()->createProperties("CL5Properties");
    //WProperties = bRenderer().getObjects()->createProperties("WProperties");
    // load model
    //bRenderer().getObjects()->loadObjModel("guy.obj", true, true, true, 0, false, false, guyProperties);
    hit_Terrain=bRenderer().getObjects()->loadObjModel("terraintree_simple.obj", false, true, guyShader, guyProperties)->getBoundingBoxObjectSpace();
    //hit_TAL=bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties)->getBoundingBoxObjectSpace();
    hit_Zep=bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties)->getBoundingBoxObjectSpace();
    // automatically generates a shader with a maximum of 4 lights (number of lights may vary between 0 and 4 during rendering without performance loss)
    bRenderer().getObjects()->loadObjModel("skybox.obj", false, true, SKYShader, SKYProperties);
    bRenderer().getObjects()->loadObjModel("clouds.obj", false, true, CLShader, CLProperties);
    bRenderer().getObjects()->loadObjModel("clouds2.obj", false, true, CL2Shader, CL2Properties);
    bRenderer().getObjects()->loadObjModel("clouds3.obj", false, true, CL3Shader, CL3Properties);
    bRenderer().getObjects()->loadObjModel("clouds4.obj", false, true, CL4Shader, CL4Properties);
    bRenderer().getObjects()->loadObjModel("clouds5.obj", false, true, CL5Shader, CL5Properties);
    //bRenderer().getObjects()->loadObjModel("winning.obj", false, true, WShader, WProperties);
   // auto sspriteW=bRenderer().getObjects()->createSprite("winning", "winning.png");
    
    // postprocessing
    bRenderer().getObjects()->createFramebuffer("fbo");					// create framebuffer object
    bRenderer().getObjects()->createTexture("fbo_texture1", 0.f,0.f);	// create texture to bind to the fbo
    bRenderer().getObjects()->createTexture("fbo_texture2", 0.f, 0.f);	// create texture to bind to the fbo
    blurShader = bRenderer().getObjects()->loadShaderFile("blurShader", 0, false, false, false, false, false);			// load shader that blurs the texture
    MaterialPtr blurMaterial = bRenderer().getObjects()->createMaterial("blurMaterial", blurShader);								// create an empty material to assign either texture1 or texture2 to
    bRenderer().getObjects()->createSprite("blurSprite", blurMaterial);																// create a sprite using the material created above
    
    //bloom
    bRenderer().getObjects()->createFramebuffer("bloom");
    bRenderer().getObjects()->createFramebuffer("brightness");
    bRenderer().getObjects()->createTexture("fbo_texture3", 0.f, 0.f);
    bRenderer().getObjects()->createTexture("fbo_texture4", 0.f, 0.f);
    bloomShader = bRenderer().getObjects()->loadShaderFile("bloomShader", 0, false, false, false, false, false);
    MaterialPtr bloomMaterial = bRenderer().getObjects()->createMaterial("bloomMaterial", bloomShader);
    bRenderer().getObjects()->createSprite("bloomSprite", bloomMaterial);
    brightnessShader = bRenderer().getObjects()->loadShaderFile("brightnessShader", 0, false, false, false, false, false);
    bRenderer().getObjects()->createTexture("brightness_texture", 0.f, 0.f);
    MaterialPtr brightnessMaterial = bRenderer().getObjects()->createMaterial("brightnessMaterial", brightnessShader);
    bRenderer().getObjects()->createSprite("brightnessSprite", brightnessMaterial);
    blendingShader = bRenderer().getObjects()->loadShaderFile("blendingShader", 0, false, false, false, false, false);
    bRenderer().getObjects()->createTexture("blending_texture", 0.f, 0.f);
    MaterialPtr blendingMaterial = bRenderer().getObjects()->createMaterial("blendingMaterial", blendingShader);
    bRenderer().getObjects()->createSprite("blendingSprite", blendingMaterial);
    
    //plane
    bRenderer().getObjects()->createFramebuffer("plane");
    bRenderer().getObjects()->createTexture("plane_texture", 0.f, 0.f);
    MaterialPtr TALMaterial = bRenderer().getObjects()->createMaterial("TALMaterial", planeShader);
    bRenderer().getObjects()->createSprite("TALSprite", TALMaterial);
    
    //endscreen
    bRenderer().getObjects()->createSprite("winningSprite", "winning.png");
    bRenderer().getObjects()->createSprite("losingSprite", "lost.png");
    
    // create camera
    bRenderer().getObjects()->createCamera("camera", vmml::Vector3f(.0f, 0.0f, 0.0f), vmml::Vector3f(0.f, 0.f, 0.f));
    
    // Update render queue
    updateRenderQueue("camera", 0.0f);
}

/* Draw your scene here */
void RenderProject::loopFunction(const double &deltaTime, const double &elapsedTime)
{
    //	bRenderer::log("FPS: " + std::to_string(1 / deltaTime));	// write number of frames per second to the console every frame
    
    //// Draw Scene and do post processing ////
    
    /// Begin post processing ///
    //	GLint defaultFBO;
    //	if (!_running){
    //		bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth() / 5, bRenderer().getView()->getHeight() / 5);		// reduce viewport size
    //		defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
    //		bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);	// bind the fbo
    //	}
    
    /// Draw scene ///
    
    bRenderer().getModelRenderer()->drawQueue(/*GL_LINES*/);
    bRenderer().getModelRenderer()->clearQueue();
    
    //// Camera Movement ////
    updateCamera("camera", deltaTime);
    
    /// Update render queue ///
    updateRenderQueue("camera", deltaTime);
    
    // Quit renderer when escape is pressed
    if (bRenderer().getInput()->getKeyState(bRenderer::KEY_ESCAPE) == bRenderer::INPUT_PRESS)
        bRenderer().terminateRenderer();
}

/* This function is executed when terminating the renderer */
void RenderProject::terminateFunction()
{
    bRenderer::log("I totally terminated this Renderer :-)");
}

/* Update render queue */
void RenderProject::updateRenderQueue(const std::string &camera, const double &deltaTime)
{
    /*** GYRO ***/
    
    
    modelMatrixSKY = vmml::create_translation(modelMatrixTAL.get_translation())*vmml::create_translation(vmml::Vector3f(0.0f, -225.0f,2000.0f))*vmml::create_scaling(vmml::Vector3f(2000.f));
   

    
    _time += deltaTime;
    float angle = _time * 0.9;
    
    _pitchSum += bRenderer().getInput()->getGyroscopePitch()* 0.02f;
    _rollSum += bRenderer().getInput()->getGyroscopeRoll()*0.02f;

    
    vmml::Matrix4f rotationX = vmml::create_rotation(-(float)bRenderer().getInput()->getGyroscopeRoll(), vmml::Vector3f::UNIT_X);
    vmml::Matrix4f rotationY = vmml::create_rotation(-(float)_pitchSum, vmml::Vector3f::UNIT_Y);
    vmml::Matrix4f rotationZ = vmml::create_rotation(2 * (float)bRenderer().getInput()->getGyroscopePitch(), vmml::Vector3f::UNIT_Z);
    
    vmml::Vector3f newAircraftOrientation;/* = rotationY * rotationX * _initialAircraftOrientation;*/
   
    /*** Guy ***/
    // get input rotation
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    int i = 0;
    float rotation = 0.0f;
    for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
    {
        Touch touch = t->second;
        if (touch.startPositionX < bRenderer().getView()->getWidth()/2) {
            rotation = (touch.currentPositionX - touch.startPositionX) / 100;
            if (++i > 1)
                break;
        }
        else{
            
            TouchMap touchMap3 = bRenderer().getInput()->getTouches();
            int i2 = 0;
            for (auto t = touchMap3.begin(); t != touchMap3.end(); ++t)
            {
                Touch touchy = t->second;
                if (touchy.startPositionY < bRenderer().getView()->getHeight()/2) {
                    if(boostb < 3.0){
                    boostb += 0.025;
                    }
                }
                else{
                    if (boostb >0.5) {
                        boostb -= 0.025;
                    }
                }
            }
            
         }
       
    }
    // get input rotation
    TouchMap touchMap2 = bRenderer().getInput()->getTouches();
    int i2 = 0;
    float rotation2 = 0.0f;
    for (auto t = touchMap2.begin(); t != touchMap2.end(); ++t)
    {
        Touch touch = t->second;
        if (touch.startPositionX < bRenderer().getView()->getWidth()/2) {
        rotation2 = (touch.currentPositionY - touch.startPositionY) / 100;
        if (++i2 > 1)
            break;
        }
    }
    
    
    counterW++;
    if (counterW>=counterWmax) {
        counterW=1;
    }
   
    
    vmml::Matrix4f viewMatrix; // = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    
    
    // translate, rotate and scale
    if (hit_TAL.getMin().y()>=-150.0f) {
        if(!hit_Zep.isIn(hit_TAL.getMax()-vmml::Vector3f(8.0f,8.0f,8.0f))){
            if(_running){
                newAircraftOrientation = rotationY * rotationX *vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_X)*vmml::create_rotation((float)(270*M_PI_F/180), vmml::Vector3f::UNIT_Z);
                _newAircraftPosition += newAircraftOrientation * -_speed*boostb;
                
                vmml::Matrix4f translation = vmml::create_translation(_newAircraftPosition);
                
                modelMatrixTAL = translation * rotationY * rotationZ * rotationX *vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_X)*vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
                
                _eyePos = modelMatrixTAL * vmml::create_translation(vmml::Vector3f(0.0f, 10.0f, -1.0f)) * _cameraOffset;
                
                bRenderer().getObjects()->getCamera("camera")->setPosition(_eyePos);
                
                
                viewMatrix = lookAt(_eyePos, _newAircraftPosition, vmml::Vector3f::UP);
            }
        }
        else{
            _running = false;
            player_won=true;
        }
    }
    else{
        if(!player_won){
            _running = false;
            player_lost=true;
        }
    }
    
    //move Zeppelin
    if (square_count==0&&!is_turning) {
        modelMatrixZep *= vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 10.0f));
    }
    if ((square_count==1)&!is_turning) {
        modelMatrixZep *= vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 10.0f));
    }
    if (square_count==2&&!is_turning) {
        modelMatrixZep *= vmml::create_translation(vmml::Vector3f(0.0f, 0.0f,10.0f));
    }
    if (square_count==3&&!is_turning) {
        modelMatrixZep *= vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 10.0f));
    }
    if (line_count>=line_max) {
        square_count++;
     //   is_turning=true;
        line_count=0;
    }
    if (square_count>=square_max) {
        square_count=0;
    }
    line_count++;
    if (line_count%6==0) {
        modelMatrixZep*=vmml::create_rotation((90/(line_max/6)*M_PI_F/180), vmml::Vector3f::UNIT_Y);
    }
    

    //vmml::Vector3f camTranslation = modelMatrixTAL.get_translation();
    
    //bRenderer().getObjects()->getCamera("camera")->setPosition(-(camTranslation));
    
    
    vmml::Matrix4f rotationMatrix = vmml::create_rotation(rotation, vmml::Vector3f::UNIT_Y);
    
    
    
    
    //bRenderer().getObjects()->getCamera("camera")->lookAt(bRenderer().getObjects()->getCamera("camera")->getPosition(), modelMatrixTAL.get_translation(), vmml::Vector3f(0.0f, 0.0f,0.0f));
    
    
    
    
    rotationMatrix = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    
    if(!_running){
        rotation2 = bRenderer().getInput()->getGyroscopeRoll();
        rotation = bRenderer().getInput()->getGyroscopePitch();
        
        bRenderer().getObjects()->getCamera("camera")->rotateCamera(rotation2/100, 0.0f, 0.0f);
        
        bRenderer().getObjects()->getCamera("camera")->rotateCamera(0.0f, rotation/100, 0.0f);
        
        bRenderer().getObjects()->getCamera("camera")->moveCameraForward(cos(rotation2/100)*-10.0f);
        
        bRenderer().getObjects()->getCamera("camera")->moveCameraUpward(sin(rotation2/100)*10.0f);
    }
   
    
    //camTranslation.z() = camTranslation.z() - 10.0f;
    
    angle++;
    
    //vmml::Matrix4f rotationMatrixTAL = rotationX*rotationY;//*rotationZ;
    //modelMatrixTAL *= rotationMatrixTAL;

    

    
    //turn plane right
    hit_Terrain_size=bRenderer().getObjects()->loadObjModel("terraintree_simple.obj", false, true, guyShader, guyProperties)->getBoundingBoxObjectSpace();
    vmml::AABBf hit_TAL_size=bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties)->getBoundingBoxObjectSpace();
    //hit_Zep=vmml::create_translation(vmml::Vector3f(0.0f, 100.0f, 500.f))*vmml::create_scaling(vmml::Vector3f(0.1f,0.1f,0.1f));
    hit_Zep_size=bRenderer().getObjects()->loadObjModel("Zep.obj", false, true, ZepShader, ZepProperties)->getBoundingBoxObjectSpace();
    
    
    hit_TAL.setMin(hit_TAL_size.getMin()+modelMatrixTAL.get_translation() );
    hit_TAL.setMax(hit_TAL_size.getMax()+modelMatrixTAL.get_translation() );
    
    hit_Terrain.setMin(hit_Terrain_size.getMin()+modelMatrixTerrain.get_translation() );
    hit_Terrain.setMax(hit_Terrain_size.getMax()+modelMatrixTerrain.get_translation() );

    hit_Zep.setMin(hit_Zep_size.getMin()*0.1+modelMatrixZep.get_translation() );
    hit_Zep.setMax(hit_Zep_size.getMax()*0.1+modelMatrixZep.get_translation() );

    if (player_won) {
        bRenderer().getObjects()->getCamera("camera")->setPosition(modelMatrixW.get_translation());
    }
    if (player_lost) {
        bRenderer().getObjects()->getCamera("camera")->setPosition(modelMatrixW.get_translation());
    }
    
    
    ShaderPtr shader = bRenderer().getObjects()->getShader("guy");

    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTerrain", modelMatrixTerrain);

        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTerrain)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);

  
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 300.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
        shader->setUniform("PosPlane", vmml::Vector3f(modelMatrixTAL.get_translation()));
        shader->setUniform("PosZEP", vmml::Vector3f(modelMatrixZep.get_translation()));

    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("SKY");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixSKY", modelMatrixSKY);
        
        
        vmml::Matrix3f normalMatrixSKY;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixSKY)), normalMatrixSKY);
        shader->setUniform("NormalMatrixSKY", normalMatrixSKY);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 300.5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }

    
    shader = bRenderer().getObjects()->getShader("Zep");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixZep", modelMatrixZep);

        
        vmml::Matrix3f normalMatrixZep;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixZep)), normalMatrixZep);
        shader->setUniform("NormalMatrixZep", normalMatrixZep);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
  
    shader = bRenderer().getObjects()->getShader("CL");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixCL", modelMatrixCL);
        
        
        vmml::Matrix3f normalMatrixCL;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixCL)), normalMatrixCL);
        shader->setUniform("NormalMatrixCL", normalMatrixCL);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    shader = bRenderer().getObjects()->getShader("CL2");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixCL2", modelMatrixCL2);
        
        
        vmml::Matrix3f normalMatrixCL2;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixCL2)), normalMatrixCL2);
        shader->setUniform("NormalMatrixCL2", normalMatrixCL2);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    shader = bRenderer().getObjects()->getShader("CL3");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixCL3", modelMatrixCL3);
        
        
        vmml::Matrix3f normalMatrixCL3;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixCL3)), normalMatrixCL3);
        shader->setUniform("NormalMatrixCL3", normalMatrixCL3);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    shader = bRenderer().getObjects()->getShader("CL4");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixCL4", modelMatrixCL4);
        
        
        vmml::Matrix3f normalMatrixCL4;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixCL4)), normalMatrixCL4);
        shader->setUniform("NormalMatrixCL4", normalMatrixCL4);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    shader = bRenderer().getObjects()->getShader("CL5");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixCL5", modelMatrixCL5);
        
        
        vmml::Matrix3f normalMatrixCL5;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixCL5)), normalMatrixCL5);
        shader->setUniform("NormalMatrixCL5", normalMatrixCL5);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("W");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixW", modelMatrixW);
        
        
        vmml::Matrix3f normalMatrixW;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixW)), normalMatrixW);
        shader->setUniform("NormalMatrixW", normalMatrixW);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("end", player_lost);
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
        shader->setUniform("counter", (float)(counterW));
    }
    
    
    shader = bRenderer().getObjects()->getShader("blurShader");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTAL", modelMatrixTAL);
        
        
        vmml::Matrix3f normalMatrixTAL;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTAL)), normalMatrixTAL);
        shader->setUniform("NormalMatrixTAL", normalMatrixTAL);
        
        shader->setUniform("EyePos", _eyePos);
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 3.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 1.f, .5f, 1.f));
        
        shader->setUniform("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture1"));
        shader->setUniform("speed" ,boostb);
        //uniform sampler2D fbo_texture;
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("plane");
    if(shader.get()){
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("fbo_texture", bRenderer().getObjects()->getTexture("plane_texture"));
    }
    else{
       bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("brightnessShader");
    if(shader.get()){
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("fbo_texture", bRenderer().getObjects()->getTexture("blending_texture"));
    }
    else{
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("bloomShader");
    if (shader.get()){
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("fbo_texture", bRenderer().getObjects()->getTexture("fbo_texture3"));
        //shader->setUniform("isVertical", static_cast<GLfloat>(true));
    }
    else{
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("blendingShader");
    if (shader.get()){
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("scene", bRenderer().getObjects()->getTexture("blending_texture"));
        shader->setUniform("bloomBlur", bRenderer().getObjects()->getTexture("brightness_texture"));
        shader->setUniform("exposure", 1.0f);
    }
    else{
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("TAL");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", projectionMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTAL", modelMatrixTAL);
        
        
        vmml::Matrix3f normalMatrixTAL;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTAL)), normalMatrixTAL);
        shader->setUniform("NormalMatrixTAL", normalMatrixTAL);
        
        
        
        shader->setUniform("EyePos", bRenderer().getObjects()->getCamera("camera")->getPosition());
        
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }

    
   
    glDisable(GL_CULL_FACE);
    vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    
    //BLOOM
    if(_running){
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());		// reduce viewport size
        defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
        bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("blending_texture"), false);	// bind the fbo
    }
    
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("skybox"), modelMatrixSKY, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("terraintree_simple"), modelMatrixTerrain, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("Zep"), modelMatrixZep, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("clouds"), modelMatrixCL, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("clouds2"), modelMatrixCL2, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("clouds3"), modelMatrixCL3, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("clouds4"), modelMatrixCL4, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("clouds5"), modelMatrixCL5, viewMatrix, projectionMatrix, std::vector<std::string>({}));
  
    
    bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture3"), false);
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    bRenderer().getObjects()->getMaterial("brightnessMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("blending_texture"));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("brightnessSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    

    /*** Bloom ***/
    // blur vertically and horizontally
    bool b = true;		int numberOfBlurSteps = 2;
    for (int i = 0; i < numberOfBlurSteps; i++) {
        if (i == numberOfBlurSteps - 1){
            bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("brightness_texture"), false);
            bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
        }
        else
            bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture(b ? "fbo_texture4" : "fbo_texture3"), false);
        
        bRenderer().getObjects()->getMaterial("bloomMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture(b ? "fbo_texture3" : "fbo_texture4"));
        bRenderer().getObjects()->getMaterial("bloomMaterial")->setScalar("isVertical", static_cast<GLfloat>(b));
        // draw
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("bloomSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        b = !b;
    }
    
    bRenderer().getObjects()->getFramebuffer("fbo")->unbind(defaultFBO);
    
    //BLUR
    if(boostb>1.0 && _running){
        bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());		// reduce viewport size
        defaultFBO = Framebuffer::getCurrentFramebuffer();	// get current fbo to bind it again after drawing the scene
        bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture("fbo_texture1"), false);	// bind the fbo
    }
    
    bRenderer().getObjects()->getMaterial("blendingMaterial")->setTexture("scene", bRenderer().getObjects()->getTexture("blending_texture"));
    bRenderer().getObjects()->getMaterial("blendingMaterial")->setTexture("bloomBlur", bRenderer().getObjects()->getTexture("brightness_texture"));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blendingSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);

//    bRenderer().getObjects()->getMaterial("brightnessMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("brightness_texture"));
//    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("brightnessSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
    
    /*** Blur ***/
    // translate
    //vmml::Matrix4f modelMatrix = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, -0.5));
    // blur vertically and horizontally
    if (boostb>1.0 && _running) {
    bool b = true;
    int numberOfBlurSteps = 1;
    for (int i = 0; i < numberOfBlurSteps; i++) {
        if (i == numberOfBlurSteps - 1){
            bRenderer().getObjects()->getFramebuffer("fbo")->unbind(defaultFBO); //unbind (original fbo will be bound)
            bRenderer().getView()->setViewportSize(bRenderer().getView()->getWidth(), bRenderer().getView()->getHeight());								// reset vieport size
        }
        else
            bRenderer().getObjects()->getFramebuffer("fbo")->bindTexture(bRenderer().getObjects()->getTexture(b ? "fbo_texture2" : "fbo_texture1"), false);
        bRenderer().getObjects()->getMaterial("blurMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture(b ? "fbo_texture1" : "fbo_texture2"));
        bRenderer().getObjects()->getMaterial("blurMaterial")->setScalar("isVertical", static_cast<GLfloat>(b));
        // draw
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("blurSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}), false);
        b = !b;
    }
    }
    
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("TAL16OBJ"), modelMatrixTAL, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    
    bRenderer().getObjects()->getFramebuffer("plane")->bindTexture(bRenderer().getObjects()->getTexture("plane_texture"), false);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("TAL16OBJ"), modelMatrixTAL, viewMatrix, projectionMatrix, std::vector<std::string>({}));
    bRenderer().getObjects()->getFramebuffer("plane")->unbind(defaultFBO);
    
    
    bRenderer().getObjects()->getMaterial("TALMaterial")->setTexture("fbo_texture", bRenderer().getObjects()->getTexture("plane_texture"));
    bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("TALSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}));
    
    if(player_won){
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("winningSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}));
    }else if(player_lost){
        bRenderer().getModelRenderer()->drawModel(bRenderer().getObjects()->getModel("losingSprite"), modelMatrix, _viewMatrixHUD, vmml::Matrix4f::IDENTITY, std::vector<std::string>({}));
    }
}

/* Camera movement */
void RenderProject::updateCamera(const std::string &camera, const double &deltaTime)
{
    //// Adjust aspect ratio ////
    bRenderer().getObjects()->getCamera(camera)->setAspectRatio(bRenderer().getView()->getAspectRatio());
}

/* For iOS only: Handle device rotation */
void RenderProject::deviceRotated()
{
    if (bRenderer().isInitialized()){
        // set view to full screen after device rotation
        bRenderer().getView()->setFullscreen(true);
        bRenderer::log("Device rotated");
    }
}

/* For iOS only: Handle app going into background */
void RenderProject::appWillResignActive()
{
    if (bRenderer().isInitialized()){
        // stop the renderer when the app isn't active
        bRenderer().stopRenderer();
    }
}

/* For iOS only: Handle app coming back from background */
void RenderProject::appDidBecomeActive()
{
    if (bRenderer().isInitialized()){
        // run the renderer as soon as the app is active
        bRenderer().runRenderer();
    }
}

/* For iOS only: Handle app being terminated */
void RenderProject::appWillTerminate()
{
    if (bRenderer().isInitialized()){
        // terminate renderer before the app is closed
        bRenderer().terminateRenderer();
    }
}

/* Helper functions */
GLfloat RenderProject::randomNumber(GLfloat min, GLfloat max){
    return min + static_cast <GLfloat> (rand()) / (static_cast <GLfloat> (RAND_MAX / (max - min)));
}