#pragma once

#include <G3D/G3D.h>

class BallApp : public GApp {
    shared_ptr<Scene>	m_scene;
    bool		m_debugCam;
  protected:
    /** Called from onInit */
    void makeGUI();

  public:
    BallApp(const GApp::Settings &settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onPose(Array<shared_ptr<Surface>> &posed3D,
                        Array<shared_ptr<Surface2D>> &posed2D) override;

    virtual bool onEvent(const GEvent &e) override;
    virtual void onUserInput(UserInput *ui) override;
};
