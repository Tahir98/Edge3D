#pragma once
#include "Math/Math.h"


namespace Edge3D {
	class Camera {
	protected:
		Matrix4x4 projection;
		Matrix4x4 view;

		Vec3 position{ 0,0,0 };
		Vec3 direction{ 0,0,-1 };

		float pitch = 0, yaw = -90, roll = 0;

	public:

		virtual void update(const float delta) {
			direction.x = cosf(pitch * PI / 180.0f) * cosf(yaw * PI / 180.0f);
			direction.y = sinf(pitch * PI / 180.0f);
			direction.z = cosf(pitch * PI / 180.0f) * sinf(yaw * PI / 180.0f);
			direction = Math::normalize(direction);
		}
		void setPosition(const Vec3 pos) {
			position = Vec3(pos);
		}

		Vec3 getPosition() {
			return position;
		}

		void setPosition(const float x, const float y, const float z) {
			position = { x,y,z };
		}

		void translate(const float x, const float y, const float z) {
			position.x += x;
			position.y += y;
			position.z += z;
		}

		void translate(const Vec3 trans) {
			position = position + trans;
		}

		void translate2(const float x, const float y, const float z) {
			position = position + Math::normalize(Math::crossProduct(direction, { 0,1,0 })) * x;

			Vec3 right = Math::normalize(Math::crossProduct(direction, { 0,1,0 }));
			Vec3 up = Math::normalize(Math::crossProduct(right, direction));
			position = position + up * y;

			position = position + direction * z;
		}

		void translate2(const Vec3 trans) {
			position = position - Math::normalize(Math::crossProduct(direction, { 0,1,0 })) * trans.x;

			Vec3 right = Math::normalize(Math::crossProduct(direction, { 0,1,0 }));
			Vec3 up = Math::normalize(Math::crossProduct(right, direction));
			position = position + up * trans.y;

			position = position + direction * trans.z;
		}


		float* getProjectionMatrix(){
			return projection.m[0];
		}

		float* getViewMatrix() {
			return view.m[0];
		}

		void setViewMatrix(const Matrix4x4 view) {
			this->view = view;
		}

		void rotX(const float degree) {
			pitch += degree;
			if (pitch > 80)
				pitch = 80;
			else if (pitch < -80)
				pitch = -80;
		}

		void rotY(const float degree) {
			yaw += degree;
		}

		void rotZ(const float degree) {
			roll += degree;
		}

		void rotate(const Vec3 rot) {
			pitch += rot.x;
			yaw += rot.y;
			roll += rot.z;

			if (pitch > 80)
				pitch = 80;
			else if (pitch < -80)
				pitch = -80;
		}

		void rotate(const float x, const float y, const float z) {
			pitch += x;
			yaw += y;
			roll += z;

			if (pitch > 80)
				pitch = 80;
			else if (pitch < -80)
				pitch = -80;
		}

		Vec3 getDirection() {
			return direction;
		}
	};
}

