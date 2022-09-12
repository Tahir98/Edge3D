#include "SceneManager.h"
#include "Log.h"

namespace Edge3D {
	std::vector<Scene*> SceneManager::scenes;

	SceneManager::SceneManager() {}
	SceneManager::~SceneManager() {
		for (unsigned int i = 0; i < scenes.size(); ) {
			scenes[scenes.size() - 1]->onDestroy();
			delete scenes[scenes.size() - 1];
			scenes.pop_back();
		}
	}

	void SceneManager::push(Scene* scene) {
		if(scenes.size() > 0)
			scenes[scenes.size() - 1]->onSuspend();

		if (scenes.size() == 0)
			Scene::count = 0;

		scenes.push_back(scene);
		ENG_LOG_INFO("{0} scene created", scene->name);
		scene->id = Scene::count++;
		scene->onCreate();
	}

	void SceneManager::pop() {
		scenes[scenes.size() - 1]->onDestroy();
		delete scenes[scenes.size() - 1];
		scenes.pop_back();

		scenes[scenes.size() - 1]->onResume();
	}

	void SceneManager::update(float delta) {
		if(scenes.size() > 0)
			scenes[scenes.size() - 1]->onUpdate(delta);
	}

	void SceneManager::top(Scene* scene) {
		int index = -1;
		for (unsigned int i = 0; i < scenes.size() - 2; i++) {
			if (scene->id == scenes[i]->id) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			scenes[scenes.size() - 1]->onSuspend();

			scenes.erase(scenes.begin() + index);
			scenes.push_back(scene);
			scenes[scenes.size() - 1]->onResume();
		}
	}

	void SceneManager::clearAll() {
		for(unsigned int i = 0; i < scenes.size(); i++) {
			ENG_LOG_INFO("{0} scene deleted",scenes[i]->name);
			scenes[i]->onDestroy();
			delete scenes[i];
		}

		scenes.clear();
	}
}