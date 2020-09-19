/** Copyright 2020 Blood Eagle Studio
 *
 * You may not use, not distribute and not modify this code 
 * under any manifestable possibility and if such a scenario
 * occurs, any changes to the code must be reviewed by the 
 * original author of this project.
 *
 *  Author : Siddharth J Singh(dante)
 */

#pragma once

#include <G3D-base/G3D-base.h>
#include <G3D-gfx/G3D-gfx.h>
#include <G3D-app/G3D-app.h>

//#include <G3D/G3D.h>
#include "PhysicsScene.h"
#include "RigidEntity.h"

namespace G3D {
class BallApp : public GApp {
    shared_ptr<Scene> m_scene;
    bool m_debugCam;

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
} // namespace G3D
