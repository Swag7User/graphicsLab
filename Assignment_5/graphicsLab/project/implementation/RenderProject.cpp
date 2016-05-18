#include "RenderProject.h"
#ifdef __OBJC__
#import <CoreMotion/CoreMotion.h>
#endif
vmml::Matrix4f modelMatrixTerrain = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 5.5f));
;
vmml::Matrix4f modelMatrixTAL = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 0.f)) * vmml::create_scaling(vmml::Vector3f(1.f))*vmml::create_rotation((float)(90*M_PI_F/180), vmml::Vector3f::UNIT_X)*vmml::create_rotation((float)(180*M_PI_F/180), vmml::Vector3f::UNIT_Z);
vmml::Matrix4f modelMatrixZep = vmml::create_translation(vmml::Vector3f(0.0f, 100.0f, 500.f))*vmml::create_scaling(vmml::Vector3f(0.1f,0.1f,0.1f));
vmml::Matrix4f modelMatrixSKY = vmml::create_translation(modelMatrixTerrain.get_translation()) * vmml::create_scaling(vmml::Vector3f(1.f));
vmml::AABBf hit_TAL;
vmml::AABBf hit_Zep;
vmml::AABBf hit_Terrain;
ShaderPtr guyShader;
ShaderPtr TALShader;
ShaderPtr ZepShader;
ShaderPtr SKYShader;
PropertiesPtr guyProperties;
PropertiesPtr TALProperties;
PropertiesPtr ZepProperties;
PropertiesPtr SKYProperties;

double _time = 0;
double _pitchSum;
//CMMotionManager *cmMotionManager=CMMotionManager();
float angle=0.f;
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
    
    // set shader versions (optional)
    bRenderer().getObjects()->setShaderVersionDesktop("#version 120");
    bRenderer().getObjects()->setShaderVersionES("#version 100");
    
    // load materials and shaders before loading the model
    guyShader = bRenderer().getObjects()->loadShaderFile("guy", 0, false, false, false, false, false);				// load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    TALShader = bRenderer().getObjects()->loadShaderFile("TAL", 0, false, false, false, false, false);
    
    ZepShader = bRenderer().getObjects()->loadShaderFile("Zep", 0, false, false, false, false, false);
    SKYShader = bRenderer().getObjects()->loadShaderFile("SKY", 0, false, false, false, false, false);


    
    // create additional properties for a model
    guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    TALProperties = bRenderer().getObjects()->createProperties("TALProperties");
    ZepProperties = bRenderer().getObjects()->createProperties("ZepProperties");
    SKYProperties = bRenderer().getObjects()->createProperties("SKYProperties");
    
    // load model
    //bRenderer().getObjects()->loadObjModel("guy.obj", true, true, true, 0, false, false, guyProperties);
    hit_Terrain=bRenderer().getObjects()->loadObjModel("Terrain_50000.obj", false, true, guyShader, guyProperties)->getBoundingBoxObjectSpace();
    hit_TAL=bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties)->getBoundingBoxObjectSpace();
    hit_Zep=bRenderer().getObjects()->loadObjModel("Zep.obj", false, true, ZepShader, ZepProperties)->getBoundingBoxObjectSpace();
    // automatically generates a shader with a maximum of 4 lights (number of lights may vary between 0 and 4 during rendering without performance loss)
    bRenderer().getObjects()->loadObjModel("skybox.obj", false, true, SKYShader, SKYProperties);
    
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
    
    _time += deltaTime;
    float angle = _time * 0.9;
    int timer = 1;
    int start = 2;
    
    _pitchSum += bRenderer().getInput()->getGyroscopePitch()* 1.0f;
    float rotX=-(float)(bRenderer().getInput()->getGyroscopeRoll()/150);
    float rotY=(float)(bRenderer().getInput()->getGyroscopePitch()/32);
    
    vmml::Matrix4f rotationX = vmml::create_rotation(rotX, vmml::Vector3f::UNIT_X);
    vmml::Matrix4f rotationY = vmml::create_rotation(rotY, vmml::Vector3f::UNIT_Y);
    vmml::Matrix4f rotationZ = vmml::create_rotation(0.0f, vmml::Vector3f::UNIT_Z);
    //print("EY THIS IS ROATION:"+bRenderer().);
    /*** Guy ***/
    // get input rotation
    TouchMap touchMap = bRenderer().getInput()->getTouches();
    int i = 0;
    float rotation = 0.0f;
    for (auto t = touchMap.begin(); t != touchMap.end(); ++t)
    {
        Touch touch = t->second;
        rotation = (touch.currentPositionX - touch.startPositionX) / 100;
        if (++i > 1)
            break;
    }
    // get input rotation
    TouchMap touchMap2 = bRenderer().getInput()->getTouches();
    int i2 = 0;
    float rotation2 = 0.0f;
    for (auto t = touchMap2.begin(); t != touchMap2.end(); ++t)
    {
        Touch touch = t->second;
        rotation2 = (touch.currentPositionY - touch.startPositionY) / 100;
        if (++i2 > 1)
            break;
    }
    
    
    
    // TODO: implement solar system here
    
    //vmml::Matrix4f modelMatrixTerrain = vmml::create_scaling(vmml::Vector3f(0.6f));
   // vmml::Matrix4f rotationMatrixView = vmml::create_rotation(5.0f, vmml::Vector3f::UNIT_X);
    
    
    vmml::Matrix4f viewMatrix = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    
    
    // translate, rotate and scale
//    if (hit_Terrain.isIn(modelMatrixTAL.get_translation())|hit_Zep.isIn(modelMatrixTAL.get_translation())) {
//        modelMatrixTAL *= vmml::create_translation(vmml::Vector3f(0.0f, -1.0f, 0.0f));
//    }
    modelMatrixTAL *= vmml::create_translation(vmml::Vector3f(0.0f, -1.0f, 0.0f));

    
    vmml::Vector3f camTranslation = modelMatrixTAL.get_translation();
    
    bRenderer().getObjects()->getCamera("camera")->setPosition(-(camTranslation));
    
    
    vmml::Matrix4f rotationMatrix = vmml::create_rotation(rotation, vmml::Vector3f::UNIT_Y);
    //modelMatrixTerrain *= rotationMatrix;
    //modelMatrixTAL *= rotationMatrix;
    //viewMatrix*=rotationMatrix;
    //viewMatrix*=rotationMatrix;
    

//    bRenderer().getObjects()->getCamera("camera")->moveCameraForward(cos(rotation)*-10.0f);
    
    
    
    bRenderer().getObjects()->getCamera("camera")->lookAt(bRenderer().getObjects()->getCamera("camera")->getPosition(), modelMatrixTAL.get_translation(), vmml::Vector3f(0.0f, 0.0f,0.0f));
    
    
    
    
    rotationMatrix = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    //modelMatrixTAL *= rotationMatrix;
    //viewMatrix*=rotationMatrix;
    //modelMatrixTerrain *= rotationMatrix;
    bRenderer().getObjects()->getCamera("camera")->rotateCamera(rotation2/100, 0.0f, 0.0f);

    bRenderer().getObjects()->getCamera("camera")->rotateCamera(0.0f, rotation/100, 0.0f);
    
    bRenderer().getObjects()->getCamera("camera")->moveCameraForward(cos(rotation2/100)*-10.0f);
    //bRenderer().getObjects()->getCamera("camera")->moveCameraSideward(cos(rotation2)*10.0f*sin(rotation));
    
    bRenderer().getObjects()->getCamera("camera")->moveCameraUpward(sin(rotation2/100)*10.0f);
    
    camTranslation.z() = camTranslation.z() - 10.0f;
    
    
    //modelMatrixTAL = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    
    
    //move plane
    angle++;
    
    //vmml::Vector3f planeChange=vmml::Vector3f(0.f,0.f,angle/50*10.f);
    
    //vmml::Matrix4f planeMotion=vmml::create_translation(planeChange);
    //modelMatrixTAL *=planeMotion;
    //move camer with plane
//    modelMatrixTAL *= vmml::create_rotation((float)(rotation2*M_PI_F/180), vmml::Vector3f::UNIT_X);
//    modelMatrixTAL *= vmml::create_rotation((float)(rotation*M_PI_F/180), vmml::Vector3f::UNIT_Y);
    vmml::Matrix4f rotationMatrixTAL = rotationX*rotationY;//*rotationZ;
    modelMatrixTAL *= rotationMatrixTAL;
    
 //   vmml::Vector3f cameraPos=bRenderer().getObjects()->getCamera("camera")->getPosition();
    
    //cameraPos=vmml::Vector3f(cameraPos.x()-planeChange.x()+0.0f,-planeChange.y()+cameraPos.y()-0.f,-planeChange.z()+cameraPos.z());
    //bRenderer().getObjects()->getCamera("camera")->setPosition(cameraPos);
    
    //modelMatrixTerrain *= modelMatrixTAL;
    
    modelMatrixSKY = vmml::create_translation(modelMatrixTAL.get_translation())*vmml::create_translation(vmml::Vector3f(0.0f, -225.0f,2000.0f))*vmml::create_scaling(vmml::Vector3f(1500.f));
    
    //turn plane right
    hit_Terrain=bRenderer().getObjects()->loadObjModel("Terrain_50000.obj", false, true, guyShader, guyProperties)->getBoundingBoxObjectSpace();
    hit_TAL=bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties)->getBoundingBoxObjectSpace();
    hit_Zep=bRenderer().getObjects()->loadObjModel("Zep.obj", false, true, ZepShader, ZepProperties)->getBoundingBoxObjectSpace();
    
    

    
    ShaderPtr shader = bRenderer().getObjects()->getShader("guy");

    
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTerrain", modelMatrixTerrain);

        
        vmml::Matrix3f normalMatrix;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTerrain)), normalMatrix);
        shader->setUniform("NormalMatrix", normalMatrix);

  
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        shader->setUniform("EyePos", eyePos);
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 3.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 1.f, .5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
        shader->setUniform("PosPlane", vmml::Vector3f(modelMatrixTAL.get_translation()));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    
    shader = bRenderer().getObjects()->getShader("TAL");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixTAL", modelMatrixTAL);
        
        
        vmml::Matrix3f normalMatrixTAL;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixTAL)), normalMatrixTAL);
        shader->setUniform("NormalMatrixTAL", normalMatrixTAL);
        
        
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        shader->setUniform("EyePos", eyePos);
                
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 3.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 1.f, .5f, 1.f));
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
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixZep", modelMatrixZep);
        
        
        vmml::Matrix3f normalMatrixZep;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixZep)), normalMatrixZep);
        shader->setUniform("NormalMatrixZep", normalMatrixZep);
        
        
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        shader->setUniform("EyePos", eyePos);
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 3.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 1.f, .5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }
    shader = bRenderer().getObjects()->getShader("SKY");
    if (shader.get())
    {
        shader->setUniform("ProjectionMatrix", vmml::Matrix4f::IDENTITY);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("ViewMatrix", viewMatrix);
        shader->setUniform("modelMatrixSKY", modelMatrixSKY);
        
        
        vmml::Matrix3f normalMatrixSKY;
        vmml::compute_inverse(vmml::transpose(vmml::Matrix3f(modelMatrixSKY)), normalMatrixSKY);
        shader->setUniform("NormalMatrixSKY", normalMatrixSKY);
        
        
        
        vmml::Vector4f eyePos = vmml::Vector4f(0.0f, 0.0f, 10.0f, 1.0f);
        shader->setUniform("EyePos", eyePos);
        
        shader->setUniform("LightPos", vmml::Vector4f(.5f, 1.f, 3.5f, 1.f));
        shader->setUniform("LightPos2", vmml::Vector4f(1.f, 1.f, .5f, 1.f));
        shader->setUniform("Ia", vmml::Vector3f(1.f));
        shader->setUniform("Id", vmml::Vector3f(1.f));
        shader->setUniform("Is", vmml::Vector3f(1.f));
    }
    else
    {
        bRenderer::log("No shader available.");
    }



    

    
    

    
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("Terrain_50000", "camera", modelMatrixTerrain, std::vector<std::string>({ }));
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("TAL16OBJ", "camera", modelMatrixTAL, std::vector<std::string>({ }));
    bRenderer().getModelRenderer()->drawModel("Zep", "camera", modelMatrixZep, std::vector<std::string>({ }));
    bRenderer().getModelRenderer()->drawModel("skybox", "camera", modelMatrixSKY, std::vector<std::string>({ }));

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