#pragma once
#include "Camera.h"
#include <iostream>

namespace Edge3D {
	class PerspectiveCamera : public Camera {
	private:
		float fov = 70;
		float aspectRatio;
		float n;
		float f;

	public:
		PerspectiveCamera(const float fov, float aspectRatio, float n, float f) {
			this->fov = fov;
			this->aspectRatio = aspectRatio;
			this->n = n;
			this->f = f;

			projection = Math::perspective(aspectRatio, fov, n, f);
			view = Math::lookat(position, position + direction, { 0,1,0 });
		}

		void update(float delta) override {
			Camera::update(delta);
			view = Math::lookat(position, position + direction, { 0,1,0 });
		}

		void setAspectRatio(float ratio) {
			if (ratio <= 0) {
				std::cout << "Given value is not acceptable" << std::endl;
				return;
			}

			aspectRatio = ratio;
			projection = Math::perspective(aspectRatio, fov, n, f);
		}

		void setFieldOfView(float degree) {
			if (degree < 5 || degree > 150) {
				std::cout << "Given value is not acceptable" << std::endl;
				return;
			}

			fov = degree;
			projection = Math::perspective(aspectRatio, fov, n, f);
		}

		void setFrustum(const float n, const float f) {
			if (n < 0.0f || f - n <= 0.0f || f <= 0.0) {
				std::cout << "Values are not in range" << std::endl;
				return;
			}

			this->n = n;
			this->f = f;

			projection = Math::perspective(aspectRatio, fov, n, f);
		}

	};
}



