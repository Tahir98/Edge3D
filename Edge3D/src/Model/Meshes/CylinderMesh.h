#pragma once
#include "Mesh.h"
#include "Log.h"

namespace Edge3D {
    class CylinderMesh : public Mesh {
    private:
        unsigned int segment = 20;
        float radius = 0.5f, height = 1.0f;
    public:
        void calculate() {
            clear();

            vertices.push_back({ 0,height / 2.0f,0 });
            vertices.push_back({ 0,-1,0 });
            vertices.push_back({ 0,-height / 2.0f,0 });
            vertices.push_back({ 0,1,0 });

            for (unsigned int j = 0; j < 3; j++) {
                for (unsigned int i = 0; i < segment; i++) {
                    float radian = i * 2.0f * PI / (float)segment;
                    Vec3 pos;

                    pos.x = cosf(radian) * radius;
                    pos.z = sinf(radian) * radius;

                    if (j == 0) {
                        pos.y = height / 2.0f;
                        vertices.push_back(pos);
                        vertices.push_back({ 0,-1,0 });
                    }
                    else if (j == 1) {
                        pos.y = -height / 2.0f;
                        vertices.push_back(pos);
                        vertices.push_back({ 0,1,0 });
                    }
                    else {
                        pos.y = height / 2.0f;
                        vertices.push_back(pos);
                        vertices.push_back({ -pos.x,0,-pos.z });

                        pos.y = -height / 2.0f;
                        vertices.push_back(pos);
                        vertices.push_back({ -pos.x,0,-pos.z });
                    }
                }
            }

            for (unsigned int j = 0; j < 3; j++) {
                for (unsigned int i = 0; i < segment; i++) {
                    if (j == 0) {
                        if (i < segment - 1) {
                            indices.push_back(0);
                            indices.push_back(2 + i + 1);
                            indices.push_back(2 + i);
                        }
                        else {
                            indices.push_back(0);
                            indices.push_back(2);
                            indices.push_back(2 + i);
                        }
                    }
                    else if (j == 1) {
                        if (i < segment - 1) {
                            indices.push_back(1);
                            indices.push_back(2 + segment + i);
                            indices.push_back(2 + segment + i + 1);
                        }
                        else {
                            indices.push_back(1);
                            indices.push_back(2 + segment + i);
                            indices.push_back(2 + segment);
                        }
                    }
                    else {
                        if (i < segment - 1) {
                            indices.push_back(2 + 2 * segment + 2 * i);
                            indices.push_back(2 + 2 * segment + 2 * i + 3);
                            indices.push_back(2 + 2 * segment + 2 * i + 1);

                            indices.push_back(2 + 2 * segment + 2 * i);
                            indices.push_back(2 + 2 * segment + 2 * i + 2);
                            indices.push_back(2 + 2 * segment + 2 * i + 3);
                        }
                        else {
                            indices.push_back(2 + 2 * segment + 2 * i);
                            indices.push_back(2 + 2 * segment + 1);
                            indices.push_back(2 + 2 * segment + 2 * i + 1);

                            indices.push_back(2 + 2 * segment + 2 * i);
                            indices.push_back(2 + 2 * segment + 0);
                            indices.push_back(2 + 2 * segment + 1);
                        }
                    }
                }
            }

            vSize = sizeof(Vec3) * vertices.size();
            iSize = sizeof(unsigned int) * indices.size();
        }

        void setSegment(const unsigned int segment) {
            if (segment < 3)
                ENG_LOG_ERROR("Cone Mesh: segment must be at least 3");
            else
                this->segment = segment;
        }

        unsigned int getSegment() { return segment; }

        void setHeight(const float height) {
            if (height <= 0.0f)
                ENG_LOG_ERROR("Cone Mesh: height must be bigger than 0");
            else
                this->height = height;
        }

        float getHeight() { return height; }

        void setRadius(const float radius) {
            if (radius <= 0.0f)
                ENG_LOG_ERROR("Cone Mesh: radius must be bigger than 0");
            else
                this->radius = radius;
        }

        float getRadius() { return radius; }
    };
}

