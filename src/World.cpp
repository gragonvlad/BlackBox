#include <BlackBox/World.hpp>

float World::gravity = 1;

World::World()
{

}

void World::draw(float dt) {
  activeScene->draw(dt);
  // Camera ...
}

void World::setCamera(CCamera *camera)
{
  activeScene->setCamera(camera);
}

void World::setScene(Scene *scene) {
  activeScene = scene;
}

void World::update(float deltatime)
{
  activeScene->update(deltatime);
}
