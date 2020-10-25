#include "bball.h"
#include "G3D-base/FileSystem.h"

int main(int argc, const char *argv[]) {
    G3D::initGLG3D();

    G3D::GApp::Settings settings(argc, argv);

    settings.window.caption = "BouncyBall-G3D";
    settings.window.width = 1920;
    settings.window.height = 1080;
    settings.dataDir = G3D::FileSystem::NFDStandardizeFilename("../data-files");

    return G3D::BallApp(settings).run();
}

namespace G3D {
const CFrame
BallApp::computePlayerMotionFrame(const shared_ptr<Entity> player) {

    /** The y-coordinate of both these points are more or less the same for
     * normal motion except when they jump */
    Point3 previousPos = player->previousFrame().translation;
    Point3 currentPos = player->frame().translation;

    Point3 diff = currentPos - previousPos;
    Point2 diffXZ(diff.x, diff.z);
    /** We only care if the position of the ball has changed at all in the XZ
     * space and only compute a frame when we see a non-zero difference,
     * otherwise return the previously updated frame */
    if (diffXZ.length() > 0.01) {
        Point2 dirXZ = diffXZ.fastDirection();
        float yaw = -acos(dirXZ.x);
        /** If the z-value is negative, then invert the angle since dot product
         * is not good enough for this */
        if (dirXZ.y < 0)
            yaw *= -1;

        /** Assiging current Position should also take care of the jumping
         * issues */
        m_scene->m_playerMotion = CFrame::fromXYZYPRRadians(
            currentPos.x, currentPos.y, currentPos.z, yaw, 0, 0);
        m_isPlayerMoving = true;
    } else {
        m_isPlayerMoving = false;
    }
    return m_scene->m_playerMotion;
}

BallApp::BallApp(const GApp::Settings &settings) : GApp(settings) {}

void BallApp::onInit() {
    // create screen capture after data directory is set and opengl initialized
    const shared_ptr<GFont> &devFont = GFont::fromFile(
        System::findDataFile(m_settings.developerToolsFontName));
    const shared_ptr<GuiTheme> &devTheme = GuiTheme::fromFile(
        System::findDataFile(m_settings.developerToolsThemeName), devFont);
    auto sc = screenCapture();
    sc = new ScreenCapture(devTheme, this);

    m_gbufferSpecification.encoding[GBuffer::Field::DEPTH_AND_STENCIL] =
        ImageFormat::DEPTH32();
    m_gbufferSpecification.depthEncoding = DepthEncoding::HYPERBOLIC;

    m_gbufferSpecification.encoding[GBuffer::Field::WS_NORMAL] =
        ImageFormat::RGB16F();

    // Called before the application loop begins.  Load data here and
    // not in the constructor so that common exceptions will be
    // automatically caught.
    showRenderingStats = false;

#ifdef G3D_DEBUG
    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);
    if (m_settings.useDeveloperTools) {
        // For now pass the nullptr
        createDeveloperHUD();
    }
#endif

    debugAssert(notNull(m_ambientOcclusion));
    m_scene = PhysicsScene::create(m_ambientOcclusion);
    // Allowing custom Entity subclasses to be parsed from .Scene.Any files
    m_scene->registerEntitySubclass("PhysicsEntity", &PhysicsEntity::create);
    m_scene->registerEntitySubclass("GhostEntity", &GhostEntity::create);

    setScene(m_scene);

    // Detect scene files in additional data directories.
    if (!dataDirsAddedToScene) {
        Array<String> additionalDataDirs = {dataDir};
        for (const String &s : dataDirs) {
            additionalDataDirs.append(s);
        }
        scene()->appendSceneSearchPaths(additionalDataDirs);
        dataDirsAddedToScene = true;
    }

#ifdef G3D_DEBUG
    const shared_ptr<GFont> &arialFont =
        GFont::fromFile(System::findDataFile("arial.fnt"));
    const shared_ptr<GuiTheme> &theme =
        GuiTheme::fromFile(System::findDataFile("osx-10.7.gtm"), arialFont);
    developerWindow->sceneEditorWindow =
        PhysicsSceneEditorWindow::create(this, m_scene, theme);
    // Add some offset to y-height
    auto pos = developerWindow->cameraControlWindow->rect();
    developerWindow->sceneEditorWindow->moveTo(pos.x0y1() + Vector2(0, 15));

    makeGUI();

    developerWindow->cameraControlWindow->moveTo(
        Point2(developerWindow->cameraControlWindow->rect().x0(), 0));
    developerWindow->sceneEditorWindow->setPreventEntitySelect(false);
    developerWindow->setVisible(false);
#endif

    loadScene("Level");
    setActiveCamera(m_scene->typedEntity<Camera>("camera"));
    //m_scene->addBoxArray("box", Vector2(10, 5), Vector3(10, 0, 0),
    //                     Vector3(0, 0, 1));
}

void BallApp::makeGUI() {
    debugWindow->setVisible(false);
    developerWindow->videoRecordDialog->setEnabled(true);
    developerWindow->sceneEditorWindow->setVisible(false);
    developerWindow->cameraControlWindow->setVisible(false);

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(),
                                      debugWindow->rect().height()));
}

bool BallApp::onEvent(const GEvent &event) {
    // Handle super-class events
    if (GApp::onEvent(event)) {
        return true;
    }

    if ((event.type == GEventType::KEY_DOWN) &&
        (event.key.keysym.sym == GKey::TAB)) {
        const shared_ptr<Camera> &camera =
            m_debugCam ? m_scene->defaultCamera() : debugCamera();
        m_debugCam = !m_debugCam;
        setActiveCamera(camera);
    }

#ifdef G3D_DEBUG
    if ((event.type == GEventType::KEY_DOWN) &&
        (event.key.keysym.sym == GKey('e'))) {
        auto edit = !(m_scene->editing());
        m_scene->setEditing(edit);
        auto isVisible = !(developerWindow->sceneEditorWindow->visible());
        developerWindow->sceneEditorWindow->setVisible(isVisible);
    }
#endif

    return false;
}

void BallApp::onUserInput(UserInput *ui) {
    GApp::onUserInput(ui);
    ui->setPureDeltaMouse(m_editMode);
    if (!m_debugCam) {
        shared_ptr<Entity> player = m_scene->getPlayer();
        const shared_ptr<Camera> camera = m_scene->defaultCamera();

        /** Use this motionFrame object to compute the force required in world
         * space */
        CFrame motionFrame = computePlayerMotionFrame(player);
        Point3 forwardForce = motionFrame.vectorToWorldSpace(Point3(2.5, 0, 0));
        Point3 backwardForce =
            motionFrame.vectorToWorldSpace(Point3(-2.5, 0, 0));
        Point3 leftForce = motionFrame.vectorToWorldSpace(Point3(0, 0, -1.5));
        Point3 rightForce = motionFrame.vectorToWorldSpace(Point3(0, 0, 1.5));

        /** Cannot move forward and reverse at the same time and give priority
         * to forward motion */
        if (ui->keyDown(GKey('w'))) {
            m_scene->getPhysicsEngine()->applyForce(player.get(), forwardForce);

        } else if (ui->keyDown(GKey('s')) && m_isPlayerMoving) {
            m_scene->getPhysicsEngine()->applyForce(player.get(),
                                                    backwardForce);
        }
        /** Similar flow for left and right motions */
        if (ui->keyDown(GKey('a')) && m_isPlayerMoving) {
            m_scene->getPhysicsEngine()->applyForce(player.get(), leftForce);
        } else if (ui->keyDown(GKey('d')) && m_isPlayerMoving) {
            m_scene->getPhysicsEngine()->applyForce(player.get(), -leftForce);
        }
        if (ui->keyDown(GKey::SPACE)) {
            m_scene->getPhysicsEngine()->applyForce(player.get(),
                                                    Point3(0, 12.5, 0));
        }

        /** Update the camera postion and set this to look at the ball */
        // TODO: remove z-value after demos
        /*Point3 camPos = motionFrame.vectorToWorldSpace(Point3(-10, 4, -15));
        camera->setPosition(camPos + motionFrame.translation);
        camera->lookAt(motionFrame.translation);*/
    }
}

void BallApp::onPose(Array<shared_ptr<Surface>> &posed3D,
                     Array<shared_ptr<Surface2D>> &posed2D) {
    m_widgetManager->onPose(posed3D, posed2D);

    scene()->onPose(posed3D);

    screenPrintf("WASD to move");
    screenPrintf("Space to jump");
}
} // namespace G3D
