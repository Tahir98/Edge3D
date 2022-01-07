#pragma once

#include <string>

namespace Edge3D {
	class Scene {
		friend class SceneManager;
	protected:
		static unsigned count;
		unsigned int id;
		std::string name;
	public:
		Scene(std::string name) : name(name) {
		
		}

		virtual ~Scene();

		virtual void onCreate() = 0;
		virtual void onUpdate(float delta) = 0;
		virtual void onSuspend() = 0;
		virtual void onResume() = 0;
		virtual void onDestroy() = 0;
	};
}

