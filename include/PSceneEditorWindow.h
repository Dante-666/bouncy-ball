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

#include "G3D/G3D.h"

class PhysicsSceneEditorWindow : public SceneEditorWindow {
protected:
    friend class EntitySelectWidget;
    PhysicsSceneEditorWindow(GApp *app, shared_ptr<Scene> scene,
                             shared_ptr<GuiTheme> theme);

public:
    //virtual void onSelectEntity() override;
    static shared_ptr<PhysicsSceneEditorWindow>
    create(GApp *app, shared_ptr<Scene> scene, shared_ptr<GuiTheme> theme);

    // Hack to make it work
    GApp *getApp();

    shared_ptr<Scene> getScene();

    SceneVisualizationSettings& getVisSettings();

    shared_ptr<Entity> getSelectedEntity();

    Model::HitInfo& getSelectionInfo();
};

namespace {
class EntitySelectWidget : public Widget {
protected:
    //shared_ptr<RigidEntity> m_lastSelectedRigidEntity;
    PhysicsSceneEditorWindow *m_sceneEditor;
    mutable EventCoordinateMapper m_mapper;

public:
    EntitySelectWidget(PhysicsSceneEditorWindow *s);

    virtual void render(RenderDevice *rd) const override;

    virtual float positionalEventZ(const Point2 &pixel) const override;

    bool onEvent(const GEvent &event) override;
};
} // namespace
