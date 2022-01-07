#pragma once

#include "Scene.h"

#include <vector>
#include <memory>

namespace Edge3D {
	

	class SceneManager {
	private:
		static std::vector<Scene*> scenes;

	public:
		SceneManager();
		~SceneManager();

		static void push(Scene* scene);

		static void pop();

		static void update(float delta);

		static void top(Scene* scene);

		static void clearAll();
	};
}

