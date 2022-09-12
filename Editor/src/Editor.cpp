#include "Edge3D.h"
#include "Scenes/Menu.h"
#include "Scenes/TerrainAndPhysics.h"
#include "Scenes/PBRScene.h"
#include "Scenes/PhysicsEngine.h"
#include "Scenes/TerrainGeneration.h"
#include "Scenes/PhysicsSimulation2D.h"

class Editor : public Edge3D::Application {
public:
	Editor() : Edge3D::Application() {

	}

	~Editor() override {

	}

	void run() override {
		Edge3D::Window window("Multi Layer Perlin Noise", 1920, 1080, true ,true, true);

		//Edge3D::SceneManager::push(new T_P(window,"Terrain And Physics"));
		//Edge3D::SceneManager::push(new PBRScene(window, "PBR"));
		//Edge3D::SceneManager::push(new PhysicsEngine(window, "Physic Engine"));
		//Edge3D::SceneManager::push(new TerrainGeneration(window, "Terrain Generation"));
		Edge3D::SceneManager::push(new PhysicsSimulation2D(window, "Physics Simulation"));

		glfwSwapInterval(1);

		while (window.isRunning()) {
			window.update();
			Menu::begin(window);

			Edge3D::SceneManager::update(window.getDelta());

			Menu::end();
			window.postEvents();
		}

		Edge3D::SceneManager::clearAll();

		window.shutdown();
	}
};

Edge3D::Application* Edge3D::createApplication() {
	return new Editor();
}