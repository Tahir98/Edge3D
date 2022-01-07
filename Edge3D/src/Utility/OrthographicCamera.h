#pragma once
#include "Camera.h"
#include <iostream>

namespace Edge3D {
	class OrthographicCamera : public Camera {
	private:
		float left, right;
		float bottom, top;
		float n, f;

	public:
		OrthographicCamera(const float left, const float right, const float bottom, const float top, const float n, const float f) {
			this->left = left;
			this->right = right;
			this->bottom = bottom;
			this->top = top;
			this->n = n;
			this->f = f;

			projection = Math::ortho(left, right, bottom, top, n, f);
		}

		void update(float delta) override {
			Camera::update(delta);
			view = Math::lookat(position, position + direction, { 0,1,0 });
		}

	};
}



