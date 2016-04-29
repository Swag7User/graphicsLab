#include "RenderProject.h"
int speed;
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
    ShaderPtr guyShader = bRenderer().getObjects()->loadShaderFile("guy", 0, false, false, false, false, false);				// load shader from file without lighting, the number of lights won't ever change during rendering (no variable number of lights)
    ShaderPtr TALShader = bRenderer().getObjects()->loadShaderFile("TAL", 0, false, false, false, false, false);
    
    // create additional properties for a model
    PropertiesPtr guyProperties = bRenderer().getObjects()->createProperties("guyProperties");
    PropertiesPtr TALProperties = bRenderer().getObjects()->createProperties("TALProperties");
    
    // load model
    //bRenderer().getObjects()->loadObjModel("guy.obj", true, true, true, 0, false, false, guyProperties);
    bRenderer().getObjects()->loadObjModel("Terrain_50000.obj", false, true, guyShader, guyProperties);
    bRenderer().getObjects()->loadObjModel("TAL16OBJ.obj", false, true, TALShader, TALProperties);
    // automatically generates a shader with a maximum of 4 lights (number of lights may vary between 0 and 4 during rendering without performance loss)
    
    // create camera
    bRenderer().getObjects()->createCamera("camera", vmml::Vector3f(0.0f, 0.0f, 0.0f), vmml::Vector3f(0.f, 0.f, 0.f));
    
    
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
    /*** solar system ***/
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
        Touch touch2 = t->second;
        rotation2 = (touch2.currentPositionY - touch2.startPositionY) / 100;
        if (++i2 > 1)
            break;
    }
    
    
    
    // TODO: implement solar system here
    
    //vmml::Matrix4f modelMatrixTerrain = vmml::create_scaling(vmml::Vector3f(0.6f));
    vmml::Matrix4f viewMatrix = bRenderer().getObjects()->getCamera("camera")->getViewMatrix();
    
    
    // translate and scale
    vmml::Matrix4f modelMatrixTerrain = vmml::create_translation(vmml::Vector3f(1000.0f, 0.0f, 0.f));
    vmml::Matrix4f rotationMatrix = vmml::create_rotation(rotation, vmml::Vector3f::UNIT_Y);
    modelMatrixTerrain *= rotationMatrix;
    rotationMatrix = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    modelMatrixTerrain *= rotationMatrix;
    
    vmml::Matrix4f modelMatrixTAL = vmml::create_translation(vmml::Vector3f(0.0f, 0.0f, 5.5f)) * vmml::create_rotation((float) 90*M_PI_F/180, vmml::Vector3f::UNIT_X)*vmml::create_rotation((float) 180*M_PI_F/180, vmml::Vector3f::UNIT_Z)* vmml::create_scaling(vmml::Vector3f(5.f));
    
    speed++;
    vmml::Matrix4f rotationMatrixTAL = vmml::create_translation(vmml::Vector3f(0.0f,-speed,0.0f)) * vmml::create_rotation(rotation, vmml::Vector3f::UNIT_Y);
    modelMatrixTAL *= rotationMatrixTAL;
    rotationMatrixTAL = vmml::create_rotation(rotation2, vmml::Vector3f::UNIT_X);
    modelMatrixTAL *= rotationMatrixTAL;
    

    
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


    
    
    bRenderer().getObjects()->getCamera("camera")->setPosition(vmml::Vector3f(0.f,0.f,30.f));

    
    

    
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("Terrain_50000", "camera", modelMatrixTerrain, std::vector<std::string>({ }));
    //shader->setUniform("NormalMatrix", vmml::Matrix3f(modelMatrixTerrain));
    bRenderer().getModelRenderer()->drawModel("TAL16OBJ", "camera", modelMatrixTAL*speed, std::vector<std::string>({ }));
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