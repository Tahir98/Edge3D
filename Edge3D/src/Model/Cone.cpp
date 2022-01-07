#pragma once

#include "Cone.h"
#include <iostream>
#include "imgui.h"
#include <string>

namespace Edge3D {
    static int objectCount = 0;

    Cone::Cone() {
        init();
    }

    Cone::Cone(const Vec3 pos, const float radius, const float height) {
        this->pos = pos;

        if (radius > 0)
            this->radius = rTemp = radius;
        if (height >= 0)
            this->height = hTemp = height;

        init();
    }

    Cone::Cone(const Vec3 pos, const Vec3 scale, const Vec3 rotation) {
        this->pos = pos;
        this->scale = scale;
        this->rotation = rotation;
        init();
    }

    Cone::Cone(const float radius, const float height, const unsigned int segment) {
        if (radius > 0)
            this->radius = rTemp = radius;
        if (height >= 0)
            this->height = hTemp = height;
        if (segment >= 3 && segment <= 50)
            this->segment = sTemp = segment;

        init();
    }

    Cone::~Cone() {
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

    void Cone::init() {
        m_objectCount = objectCount++;

        material.ambient = { 0.3f,0.3f,0.3f };
        material.diffuse = { 0.7f,0.7f,0.7f };
        material.specular = { 0.9f,0.9f,0.9f };
        material.shininess = 150;

        shader = new Shader("Shaders/shape.shader");

        vb = new VertexBuffer(nullptr, sizeof(Vec3) * 602, GL_STATIC_DRAW);

        ib = new IndexBuffer(nullptr, sizeof(unsigned int) * 600, GL_STATIC_DRAW);
        ibl = new IndexBuffer(nullptr, sizeof(unsigned int) * 600, GL_STATIC_DRAW);

        va.bind();
        vb->bind();

        va.addVertexAttribute(3, GL_FLOAT, false);
        va.addVertexAttribute(3, GL_FLOAT, false);

        setSegment(segment);
    }

    void Cone::draw(Camera& camera) {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

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

    void Cone::draw(Camera& camera, Light light) {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

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

    void Cone::imGuiDraw() {
        ImGui::Begin(("Cone" + std::to_string(m_objectCount)).c_str());
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


    void Cone::setSegment(const unsigned int segment) {

        if (segment >= 3 && segment <= 50) {
            this->segment = segment;
            if (sTemp != segment)
                sTemp = segment;
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


        for (unsigned int i = 0; i < segment; i++) {
            float radian = (i + 0.5f) * 2.0f * PI / (float)segment;
            Vec3 pos;

            float length = sqrtf(radius * radius + height * height);
            float coneX = radius / length;
            float coneY = -height / length;

            pos.y = -height / 2.0f;
            pos.x = cosf(radian);
            pos.z = sinf(radian);

            vertices.push_back({ 0, height / 2.0f,0 });
            vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });

            radian = i * 2.0f * PI / (float)segment;

            pos.y = -height / 2.0f;
            pos.x = cosf(radian);
            pos.z = sinf(radian);

            vertices.push_back({ pos.x * radius, pos.y, pos.z * radius });
            vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });
        }

        vertices.push_back({ 0, -height / 2.0f,0 });
        vertices.push_back({ 0,-1,0 });

        for (unsigned int i = 0; i < segment; i++) {
            float radian = i * 2.0f * PI / (float)segment;
            Vec3 pos;

            pos.y = -height / 2.0f;
            pos.x = cosf(radian) * radius;
            pos.z = sinf(radian) * radius;

            vertices.push_back(pos);
            vertices.push_back({ 0,-1,0 });
        }

        //Triangle indices
        for (unsigned int i = 0; i < segment * 2; i += 2) {
            if (i < 2 * segment - 2) {
                indices.push_back(i);
                indices.push_back(i + 3);
                indices.push_back(i + 1);
            }
            else {
                indices.push_back(i);
                indices.push_back(1);
                indices.push_back(i + 1);
            }
        }

        for (unsigned int i = 2 * segment + 1; i < 3 * segment + 1; i++) {
            if (i < 3 * segment) {
                indices.push_back(2 * segment);
                indices.push_back(i);
                indices.push_back(i + 1);
            }
            else {
                indices.push_back(2 * segment);
                indices.push_back(i);
                indices.push_back(2 * segment + 1);
            }
        }

        //Line indices
        for (unsigned int i = 0; i < segment * 2; i += 2) {
            if (i < 2 * segment - 2) {
                lineIndices.push_back(i);
                lineIndices.push_back(i + 3);
                lineIndices.push_back(i + 3);
                lineIndices.push_back(i + 1);
            }
            else {
                lineIndices.push_back(i);
                lineIndices.push_back(1);
                lineIndices.push_back(1);
                lineIndices.push_back(i + 1);
            }
        }

        for (unsigned int i = 2 * segment + 1; i < 3 * segment + 1; i++) {
            if (i < 3 * segment) {
                lineIndices.push_back(2 * segment);
                lineIndices.push_back(i);
                //lineIndices.push_back(i + 1);
            }
            else {
                lineIndices.push_back(2 * segment);
                lineIndices.push_back(i);
                //lineIndices.push_back(2 * segment + 1);
            }
        }

        //std::cout << "Vertices size: " << vertices.size() << std::endl;
        //std::cout << "Indices size: " << indices.size() << std::endl;
        //std::cout << "LineIndices size: " << lineIndices.size() << std::endl;

        vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        ib->subData(0, indices.size() * sizeof(unsigned int), indices.data());
        ibl->subData(0, lineIndices.size() * sizeof(unsigned int), lineIndices.data());

    }

    unsigned int Cone::getSegment() {
        return segment;
    }

    void Cone::setRadius(const float radius) {
        if (radius == this->radius)
            return;

        if (rTemp != radius)
            rTemp = radius;

        if (radius > 0) {
            vertices.clear();
            vertices.resize(0);
            vertices.shrink_to_fit();

            for (unsigned int i = 0; i < segment; i++) {
                float radian = (i + 0.5f) * 2.0f * PI / (float)segment;
                Vec3 pos;

                float length = sqrtf(radius * radius + height * height);
                float coneX = radius / length;
                float coneY = -height / length;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ 0, height / 2.0f,0 });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });

                radian = i * 2.0f * PI / (float)segment;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ pos.x * radius, pos.y, pos.z * radius });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });
            }

            vertices.push_back({ 0, -height / 2.0f,0 });
            vertices.push_back({ 0,-1,0 });

            for (unsigned int i = 0; i < segment; i++) {
                float radian = i * 2.0f * PI / (float)segment;
                Vec3 pos;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian) * radius;
                pos.z = sinf(radian) * radius;

                vertices.push_back(pos);
                vertices.push_back({ 0,-1,0 });
            }

            this->radius = radius;
            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        }
    }

    float Cone::getRadius() {
        return radius;
    }

    void Cone::setHeight(const float height) {
        if (height == this->height)
            return;

        if (hTemp != height)
            hTemp = height;

        if (height >= 0) {
            vertices.clear();
            vertices.resize(0);
            vertices.shrink_to_fit();

            for (unsigned int i = 0; i < segment; i++) {
                float radian = (i + 0.5f) * 2.0f * PI / (float)segment;
                Vec3 pos;

                float length = sqrtf(radius * radius + height * height);
                float coneX = radius / length;
                float coneY = -height / length;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ 0, height / 2.0f,0 });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });

                radian = i * 2.0f * PI / (float)segment;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ pos.x * radius, pos.y, pos.z * radius });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });
            }

            vertices.push_back({ 0, -height / 2.0f,0 });
            vertices.push_back({ 0,-1,0 });

            for (unsigned int i = 0; i < segment; i++) {
                float radian = i * 2.0f * PI / (float)segment;
                Vec3 pos;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian) * radius;
                pos.z = sinf(radian) * radius;

                vertices.push_back(pos);
                vertices.push_back({ 0,-1,0 });
            }

            this->height = height;
            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        }
    }

    float Cone::getHeight() {
        return height;
    }

    void Cone::setSize(const float radius, const float height) {
        if (radius == this->radius && height == this->height)
            return;

        if (rTemp != radius)
            rTemp = radius;
        if (hTemp != height)
            hTemp = height;

        if (radius > 0 || height >= 0) {
            vertices.clear();
            vertices.resize(0);
            vertices.shrink_to_fit();

            for (unsigned int i = 0; i < segment; i++) {
                float radian = (i + 0.5f) * 2.0f * PI / (float)segment;
                Vec3 pos;

                float length = sqrtf(radius * radius + height * height);
                float coneX = radius / length;
                float coneY = -height / length;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ 0, height / 2.0f,0 });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });

                radian = i * 2.0f * PI / (float)segment;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian);
                pos.z = sinf(radian);

                vertices.push_back({ pos.x * radius, pos.y, pos.z * radius });
                vertices.push_back({ -coneY * pos.x,coneX,-coneY * pos.z });
            }

            vertices.push_back({ 0, -height / 2.0f,0 });
            vertices.push_back({ 0,-1,0 });

            for (unsigned int i = 0; i < segment; i++) {
                float radian = i * 2.0f * PI / (float)segment;
                Vec3 pos;

                pos.y = -height / 2.0f;
                pos.x = cosf(radian) * radius;
                pos.z = sinf(radian) * radius;

                vertices.push_back(pos);
                vertices.push_back({ 0,-1,0 });
            }

            this->radius = radius;
            this->height = height;

            vb->subData(0, vertices.size() * sizeof(Vec3), (float*)vertices.data());
        }
    }

    void Cone::setMaterial(Material material) {
        this->material = material;
    }


}

