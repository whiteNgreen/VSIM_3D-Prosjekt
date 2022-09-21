#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>

#include <string>

#include "Shaders/shader.h"
#include "mainwindow.h"
#include "logger.h"


RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)

{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);
}

RenderWindow::~RenderWindow()
{
    //cleans up the GPU memory
//    glDeleteVertexArrays( 1, &mVAO );
//    glDeleteBuffers( 1, &mVBO );
}


// Sets up the general OpenGL stuff and the buffers needed to render a triangle --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderWindow::init()
{
    //Get the instance of the utility Output logger
    //Have to do this, else program will crash (or you have to put in nullptr tests...)
    mLogger = Logger::getInstance();

    //Connect the gameloop timer to the render function:
    //This makes our render loop
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));
    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        mLogger->logText("makeCurrent() failed", LogType::REALERROR);
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info (what GPU is used):
    //Nice to see if you use the Intel GPU or the dedicated GPU on your laptop
    // - can be deleted
    mLogger->logText("The active GPU and API:", LogType::HIGHLIGHT);
    std::string tempString;
    tempString += std::string("  Vendor: ") + std::string((char*)glGetString(GL_VENDOR)) + "\n" +
            std::string("  Renderer: ") + std::string((char*)glGetString(GL_RENDERER)) + "\n" +
            std::string("  Version: ") + std::string((char*)glGetString(GL_VERSION));
    mLogger->logText(tempString);

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines - at least with NVidia GPUs
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);            //enables depth sorting - must then use GL_DEPTH_BUFFER_BIT in glClear
        glEnable(GL_CULL_FACE);       //draws only front side of models - usually what you want - test it out!
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);    //gray color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    //NB: hardcoded path to files! You have to change this if you change directories for the project.
    //Qt makes a build-folder besides the project folder. That is why we go down one directory
    // (out of the build-folder) and then up into the project folder.
    plainShader = new Shader("../VSIM_3D-Prosjekt/Shaders/plainshader.vert", "../VSIM_3D-Prosjekt/Shaders/plainshader.frag");
    plainShader->setShaderType(ShaderType::plain);

    textureShader = new Shader("../VSIM_3D-Prosjekt/Shaders/textureshader.vert", "../VSIM_3D-Prosjekt/Shaders/textureshader.frag");
    textureShader->setShaderType(ShaderType::phong);
    textureShader->getUniformMatrix("textureSampler");

    phongShader = new Shader("../VSIM_3D-Prosjekt/Shaders/phongshader.vert", "../VSIM_3D-Prosjekt/Shaders/phongshader.frag");
    phongShader->setShaderType(ShaderType::phong);
    phongShader->getUniformMatrix("mMatrix");
    phongShader->getUniformMatrix("pMatrix");
    phongShader->getUniformMatrix("vMatrix");
    phongShader->getUniformMatrix("textureSampler");
    phongShader->getUniformMatrix("ambientStrength");
    phongShader->getUniformMatrix("lightPosition");
    phongShader->getUniformMatrix("lightColor");
    phongShader->getUniformMatrix("lightStrength");
    phongShader->getUniformMatrix("specularStrength");
    phongShader->getUniformMatrix("specularExponent");
    phongShader->getUniformMatrix("objectColor");
    phongShader->getUniformMatrix("cameraPosition");


    cubeMapShader = new Shader("../VSIM_3D-Prosjekt/Shaders/cubemapShader.vert", "../VSIM_3D-Prosjekt/Shaders/cubemapShader.frag");
    cubeMapShader->setShaderType(ShaderType::cubemap);
    cubeMapShader->getUniformMatrix("view");
    cubeMapShader->getUniformMatrix("projection");
    cubeMapShader->getUniformMatrix("skybox");


    mCamera = new Camera();


    // Sets up texture map
    mTextureMap.insert({"DummyTexture", new Texture()});
    mTextureMap.insert({"RedTrophy", new Texture("RedTrophy.jpg")});
    mTextureMap.insert({"BlueTrophy", new Texture("BlueTrophy.jpg")});
    mTextureMap.insert({"Spiller", new Texture("spiller/Spiller Base Color.jpg")});
    mTextureMap.insert({"NPC", new Texture("spiller/NPC Base Color.jpg")});
    mTextureMap.insert({"RedWon", new Texture("RedWon.png")});
    mTextureMap.insert({"BlueWon", new Texture("BlueWon.png")});
    mTextureMap.insert({"Monkey", new Texture("MonkeyBilde.jpg")});
    mTextureMap.insert({"White", new Texture("WhiteTexture.jpg")});
    mTextureMap.insert({"TerrainTexture", new Texture("HeightmapTexture.png")});


    quadtree = new Quadtree(plainShader);

    /* World Axis. Visible only in editor mode */
    Axis = new SimpleObject(Type::Axis, plainShader);
    Axis->init();


    terrain = new Terrain(phongShader);
    terrain->init();
    auto tmp = new Texture("HeightmapTexture.png");
    terrain->setTexture(tmp->getTexture("HeightmapTexture.png"));
    terrain->setTexture(mTextureMap["TerrainTexture"]->textureID);
//    mMap.insert({"terrain", terrain});


    /* Importing a sphere.obj to the light.
     * Sets the initial translation to 20 in the z direction */
//    light = new Light(new ObjMesh("Sphere.obj", plainShader), plainShader);
//    light->init();
//    light->mMatrix.translate(0, 0, 20);
//    mMap.insert({"light", light});

//    SimpleObject* FlatGround = new SimpleObject(Type::Plane, 10.f, plainShader);
//    FlatGround->init();
//    FlatGround->mMatrix.rotate(-90.f, 1, 0 ,0);
//    mMap.insert({"FlatGround", FlatGround});


    /* Bakken */
    Bakken = new Bakke(plainShader);

    /* Oblig 1 Surface */
//    {
//        /* Vertex punkter */
//        QVector3D a{  0.f, 30.f,   8.6f };        // A
//        QVector3D b{  0.f,  0.f,   0.f };
//        QVector3D c{ 30.f,  0.f, 6.4f };      // B
//        QVector3D d{ 30.f, 30.f,    0.f };
//        QVector3D e{ 60.f, 0.f,   0.f };
//        QVector3D f{ 60.f, 30.f,   4.f };        // C

//        /* Første quad */
//        Bakken->CreateTriangle(a, b, d);
//        Bakken->CreateTriangle(b, c, d);
//        /* Andre quad */
//        Bakken->CreateTriangle(d, c, e);
//        Bakken->CreateTriangle(d, e, f);
//    }

    /* Flat overflate med en bakke på enden */
    {
        float Width{ 10.f };
        /* Flat overflate */
        float FlatLength{8.f};

            QVector3D a{ 0, 0, 0 };
            QVector3D b{ FlatLength, 0, 0 };
            QVector3D c{ FlatLength, Width, 0 };
            QVector3D d{ 0, Width, 0 };

            Bakken->CreateTriangle(a, b, c);
            Bakken->CreateTriangle(d, a, c);

        /* Bakke på enden av flaten */
        float SlopeLength{8.f};
        float SlopeHeight{8.f};

            QVector3D e{ SlopeLength + FlatLength, 0, SlopeHeight };
            QVector3D f{ SlopeLength + FlatLength, Width, SlopeHeight };

            Bakken->CreateTriangle(c, e, f);
            Bakken->CreateTriangle(c, b, e);
    }


    Bakken->init();


    /* Ballen */
    Ball = new OctahedronBall(3, 1.f);
    Ball->m_shader = plainShader;
    Ball->init();
//    StartPosition = {a.x() + 1, a.y() - 1, a.z()};
    StartPosition = { 0, 2, Ball->GetRadius() + 2 };
    StartVelocity = { 0, 0, 0 };
    Ball->PreSim_MoveTo(StartPosition, Bakken);
    Ball->SetStartVelocity(StartVelocity);
    mMap.insert({"Ball", Ball});



    /* Skybox / CubeMap */
    cubemap = new CubeMap(cubeMapShader);



    /* VELOCITY TEST */
//    QVector3D a{2, -3, 5};
//    QVector3D b{3, 6, -4};
//    QVector3D proj = (QVector3D::dotProduct(a,b)/QVector3D::dotProduct(b,b))*b;
//    qDebug() << "proj: " << proj;


//    /* Projeksjonen av Velocity på b (orthogonal til n på vei mot Velocity)
//     * For å kunne finne hastigheten en vektor burde ha parallelt med bakken når den lander     //vProj
//     * OG for å finne hastigheten ballen skal ha perpendikulært med bakken                      //hProj
//     * */
//    float F = 0;    // Forenklet friksjons akselerasjon
//    float E = 0.2;
//    QVector3D Vc{ 0, 1, -7.67 };    // Inkommende Hastigheten
//    QVector3D V{};                  // Ny Hastighet
//    QVector3D n{ 0.707, 0, 0.707 }; // SurfaceNormal
//    QVector3D B{ QVector3D::crossProduct(n, QVector3D::crossProduct(Vc.normalized(), n)) };
//    QVector3D vProj = (QVector3D::dotProduct(Vc,B)/QVector3D::dotProduct(B,B))*B;
//    QVector3D hProj = (QVector3D::dotProduct(Vc,n)/QVector3D::dotProduct(n,n))*n;
//    V = (vProj - (vProj*F)) + (n * hProj.length() * E);

//    /* Hvis vinkelen mellom Hastigheten*-1 og SurfaceNormal er 0
//     * så reverserer vi bare Hastigheten og ganger med Elasticity
//     * */
//    if (QVector3D::dotProduct(Vc.normalized()*-1, n) == 1)
//    {
//        V = Vc * -1 * E;
//    }
//    qDebug() << "vProj: " << vProj;
//    qDebug() << "hProj: " << hProj;
//    qDebug() << "V: " << V;


    /* Setter mMainWindow verdier til GUI elementene til å matche det jeg hardkoder her */
    if (mMainWindow)
    {
        mMainWindow->OnStart();
        mMainWindow->SetBallStartPositionText(StartPosition);
        mMainWindow->SetBallStartVelocityText(StartVelocity);
    }

    glBindVertexArray(0);       //unbinds any VertexArray - good practice
}



// Called each frame - doing the rendering!!! -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderWindow::render()
{
    float DeltaTime = (mTimeStart.nsecsElapsed())/1000000000.f;
//    mLogger->logText("DeltaTime" + std::to_string(DeltaTime));

    handleInput();

    mCamera->update();
    mCamera->mProjectionMatrix.perspective(60.f, mAspectratio, 0.1, 10000);


    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    checkForGLerrors();

    /* Calculate Physics for mMap objects */
    if (bSimulate || bGoNextFrame)
    {
//        static float movement{};
//        movement += DeltaTime;
//        Ball->mMatrix.translate(sinf(movement) / 10.f, 0, 0);

        ElapsedTime += DeltaTime;

        Ball->CalculatePhysics(Bakken, DeltaTime);

        /* Viser ballens nåverende posisjon */
        if (mMainWindow)
        {
            mMainWindow->SetBallCurrentPositionText(Ball->getPositionVector3D());
            mMainWindow->SetElapsedTime(ElapsedTime);
        }

        bGoNextFrame = false;
    }


    /* Draw mMap objects */
    for (const auto& it : mMap)
    {
        it.second->draw();
    }
    Bakken->draw();
//    Bakken->drawLines();  // Linjene burde være hvite eller noe sånt så de kan faktisk ses sammen med meshen

        /* Draw other objects */
        Axis->draw(mCamera->mProjectionMatrix, mCamera->mViewMatrix);

    // Draw Cubemap
//    QMatrix4x4 view = mCamera->mViewMatrix;
//    view.translate(mCamera->mPosition);
//    view.rotate(-90, 1, 0, 0);
//    cubemap->draw(mCamera->mProjectionMatrix, view);


    //Calculate framerate before
    // checkForGLerrors() because that call takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    mContext->swapBuffers(this);
}


//This function is called from Qt when window is exposed (shown)
// and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *)
{
    //if not already initialized - run init() function - happens on program start up
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();

    //Set viewport width and height to the size of the QWindow we have set up for OpenGL
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed())
    {
        //This timer runs the actual MainLoop == the render()-function
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact...)
        mRenderTimer->start(16);
        mTimeStart.start();
    }

    mAspectratio = static_cast<float>(width()) / height();
}

// -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderWindow::togglePlay(bool bPlay)
{
//    mCamera->setFollowPlayer(bPlay);
    bGameMode = bPlay;
}

void RenderWindow::togglePause(bool bPause)
{
    bSimulate = bPause;
    qDebug() << "Playing: " << bSimulate;
}

void RenderWindow::GoNextFrame()
{
    if (!bGoNextFrame){ bGoNextFrame = true; }
}

void RenderWindow::Reset()
{
    Ball->Reset(Bakken, StartPosition, StartVelocity);

    ElapsedTime = 0.f;
    if (mMainWindow)
    {
        mMainWindow->SetBallCurrentPositionText(Ball->getPositionVector3D());
        mMainWindow->SetElapsedTime(ElapsedTime);
    }
}

void RenderWindow::ChangeBallStartPosition()
{
    if (!bSimulate)
    {

        if (Ball)
        {
            Ball->PreSim_MoveTo(StartPosition, Bakken);
        }
    }
}

/* Setter start posisjonen med GUI elementene */
void RenderWindow::ChangeBallStartPositionX(const float x)
{
    StartPosition.setX(x);
    ChangeBallStartPosition();
}

void RenderWindow::ChangeBallStartPositionY(const float y)
{
    StartPosition.setY(y);
    ChangeBallStartPosition();
}

void RenderWindow::ChangeBallStartPositionZ(const float z)
{
    StartPosition.setZ(z);
    ChangeBallStartPosition();
}

void RenderWindow::ChangeBallStartVelocity()
{
    if (!bSimulate)
    {
        if (Ball)
        {
            Ball->SetStartVelocity(StartVelocity);
        }
    }
}

/* Setter start hastigheten med GUI elementene */
void RenderWindow::ChangeBallStartVelocityX(const float x)
{
    StartVelocity.setX(x);
    ChangeBallStartVelocity();
}

void RenderWindow::ChangeBallStartVelocityY(const float y)
{
    StartVelocity.setY(y);
    ChangeBallStartVelocity();
}

void RenderWindow::ChangeBallStartVelocityZ(const float z)
{
    StartVelocity.setZ(z);
    ChangeBallStartVelocity();
}



//The way this function is set up is that we start the clock before doing the draw call,----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate()
{
    long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)            //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30)    //once pr 30 frames = update the message == twice pr second (on a 60Hz monitor)
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed/1000000.f, 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            frameCount = 0;     //reset to show a new message in 30 frames
        }
    }
}

//Uses QOpenGLDebugLogger if this is present
//Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)  //if our machine got this class to work
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType)) // get rid of uninteresting "object ...
                                                        // will use VIDEO memory as the source for
                                                        // buffer object operations"
                // valid error message:
                mLogger->logText(message.message().toStdString(), LogType::REALERROR);
        }
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            mLogger->logText("glGetError returns " + std::to_string(err), LogType::REALERROR);
            switch (err) {
            case 1280:
                mLogger->logText("GL_INVALID_ENUM - Given when an enumeration parameter is not a "
                                "legal enumeration for that function.");
                break;
            case 1281:
                mLogger->logText("GL_INVALID_VALUE - Given when a value parameter is not a legal "
                                "value for that function.");
                break;
            case 1282:
                mLogger->logText("GL_INVALID_OPERATION - Given when the set of state for a command "
                                "is not legal for the parameters given to that command. "
                                "It is also given for commands where combinations of parameters "
                                "define what the legal parameters are.");
                break;
            }
        }
    }
}

//Tries to start the extended OpenGL debugger that comes with Qt
//Usually works on Windows machines, but not on Mac...
void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            mLogger->logText("This system can not use QOpenGLDebugLogger, so we revert to glGetError()",
                             LogType::HIGHLIGHT);

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            mLogger->logText("This system can log extended OpenGL errors", LogType::HIGHLIGHT);
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                mLogger->logText("Started Qt OpenGL debug logger");
        }
    }
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void RenderWindow::handleInput()
{
    forward = 0.f;
    right = 0.f;
    up = 0.f;

    if (mKeyboard[Qt::Key_W])
        forward += 1.f;
    if (mKeyboard[Qt::Key_S])
        forward += -1.f;
    if (mKeyboard[Qt::Key_D])
        right += 1.f;
    if (mKeyboard[Qt::Key_A])
        right += -1.f;
    if (mKeyboard[Qt::Key_E])
        up += 1.f;
    if (mKeyboard[Qt::Key_Q])
        up += -1.f;

    if (mMouse[Qt::MouseButton::RightButton]){
        mCamera->move(right, forward, up);
    }

    if (mKeyboard[Qt::Key_Alt])
        light->mMatrix.translate( right * .2f, forward * .2f, up * .2f );


    if (mKeyboard[Qt::Key_1]){
        mMap["Ball"]->mMatrix.translate(0, 0, 10);
    }
    if (mKeyboard[Qt::Key_2]){
//        mCamera->setFollowPlayer(false);
    }


    if (!bSimulate)
    {
        forward = 0;
        right = 0;
    }
}

void RenderWindow::mousePressEvent(QMouseEvent *event)
{
    mMouse[event->button()] = true;
}

void RenderWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mMouse[event->button()] = false;
}

void RenderWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mMouse[Qt::MouseButton::RightButton]){
        mCamera->Pitch(event->pos().y() - lastMouseY);
        mCamera->Yaw(event->pos().x() - lastMouseX);
    }
    lastMouseX = event->pos().x();
    lastMouseY = event->pos().y();


}

//Event sent from Qt when program receives a keyPress
// NB - see renderwindow.h for signatures on keyRelease and mouse input
void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        mMainWindow->close();       //Shuts down the whole program
    }

    mKeyboard[event->key()] = true;
}

void RenderWindow::keyReleaseEvent(QKeyEvent *event)
{
    mKeyboard[event->key()] = false;
}
