#include "Edge3D.h"

//#include "Scenes/FlappyBird.h"
//#include "Scenes/Physics2D.h"
//#include "Scenes/ShapesScene.h"
//#include "Scenes/TerrainGenerator.h"
//#include "Scenes/DemoScene.h"
//#include "Scenes/DemoScene2.h"
//#include "Scenes/LightDemo.h"
//#include "Scenes/MeshTest.h"
//#include "Scenes/MeshTest2.h"
//#include "Scenes/ConvexPolygon.h"
//#include "Scenes/CubeDemo.h"
//#include "Scenes/BlackHole.h"
#include "Scenes/PhysicsDemoSAS.h"
//#include "Scenes/CollisionTest.h"
#include "Scenes/Menu.h"

class SandboxApp : public Edge3D::Application {
public:
	SandboxApp(): Edge3D::Application() {

	}

	~SandboxApp() override {

	}

	void run() override {
		Edge3D::Window window("Physics Demo",1920,1080,false,true,true);

		//Edge3D::SceneManager::push(new FlappyBird());
		//Edge3D::SceneManager::push(new Physics(window.getWindow()));
		//Edge3D::SceneManager::push(new TerrainGenerator(window,"Demo"));
		//Edge3D::SceneManager::push(new ShapesScene(window.getWindow()));
		//Edge3D::SceneManager::push(new LightDemo(window,"Light Demo"));
		//Edge3D::SceneManager::push(new DemoScene(window));
		//Edge3D::SceneManager::push(new MeshTest2(window, "Mesh Test2"));
		//Edge3D::SceneManager::push(new ConvexPolygon(window,"Convex Polygon"));
		//Edge3D::SceneManager::push(new CubeDemo(window,"Cube Demo"));
		//Edge3D::SceneManager::push(new BlackHole(window ,"Black Hole"));
		Edge3D::SceneManager::push(new PhysicsDemo(window, "Physics Demo"));
		//Edge3D::SceneManager::push(new CollisionTest(window, "Collision Test"));

		while(window.isRunning()){
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
	return new SandboxApp();
}