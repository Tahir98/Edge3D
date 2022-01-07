#include "Math.h"

#include <iostream>

namespace Math {
    
    Vec2 normalize(Vec2 a) {
        float length = a.length();
        return Vec2(a.x / length, a.y / length);
    }

    Vec3 normalize(Vec3 a) {
        float length = a.length();
        return Vec3(a.x / length, a.y / length, a.z / length);
    }

    Vec4 normalize(Vec4 a) {
        float length = a.length();
        return Vec4(a.x / length, a.y / length, a.z / length, a.w / length);
    }

    Vec3 crossProduct(const Vec3& a, const Vec3& b) {
        Vec3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    }

    float dotProduct(const Vec2& a, const Vec2& b) {
        return a.x * b.x + a.y + b.y;
    }

    float dotProduct(const Vec3& a, const Vec3& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    float dotProduct(const Vec4& a, const Vec4& b){
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    Matrix4x4 scale(const float x, const float y, const float z){
        return {
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        };
    }

    Matrix4x4 scale(const Vec3 a) {
        return {
            a.x, 0, 0, 0,
            0, a.y, 0, 0,
            0, 0, a.z, 0,
            0, 0, 0, 1
        };
    }

    Matrix4x4 translate(const float x, const float y, const float z) {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        };
    }
    Matrix4x4 translate(const Vec3 a) {
        return {
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            a.x, a.y, a.z, 1
        };
    }

    Matrix4x4 rotateX(const float radian) {
        return {
           1, 0,             0,            0,
           0, cosf(radian),  sinf(radian), 0,
           0, -sinf(radian), cosf(radian), 0,
           0, 0,             0,            1 
        };
    }

    Matrix4x4 rotateY(const float radian) {
        return {
            cosf(radian), 0, -sinf(radian), 0,
            0,            1, 0,             0,
            sinf(radian), 0, cosf(radian),  0,
            0,            0, 0,             1
        };
    }

    Matrix4x4 rotateZ(const float radian) {
        return {
            cosf(radian),  sinf(radian), 0, 0,
            -sinf(radian), cosf(radian), 0, 0,
            0,             0,            1, 0,
            0,             0,            0, 1
        };
    }

    Matrix4x4 rotate(const float x, const float y, const float z){
        return multiply(multiply(rotateZ(z),false,rotateY(y),true),false,rotateX(x),true);
    }

    Matrix4x4 rotate(const Vec3 rot){
        return rotateX(rot.x) * rotateY(rot.y) * rotateZ(rot.z);
    }

    Matrix4x4 perspective(const float aspectRatio, const float fov, const float near, const float far) {
        float fovRad = 1.0f / tanf(toRadian(fov / 2.0f));

        return {
            aspectRatio * fovRad, 0, 0, 0,
            0, fovRad, 0, 0,
            0, 0, -(far) / (far - near), -1.0f,
            0, 0, -(far * near) / (far - near), 0
        };
    }


    Matrix4x4 ortho(const float left, const float right, const float bottom, const float top, const float near, const float far){
        Matrix4x4 projection;

	    projection.m[0][0] = 2.0f / (right - left);
	    projection.m[0][3] = -(right + left) / (right - left);

	    projection.m[1][1] = 2.0f / (top - bottom);
	    projection.m[1][3] = -(top + bottom) / (top - bottom);
    
	    projection.m[2][2] = -2.0f / (far - near);
	    projection.m[2][3] = -(far + near) / (far - near);

	    projection.m[3][3] = 1.0f;

	    return projection;
    }

    Matrix4x4 lookat(Vec3 eye, Vec3 at, Vec3 up) {
        Vec3 cameraDirection = normalize(eye - at);
        Vec3 cameraRight = normalize(crossProduct(up, cameraDirection));
        Vec3 cameraUp = normalize(crossProduct(cameraDirection, cameraRight));

        Matrix4x4 direction = {
            cameraRight.x,cameraRight.y,cameraRight.z,0,
            cameraUp.x,cameraUp.y,cameraUp.z,0,
            cameraDirection.x,cameraDirection.y,cameraDirection.z,0,
            0,0,0,1
        };
        Matrix4x4 pos = {
            1,0,0, -eye.x,
            0,1,0, -eye.y,
            0,0,1, -eye.z,
            0,0,0,1
        };
        
        Matrix4x4 result = direction * pos;
        /*for (unsigned int i = 0; i < 4; i++) {
            for (unsigned int j = 0; j < 4; j++) {
                std::cout << result.m[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;*/

        return result;
    }

    Matrix2x2 multiply(const Matrix2x2& a, bool transposeA, const Matrix2x2& b, bool transposeB){
        Matrix2x2 result;
        for (unsigned int k = 0; k < 2; k++)
            for (unsigned int i = 0; i < 2; i++)
                for (unsigned int j = 0; j < 2; j++)
                    if(!transposeA && !transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[j][i];
                    else if(!transposeA && transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[i][j];
                    else if (transposeA && !transposeB)
                        result.m[i][k] += a.m[j][k] * b.m[j][i];
                    else 
                        result.m[i][k] += a.m[j][k] * b.m[i][j];

        return result;
    }

    Matrix3x3 multiply(const Matrix3x3& a, bool transposeA, const Matrix3x3& b, bool transposeB){
        Matrix3x3 result;
        for (unsigned int k = 0; k < 3; k++)
            for (unsigned int i = 0; i < 3; i++)
                for (unsigned int j = 0; j < 3; j++)
                    if (!transposeA && !transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[j][i];
                    else if (!transposeA && transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[i][j];
                    else if (transposeA && !transposeB)
                        result.m[i][k] += a.m[j][k] * b.m[j][i];
                    else
                        result.m[i][k] += a.m[j][k] * b.m[i][j];

        return result;
    }

    Matrix4x4 multiply(const Matrix4x4& a, bool transposeA, const Matrix4x4& b, bool transposeB){
        Matrix4x4 result;
        for (unsigned int k = 0; k < 4; k++)
            for (unsigned int i = 0; i < 4; i++)
                for (unsigned int j = 0; j < 4; j++)
                    if (!transposeA && !transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[j][i];
                    else if (!transposeA && transposeB)
                        result.m[i][k] += a.m[k][j] * b.m[i][j];
                    else if (transposeA && !transposeB)
                        result.m[i][k] += a.m[j][k] * b.m[j][i];
                    else
                        result.m[i][k] += a.m[j][k] * b.m[i][j];

        return result;
    }

     float toRadian(const float degree){
         return degree / 180.0f * PI;
     }

}