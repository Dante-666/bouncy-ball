#include "bball.h"

int main(int argc, const char *argv[]) {
    initGLG3D();

    GApp::Settings settings(argc, argv);

    settings.window.caption = "BouncyBall-G3D";
    settings.window.width = 800;
    settings.window.height = 600;
    settings.dataDir = "../data-files";

    return BallApp(settings).run();
}

BallApp::BallApp(const GApp::Settings &settings) : GApp(settings) {}

void BallApp::onInit() {
    GApp::onInit();

    m_gbufferSpecification.encoding[GBuffer::Field::DEPTH_AND_STENCIL] =
        ImageFormat::DEPTH32();
    m_gbufferSpecification.depthEncoding = DepthEncoding::HYPERBOLIC;

    m_gbufferSpecification.encoding[GBuffer::Field::WS_NORMAL] =
        ImageFormat::RGB16F();

    // Called before the application loop begins.  Load data here and
    // not in the constructor so that common exceptions will be
    // automatically caught.
    showRenderingStats = false;

    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);

    m_scene = Scene::create(m_ambientOcclusion);
    // Allowing custom Entity subclasses to be parsed from .Scene.Any files
    //m_scene->registerEntitySubclass("PlayerEntity", &PlayerEntity::create);
    setScene(m_scene);

    makeGUI();

    developerWindow->cameraControlWindow->moveTo(
        Point2(developerWindow->cameraControlWindow->rect().x0(), 0));
    loadScene("Level");
    setActiveCamera(m_scene->typedEntity<Camera>("camera"));
    developerWindow->sceneEditorWindow->setPreventEntitySelect(true);
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
        m_debugCam = !m_debugCam;
        const shared_ptr<Camera> &camera =
            m_debugCam ? m_scene->defaultCamera() : debugCamera();
        setActiveCamera(camera);
    }

    return false;
}

void BallApp::onUserInput(UserInput *ui) {
    GApp::onUserInput(ui);
    ui->setPureDeltaMouse(!m_debugCam);
    if (!m_debugCam) {
        // TODO: sid : Implement motion code here
        // take inspiration from samples
    }
}

void BallApp::onPose(Array<shared_ptr<Surface>> &posed3D,
                     Array<shared_ptr<Surface2D>> &posed2D) {
    m_widgetManager->onPose(posed3D, posed2D);

    scene()->onPose(posed3D);

    screenPrintf("WASD to move");
    screenPrintf("Space to jump");
    screenPrintf("Of course none of it is gonna work now!!!");
}
