#pragma once
#include "Capsule.h"
#include <iostream>
#include "imgui.h"
#include <string>

namespace Edge3D {
    static int objectCount = 0;

    Capsule::Capsule() {
        init();
    }

    Capsule::Capsule(const Vec3 pos, const float radius, const float height) {
        this->pos = pos;

        if (radius > 0)
            this->radius = rTemp = radius;
        if (height >= 0)
            this->height = hTemp = height;

        init();
    }

    Capsule::Capsule(const Vec3 pos, const Vec3 scale, const Vec3 rotation) {
        this->pos = pos;
        this->scale = scale;
        this->rotation = rotation;
        init();
    }

    Capsule::Capsule(const float radius, const float height, const unsigned int segment) {
        if (radius > 0)
            this->radius = rTemp = radius;
        if (height >= 0)
            this->height = hTemp = height;
        if (segment >= 3 && segment <= 50)
            this->segment = sTemp = segment;

        init();
    }

    Capsule::~Capsule() {
        delete vb;
        delete ib;
        delete ibl;
        delete shader;

        vertices.clear();
        vertices.resize(0);
        vertices.shrink_to_fit();

        indices.clear();
        indices.resize(0);
        indices.shrink_to_fit();

        lineIndices.clear();
        lineIndices.resize(0);
        lineIndices.shrink_to_fit();

        objectCount--;
    }

    void Capsule::init() {
        m_objectCount = objectCount++;

        material.ambient = { 0.3f,0.3f,0.3f };
        material.diffuse = { 0.7f,0.7f,0.7f };
        material.specular = { 0.9f,0.9f,0.9f };
        material.shininess = 150;

        shader = new Shader("Shaders/shape.shader");

        vb = new VertexBuffer(nullptr, sizeof(Vec3) * 10200, GL_STATIC_DRAW);

        ib = new IndexBuffer(nullptr, sizeof(unsigned int) * 29700, GL_STATIC_DRAW);
        ibl = new IndexBuffer(nullptr, sizeof(unsigned int) * 29600, GL_STATIC_DRAW);

        va.bind();
        vb->bind();

        va.addVertexAttribute(vb, 3, GL_FLOAT, false);
        va.addVertexAttribute(vb, 3, GL_FLOAT, false);

        setSegment(segment);
    }

    void Capsule::draw(Camera& camera) {
        shader->bind();

        model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

        shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
        shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
        shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

        va.bind();

        if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
            ib->bind();
            shader->setUniform1f("chosen", 2);

            shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
            shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
            shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
            shader->setUniform1f("material.shininess", material.shininess);

            shader->setUniform3f("light.specular", 0, 0, 0);

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
            shader->setUniform1f("chosen", 0);
            glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
        }

        if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
            ibl->bind();
            shader->setUniform1f("chosen", 1);
            glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
        }

        if (debugMode) {
            imGuiDraw();
            if (sTemp != segment)
                setSegment(sTemp);
            if (rTemp != radius)
                setRadius(rTemp);
            if (hTemp != height)
                setHeight(hTemp);
        }
    }

    void Capsule::draw(Camera& camera, PointLight light) {
        shader->bind();

        model = Math::multiply(Math::multiply(Math::translate(pos), true, Math::scale(scale), false), true, Math::rotate(rotation), false);

        shader->setUniformMatrix4fv("model", 1, true, model.m[0]);
        shader->setUniformMatrix4fv("view", 1, true, camera.getViewMatrix());
        shader->setUniformMatrix4fv("projection", 1, true, camera.getProjectionMatrix());

        va.bind();

        if ((debugMode && triangle) || (!debugMode && mode == DrawMode::TRIANGLE)) {
            ib->bind();
            shader->setUniform1f("chosen", 2);

            shader->setUniform3f("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
            shader->setUniform3f("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
            shader->setUniform3f("material.specular", material.specular.x, material.specular.y, material.specular.z);
            shader->setUniform1f("material.shininess", material.shininess);

            shader->setUniform3f("light.pos", light.pos.x, light.pos.y, light.pos.z);
            shader->setUniform3f("light.ambient", light.ambient.x, light.ambient.y, light.ambient.z);
            shader->setUniform3f("light.diffuse", light.diffuse.x, light.diffuse.y, light.diffuse.z);
            shader->setUniform3f("light.specular", light.specular.x, light.specular.y, light.specular.z);
            shader->setUniform1f("light.constant", light.constant);
            shader->setUniform1f("light.linear", light.linear);
            shader->setUniform1f("light.quadrantic", light.quadrantic);

            Vec3 viewPos = camera.getPosition();
            shader->setUniform3f("viewPos", viewPos.x, viewPos.y, viewPos.z);

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        if ((debugMode && point) || (!debugMode && mode == DrawMode::POINT)) {
            shader->setUniform1f("chosen", 0);
            glDrawArrays(GL_POINTS, 0, vertices.size() / 2);
        }

        if ((debugMode && line) || (!debugMode && mode == DrawMode::LINE)) {
            ibl->bind();
            shader->setUniform1f("chosen", 1);
            glDrawElements(GL_LINES, lineIndices.size(), GL_UNSIGNED_INT, 0);
        }

        if (debugMode) {
            imGuiDraw();
            if (sTemp != segment)
                setSegment(sTemp);
            if (rTemp != radius)
                setRadius(rTemp);
            if (hTemp != height)
                setHeight(hTemp);
        }
    }


    void Capsule::imGuiDraw() {
        ImGui::Begin(("Capsule" + std::to_string(m_objectCount)).c_str());
        ImGui::SliderFloat3("Position", (float*)&pos, -20, 20);
        ImGui::SliderFloat3("Scale", (float*)&scale, 0, 10);
        ImGui::SliderFloat3("Rotation", (float*)&rotation, -PI, PI);
        ImGui::SliderFloat3("amibent", (float*)&material.ambient, 0, 1);
        ImGui::SliderFloat3("diffuse", (float*)&material.diffuse, 0, 1);
        ImGui::SliderFloat3("specular", (float*)&material.specular, 0, 1);
        ImGui::SliderFloat("shininess", &material.shininess, 1, 200);
        ImGui::SliderInt("Segment", (int*)&sTemp, 3, 50);
        ImGui::SliderFloat("Height", &hTemp, 0, 10);
        ImGui::SliderFloat("Radius", &rTemp, 0, 10);
        ImGui::Checkbox("Point", &point);
        ImGui::Checkbox("Line", &line);
        ImGui::Checkbox("Triangle", &triangle);
        ImGui::End();
    }

    void Capsule::setSegment(const unsigned int segment) {

        if (segment >= 3 && segment <= 50) {
            this->segment = segment;
            if (sTemp != segment) {
                sTemp = segment;
            }
        }
        else {
            return;
        }

        vertices.clear();
        vertices.resize(0);
        vertices.shrink_to_fit();

        indices.clear();
        indices.resize(0);
        indices.shrink_to_fit();

        lineIndices.clear();
        lineIndices.resize(0);
        lineIndices.shrink_to_fit();

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

        /*      Line Indices     */
        for (unsigned int i = 0; i < segment; i++) {
            for (unsigned int j = 0; j < segment - 1; j++) {
                if (i < segment - 1) {
                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back(i * segment + j + 1);

                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back((i + 1) * segment + j);

                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back((i + 1) * segment + j + 1);
                }
                else {
                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back(i * segment + j + 1);

                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back(j);

                    lineIndices.push_back(i * segment + j);
                    lineIndices.push_back(j + 1);
                }
            }
        }

        temp = segment * segment;
        for (unsigned int i = 0; i < segment; i++) {
            for (unsigned int j = 0; j < segment - 1; j++) {
                if (i < segment - 1) {
                    lineIndices.push_back(temp + i * segment + j);
                    lineIndices.push_back(temp + (i + 1) * segment + j + 1);

                    lineIndices.push_back(temp + (i + 1) * segment + j);
                    lineIndices.push_back(temp + i * segment + j);

                    lineIndices.push_back(temp + i * segment + j);
                    lineIndices.push_back(temp + i * segment + j + 1);
                }
                else {
                    lineIndices.push_back(temp + i * segment + j);
                    lineIndices.push_back(temp + j + 1);

                    lineIndices.push_back(temp + j);
                    lineIndices.push_back(temp + i * segment + j);

                    lineIndices.push_back(temp + i * segment + j);
                    lineIndices.push_back(temp + i * segment + j + 1);
                }
            }
        }

        temp *= 2;
        for (unsigned int i = 0; i < segment; i++) {
            if (i < segment - 1) {
                lineIndices.push_back(temp + 2 * i);
                lineIndices.push_back(temp + 2 * i + 3);
                lineIndices.push_back(temp + 2 * i + 1);
                lineIndices.push_back(temp + 2 * i);
            }
            else {
                lineIndices.push_back(temp + 2 * i);
                lineIndices.push_back(temp + 1);
                lineIndices.push_back(temp + 2 * i + 1);
                lineIndices.push_back(temp + 2 * i);
            }
        }

        //std::cout << "Vertices size: " << vertices.size() << std::endl;
        //std::cout << "Indices size: " << indices.size() << std::endl;
        //std::cout << "LineIndices size: " << lineIndices.size() << std::endl;

        vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        ib->subData(0, indices.size() * sizeof(unsigned int), indices.data());
        ibl->subData(0, lineIndices.size() * sizeof(unsigned int), lineIndices.data());
    }

    unsigned int Capsule::getSegment() {
        return segment;
    }

    void Capsule::setRadius(const float radius) {
        if (radius == this->radius)
            return;

        if (rTemp != radius) {
            rTemp = radius;
        }

        if (radius > 0) {
            for (unsigned int i = 0; i < 4 * segment * segment; i += 2) {
                vertices[i].x = (vertices[i].x / this->radius) * radius;
                if (i < 2 * segment * segment) {
                    vertices[i].y -= height / 2.0f;
                    vertices[i].y = (vertices[i].y / this->radius) * radius + height / 2.0f;
                }
                else {
                    vertices[i].y += height / 2.0f;
                    vertices[i].y = (vertices[i].y / this->radius) * radius - height / 2.0f;
                }

                vertices[i].z = vertices[i].z / this->radius * radius;
            }

            for (unsigned int i = 4 * segment * segment; i < vertices.size(); i += 2) {
                vertices[i].x = vertices[i].x / this->radius * radius;
                vertices[i].z = vertices[i].z / this->radius * radius;
            }

            this->radius = radius;
            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        }
    }

    float Capsule::getRadius() {
        return radius;
    }

    void Capsule::setHeight(const float height) {
        if (height == this->height)
            return;

        if (hTemp != height) {
            hTemp = height;
        }

        if (height >= 0) {
            for (unsigned int i = 0; i < 4 * segment * segment; i += 2) {
                if (i < 2 * segment * segment)
                    vertices[i].y = vertices[i].y - this->height / 2.0f + height / 2.0f;
                else
                    vertices[i].y = vertices[i].y + this->height / 2.0f - height / 2.0f;
            }

            for (unsigned int i = 4 * segment * segment; i < vertices.size(); i += 2) {
                if (i % 4 == 0)
                    vertices[i].y = vertices[i].y - this->height / 2.0f + height / 2.0f;
                else
                    vertices[i].y = vertices[i].y + this->height / 2.0f - height / 2.0f;
            }

            this->height = height;
            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        }
    }

    float Capsule::getHeight() {
        return height;
    }

    void Capsule::setSize(const float radius, const float height) {
        if (rTemp != radius) {
            rTemp = radius;
        }

        if (hTemp != height) {
            hTemp = height;
        }

        if (height >= 0) {
            for (unsigned int i = 0; i < 4 * segment * segment; i += 2) {
                if (i < 2 * segment * segment)
                    vertices[i].y = vertices[i].y - this->height / 2.0f + height / 2.0f;
                else
                    vertices[i].y = vertices[i].y + this->height / 2.0f - height / 2.0f;
            }

            for (unsigned int i = 4 * segment * segment; i < vertices.size(); i += 2) {
                if (i % 4 == 0)
                    vertices[i].y = vertices[i].y - this->height / 2.0f + height / 2.0f;
                else
                    vertices[i].y = vertices[i].y + this->height / 2.0f - height / 2.0f;
            }

            this->height = height;
        }

        if (radius > 0) {
            for (unsigned int i = 0; i < 4 * segment * segment; i += 2) {
                vertices[i].x = (vertices[i].x / this->radius) * radius;
                if (i < 2 * segment * segment) {
                    vertices[i].y -= height / 2.0f;
                    vertices[i].y = (vertices[i].y / this->radius) * radius + height / 2.0f;
                }
                else {
                    vertices[i].y += height / 2.0f;
                    vertices[i].y = (vertices[i].y / this->radius) * radius - height / 2.0f;
                }

                vertices[i].z = vertices[i].z / this->radius * radius;
            }

            for (unsigned int i = 4 * segment * segment; i < vertices.size(); i += 2) {
                vertices[i].x = vertices[i].x / this->radius * radius;
                vertices[i].z = vertices[i].z / this->radius * radius;
            }

            this->radius = radius;
        }

        if (radius >= 0 || height >= 0)
            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
    }

    void Capsule::setMaterial(Material material) {
        this->material = material;
    }
}



