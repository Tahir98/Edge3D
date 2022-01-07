#pragma once

#include <vector>
#include "Math/Math.h"

namespace Edge3D {
	class Mesh {
	protected:
		std::vector<Vec3> vertices;
		std::vector<unsigned int> indices;
		int vSize, iSize;
	public:
		void clear() {
			vertices.clear();
			indices.clear();

			vertices.shrink_to_fit();
			indices.shrink_to_fit();

			vSize = 0;
			iSize = 0;
		}

		Vec3* getVertices() {
			return vertices.data();
		}

		unsigned int* getIndices() {
			return indices.data();
		}

		virtual void calculate() = 0;

		unsigned int getVerticesSize() {
			return vSize;
		}

		unsigned int getIndicesSize() {
			return iSize;
		}
	};
}

