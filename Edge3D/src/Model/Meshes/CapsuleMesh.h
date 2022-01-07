#pragma once
#include "Mesh.h"
#include "Log.h"

namespace Edge3D {
    class CapsuleMesh : public Mesh {
    private:
        unsigned int segment = 20;
        float radius = 0.5f, height = 1.0f;
    public:
        void calculate() {
            clear();

            float degZ, degY;
            Vec3 pos;

            /*       Vertices      */
            for (unsigned int k = 0; k < 2; k++) {
                for (unsigned int i = 0; i < segment; i++) {
                    degY = 2.0f * PI / (float)segment * i;

                    for (unsigned int j = 0; j < segment; j++) {
                        degZ = PI / (2.0f * (segment - 1)) * (float)j;

                        pos.y = (k == 0 ? height / 2.0f + sinf(degZ) * radius : -height / 2.0f - sinf(degZ) * radius);
                        pos.x = cosf(degZ) * cosf(degY) * radius;
                        pos.z = cosf(degZ) * sinf(degY) * radius;

                        vertices.push_back(pos);
                        vertices.push_back({ pos.x,pos.y - (k == 0 ? height / 2.0f : -height / 2.0f),pos.z });
                    }
                }
            }

            for (unsigned int i = 0; i < segment; i++) {
                float radian = 2.0f * PI / (float)segment * i;

                pos.x = cosf(radian) * radius;
                pos.z = sinf(radian) * radius;
                pos.y = height / 2.0f;

                vertices.push_back(pos);
                vertices.push_back({ pos.x,0,pos.z });

                pos.y = -height / 2.0f;
                vertices.push_back(pos);
                vertices.push_back({ pos.x,0,pos.z });
            }

            /*     Indices      */
            for (unsigned int i = 0; i < segment; i++) {
                for (unsigned int j = 0; j < segment - 1; j++) {
                    if (i < segment - 1) {
                        indices.push_back(i * segment + j);
                        indices.push_back(i * segment + j + 1);
                        indices.push_back((i + 1) * segment + j + 1);

                        indices.push_back(i * segment + j);
                        indices.push_back((i + 1) * segment + j + 1);
                        indices.push_back((i + 1) * segment + j);
                    }
                    else {
                        indices.push_back(i * segment + j);
                        indices.push_back(i * segment + j + 1);
                        indices.push_back(j + 1);

                        indices.push_back(i * segment + j);
                        indices.push_back(j + 1);
                        indices.push_back(j);
                    }
                }
            }

            unsigned int temp = segment * segment;
            for (unsigned int i = 0; i < segment; i++) {
                for (unsigned int j = 0; j < segment - 1; j++) {
                    if (i < segment - 1) {
                        indices.push_back(temp + i * segment + j);
                        indices.push_back(temp + (i + 1) * segment + j + 1);
                        indices.push_back(temp + i * segment + j + 1);

                        indices.push_back(temp + i * segment + j);
                        indices.push_back(temp + (i + 1) * segment + j);
                        indices.push_back(temp + (i + 1) * segment + j + 1);
                    }
                    else {
                        indices.push_back(temp + i * segment + j);
                        indices.push_back(temp + j + 1);
                        indices.push_back(temp + i * segment + j + 1);

                        indices.push_back(temp + i * segment + j);
                        indices.push_back(temp + j);
                        indices.push_back(temp + j + 1);
                    }
                }
            }

            temp *= 2;
            for (unsigned int i = 0; i < segment; i++) {
                if (i < segment - 1) {
                    indices.push_back(temp + 2 * i);
                    indices.push_back(temp + 2 * i + 3);
                    indices.push_back(temp + 2 * i + 1);

                    indices.push_back(temp + 2 * i);
                    indices.push_back(temp + 2 * i + 2);
                    indices.push_back(temp + 2 * i + 3);
                }
                else {
                    indices.push_back(temp + 2 * i);
                    indices.push_back(temp + 1);
                    indices.push_back(temp + 2 * i + 1);

                    indices.push_back(temp + 2 * i);
                    indices.push_back(temp + 0);
                    indices.push_back(temp + 1);
                }
            }

            vSize = sizeof(Vec3) * vertices.size();
            iSize = sizeof(unsigned int) * indices.size();
        }

        void setSegment(const unsigned int segment) {
            if (segment < 3)
                ENG_LOG_ERROR("Capsule Mesh: segment must be at least 3");
            else
                this->segment = segment;
        }

        unsigned int getSegment() { return segment; }

        void setHeight(const float height) {
            if (height < 0.0f)
                ENG_LOG_ERROR("Capsule Mesh: height cannot be negative");
            else
                this->height = height;
        }

        float getHeight() { return height; }

        void setRadius(const float radius) {
            if (radius <= 0.0f)
                ENG_LOG_ERROR("Capsule Mesh: radius must be bigger than 0");
            else
                this->radius = radius;
        }

        float getRadius() { return radius; }
    };
}

