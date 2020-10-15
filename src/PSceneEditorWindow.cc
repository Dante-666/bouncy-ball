#include "PSceneEditorWindow.h"

/**
 Widget created by SceneEditorWindow that sits at the bottom of the event
 order so that any click that passes through the rest of the Widget queue
 can be interpreted as entity selection.
 */

EntitySelectWidget::EntitySelectWidget(PhysicsSceneEditorWindow *s)
    : m_sceneEditor(s){};

void EntitySelectWidget::render(RenderDevice *rd) const { m_mapper.update(rd); }

float EntitySelectWidget::positionalEventZ(const Point2 &pixel) const {
    if (m_sceneEditor->preventEntitySelect()) {
        return fnan();
    }

    const Rect2D &view = m_sceneEditor->getApp()->renderDevice->viewport();

    float distance;

    const Ray &ray = m_sceneEditor->getScene()->eyeRay(
        m_sceneEditor->getApp()->activeCamera(), pixel + Vector2::one() * 0.5f,
        view,
        m_sceneEditor->getApp()
            ->settings()
            .hdrFramebuffer.depthGuardBandThickness);

    m_sceneEditor->getScene()->intersect(
        ray, distance, m_sceneEditor->getVisSettings().showMarkers);

    return distance / ray.direction().z;
    return 0;
}

bool EntitySelectWidget::onEvent(const GEvent &event) {
    if (m_sceneEditor->preventEntitySelect()) {
        return false;
    }

    // Keep the selector at the back of the z-order so that it doesn't steal
    // events from other objects
    manager()->moveWidgetToBack(
        dynamic_pointer_cast<Widget>(shared_from_this()));

    if ((event.type == GEventType::MOUSE_BUTTON_DOWN) &&
        (event.button.button == 0) && !event.button.controlKeyIsDown) {

        const GApp *app = m_sceneEditor->getApp();
        const shared_ptr<Scene> &scene = m_sceneEditor->getScene();
        const Rect2D &view = app->renderDevice->viewport();

        const Ray &ray = scene->eyeRay(
            app->activeCamera(), event.mousePosition() + Vector2(0.5f, 0.5f),
            view, app->settings().hdrFramebuffer.depthGuardBandThickness);

        // Left click: select by casting a ray through the center of the pixel
        float distance = finf();
        const shared_ptr<Entity> &newSelection = scene->intersect(
            ray, distance, m_sceneEditor->getVisSettings().showMarkers,
            Array<shared_ptr<Entity>>(), m_sceneEditor->getSelectionInfo());

        if (newSelection != m_sceneEditor->getSelectedEntity()) {
            auto rigid = dynamic_pointer_cast<RigidEntity>(newSelection);
            if (rigid) {
                if (m_lastSelectedRigidEntity) {
                    m_lastSelectedRigidEntity->isBeingEdited = false;
                }
                rigid->isBeingEdited = true;
		m_lastSelectedRigidEntity = rigid;
            } else if (m_lastSelectedRigidEntity) {
                m_lastSelectedRigidEntity->isBeingEdited = false;
		m_lastSelectedRigidEntity->updatePhysicsFrame();//m_manipulator->frame());
                m_lastSelectedRigidEntity = nullptr;
            }
            m_sceneEditor->selectEntity(newSelection);
            return true;
        }
    }
    return false;
}

PhysicsSceneEditorWindow::PhysicsSceneEditorWindow(GApp *app,
                                                   shared_ptr<Scene> scene,
                                                   shared_ptr<GuiTheme> theme)
    : SceneEditorWindow(app, scene, theme){};

shared_ptr<PhysicsSceneEditorWindow>
PhysicsSceneEditorWindow::create(GApp *app, shared_ptr<Scene> scene,
                                 shared_ptr<GuiTheme> theme) {

    shared_ptr<PhysicsSceneEditorWindow> ptr(
        new PhysicsSceneEditorWindow(app, scene, theme));
    shared_ptr<EntitySelectWidget> selector(new EntitySelectWidget(ptr.get()));
    app->addWidget(selector);
    app->addWidget(ptr);

    return ptr;
}

void PhysicsSceneEditorWindow::onSelectEntity() {
    m_perEntityPane->removeAllChildren();
    if (notNull(m_selectedEntity) && !m_preventEntitySelect) {
        // Populate the GUI
        m_selectedEntity->makeGUI(m_perEntityPane, m_app);

        m_entityList->setSelectedValue(m_selectedEntity->name());
        // Copy the current track or frame
        const shared_ptr<Entity::SplineTrack> &splineTrack =
            dynamic_pointer_cast<Entity::SplineTrack>(
                m_selectedEntity->track());
        if (notNull(splineTrack)) {
            // Track
            m_popupSplineEditor->setSpline(splineTrack->spline());
        } else {
            // Frame
            m_manipulator->setFrame(m_selectedEntity->frame());
        }
    }
}

GApp *PhysicsSceneEditorWindow::getApp() { return m_app; }

shared_ptr<Scene> PhysicsSceneEditorWindow::getScene() { return m_scene; }

SceneVisualizationSettings &PhysicsSceneEditorWindow::getVisSettings() {
    return m_visualizationSettings;
}

shared_ptr<Entity> PhysicsSceneEditorWindow::getSelectedEntity() {
    return m_selectedEntity;
}

Model::HitInfo &PhysicsSceneEditorWindow::getSelectionInfo() {
    return m_selectionInfo;
}
