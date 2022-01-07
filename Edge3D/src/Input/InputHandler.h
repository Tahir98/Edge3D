#pragma once

#include "Math/Math.h"

namespace Edge3D {
	class InputHandler {
		friend class KeyPressedEvent;
		friend class KeyReleasedEvent;
		friend class KeyRepeatEvent;
		friend class MouseButtonPressedEvent;
		friend class MouseButtonReleasedEvent;
		friend class MouseMovedEvent;

	private:
		static std::vector<std::pair<int, bool>> keyboard;
		static std::vector<std::pair<int, bool>> mouse;

		static Vec2  mousPos;

		
	public:
		static void init();

		InputHandler();
		~InputHandler();

		bool getKeyState(int key);
		bool getMouseButtonState(int button);

		Vec2 getMousePos();
	};
}

