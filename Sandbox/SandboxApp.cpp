#include "Edge3D.h"

//#include "Scenes/FlappyBird.h"
//#include "Scenes/Physics2D.h"
//#include "Scenes/ShapesScene.h"
//#include "Scenes/TerrainGenerator.h"
//#include "Scenes/DemoScene.h"
//#include "Scenes/DemoScene2.h"
//#include "Scenes/LightDemo.h"
#include "Scenes/MeshTest.h"
//#include "Scenes/ConvexPolygon.h"
//#include "Scenes/CubeDemo.h"
#include "Scenes/BlackHole.h"

class SandboxApp : public Edge3D::Application {
public:
	SandboxApp(): Edge3D::Application() {

	}

	~SandboxApp() override {

	}

	void run() override {
		Edge3D::Window window("Cube Demo",1920,1080,false,true);

		//Edge3D::SceneManager::push(new FlappyBird());
		//Edge3D::SceneManager::push(new Physics(window.getWindow()));
		//Edge3D::SceneManager::push(new TerrainGenerator(window.getWindow()));
		//Edge3D::SceneManager::push(new ShapesScene(window.getWindow()));
		//Edge3D::SceneManager::push(new LightDemo(window,"Light Demo"));
		//Edge3D::SceneManager::push(new DemoScene(window));
		Edge3D::SceneManager::push(new MeshTest(window, "Mesh Test"));
		//Edge3D::SceneManager::push(new ConvexPolygon(window,"Convex Polygon"));
		//Edge3D::SceneManager::push(new CubeDemo(window,"Cube Demo"));
		//Edge3D::SceneManager::push(new BlackHole(window ,"Black Hole"));
		while(window.isRunning()){
			window.update();
		
			Edge3D::SceneManager::update(window.getDelta());

			window.postEvents();
		}
		
		Edge3D::SceneManager::clearAll();
		
		window.shutdown();
	}
};

Edge3D::Application* Edge3D::createApplication() {
	return new SandboxApp();
}