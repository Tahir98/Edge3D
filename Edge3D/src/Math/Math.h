#pragma once

#include <cmath>

struct Vec2 {
    float x, y;
    Vec2() {
        x = 0; y = 0;
    }

    Vec2(const float x, const float y) {
        this->x = x;
        this->y = y;
    }

    Vec2 operator+(const Vec2& a) {
        return Vec2(x + a.x, y + a.y);
    }

    Vec2 operator-(const Vec2& a) {
        return Vec2(x - a.x, y - a.y);
    }

    friend Vec2 operator+(const float& a, const Vec2& b) {
        return Vec2(a + b.x, a + b.y);
    }

    friend Vec2 operator+(const Vec2& b,const float& a) {
        return Vec2(b.x + a, b.y + a);
    }

    friend Vec2 operator-(const float& a, const Vec2& b) {
        return Vec2(a - b.x, a - b.y);
    }

    friend Vec2 operator-(const Vec2& b, const float& a) {
        return Vec2(b.x - a, b.y - a);
    }

    Vec2 operator*(const Vec2& a) {
        return Vec2(x * a.x, y * a.y);
    }

    friend Vec2 operator*(const float& a, const Vec2& b) {
        return Vec2(a * b.x, a * b.y);
    }

    friend Vec2 operator*(const Vec2& b, const float& a) {
        return Vec2(b.x * a, b.y * a);
    }

    float length(){
        return sqrtf(x * x + y * y);
    }

    float lengthSquare() {
        return x * x + y * y;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
};



struct Vec3 {
    float x, y, z;
    Vec3() {
        x = 0; y = 0; z = 0;
    }

    Vec3(const float x, const float y, const float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3 operator+(const Vec3& a) {
        return Vec3(x + a.x, y + a.y, z + a.z);
    }

    Vec3 operator-(const Vec3& a) {
        return Vec3(x - a.x, y - a.y, z - a.z);
    }

    friend Vec3 operator+(const float& a, const Vec3& b) {
        return Vec3(a + b.x, a + b.y, a + b.z);
    }

    friend Vec3 operator+(const Vec3& b,const float& a) {
        return Vec3(b.x + a, b.y + a, b.z + a);
    }

    friend Vec3 operator-(const float& a, const Vec3& b) {
        return Vec3(a - b.x, a - b.y, a - b.z);
    }

    friend Vec3 operator-(const Vec3& b, const float& a) {
        return Vec3(b.x - a, b.y - a, b.z - a);
    }

    Vec3 operator*(const Vec3& a) {
        return Vec3(x * a.x, y * a.y, z * a.z);
    }

    friend Vec3 operator*(const float& a, const Vec3& b) {
        return Vec3(a * b.x, a * b.y, a * b.z);
    }

    friend Vec3 operator*(const Vec3& b, const float& a) {
        return Vec3(b.x * a, b.y * a, b.z * a);
    }

    float length(){
        return sqrtf(x * x + y * y + z * z);
    }

    float lengthSquare() {
        return x * x + y * y + z * z;
    }

    bool operator==(const Vec3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};



struct Vec4 {
    float x, y, z, w;
    Vec4() {
        x = 0; y = 0; z = 0; w = 0;
    }

    Vec4(const float x, const float y, const float z, const float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Vec4 operator+(const Vec4& a) {
        return Vec4(x + a.x, y + a.y, z + a.z, w + a.w);
    }

    Vec4 operator-(const Vec4& a) {
        return Vec4(x - a.x, y - a.y, z - a.z, w - a.w);
    }

    friend Vec4 operator+(const float& a, const Vec4& b) {
        return Vec4(a + b.x, a + b.y, a + b.z, a + b.w);
    }

    friend Vec4 operator+(const Vec4& b,const float& a) {
        return Vec4(b.x + a, b.y + a, b.z + a, b.w + a);
    }

    friend Vec4 operator-(const float& a, const Vec4& b) {
        return Vec4(a - b.x, a - b.y, a - b.z, a - b.w);
    }

    friend Vec4 operator-(const Vec4& b, const float& a) {
        return Vec4(b.x - a, b.y - a, b.z - a, b.w - a);
    }

    Vec4 operator*(const Vec4& a) {
        return Vec4(x * a.x, y * a.y, z * a.z, z * a.w);
    }

    friend Vec4 operator*(const float& a, const Vec4& b) {
        return Vec4(a * b.x, a * b.y, a * b.z, a * b.w);
    }

    friend Vec4 operator* (const Vec4& b, const float& a) {
        return Vec4(b.x * a, b.y * a, b.z * a, b.w * a);
    }

    float length(){
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    float lengthSquare() {
        return x * x + y * y + z * z + w * w;
    }

    bool operator==(const Vec4& other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }
};


struct Matrix2x2 {
    float m[2][2] = {0};

    //Matrix2x2(){}
  
    Matrix2x2 operator+(const Matrix2x2& a) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = m[i][j] + a.m[i][j];
        return result;
    }

    Matrix2x2 operator-(const Matrix2x2& a) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = m[i][j] - a.m[i][j];
        return result;
    }

    friend Matrix2x2 operator+(const float& a,const Matrix2x2& b) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = a  + b.m[i][j];
        return result;
    }

    friend Matrix2x2 operator+(const Matrix2x2& b, const float& a) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = b.m[i][j] + a;
        return result;
    }

    friend Matrix2x2 operator-(const float& a,const Matrix2x2& b) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = a  - b.m[i][j];
        return result;
    }

    friend Matrix2x2 operator-(const Matrix2x2& b, const float& a) {
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++)
            for(unsigned int j = 0; j < 2; j++)
                result.m[i][j] = b.m[i][j] - a;
        return result;
    }

    friend Matrix2x2 operator*(const float& a,const Matrix2x2& b){
        Matrix2x2 result;
        for(unsigned int i = 0; i < 2; i++) {
            for(unsigned int j = 0; j < 2; j++) {
                result.m[i][j] = a * b.m[i][j]; 
            }
        }
        return result;
    } 

    Matrix2x2 operator*(const Matrix2x2& a){
        Matrix2x2 result;
        for(unsigned int k = 0; k < 2; k++)
            for(unsigned int i = 0; i < 2; i++) 
                for(unsigned int j = 0; j < 2; j++)
                    result.m[i][k] += m[k][j] * a.m[j][i];
         
        return result;
    }

    friend Vec2 operator*(const Matrix2x2& a, const Vec2& b){
        Vec2 result;
        result.x = a.m[0][0] * b.x + a.m[0][1] * b.y;
        result.y = a.m[1][0] * b.x + a.m[1][1] * b.y;
        return result;
    }  

    friend Vec2 operator*(const Vec2& a, const Matrix2x2& b){
        Vec2 result;
        result.x = a.x * b.m[0][0] + a.y * b.m[1][0];
        result.y = a.x * b.m[0][1] + a.y * b.m[1][1];
        return result;
    }  
};


struct Matrix3x3 {
    float m[3][3] = {0};

    //Matrix3x3(){}

    Matrix3x3 operator+(const Matrix3x3& a) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = m[i][j] + a.m[i][j];
        return result;
    }

    Matrix3x3 operator-(const Matrix3x3& a) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = m[i][j] - a.m[i][j];
        return result;
    }

    friend Matrix3x3 operator+(const float& a,const Matrix3x3& b) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = a  + b.m[i][j];
        return result;
    }

    friend Matrix3x3 operator+(const Matrix3x3& b, const float& a) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = b.m[i][j] + a;
        return result;
    }

    friend Matrix3x3 operator-(const float& a,const Matrix3x3& b) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = a  - b.m[i][j];
        return result;
    }

    friend Matrix3x3 operator-(const Matrix3x3& b, const float& a) {
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++)
            for(unsigned int j = 0; j < 3; j++)
                result.m[i][j] = b.m[i][j] - a;
        return result;
    }

   friend Matrix3x3 operator*(const float& a,const Matrix3x3& b){
        Matrix3x3 result;
        for(unsigned int i = 0; i < 3; i++) {
            for(unsigned int j = 0; j < 3; j++) {
                result.m[i][j] = a * b.m[i][j]; 
            }
        }
        return result;
    } 

    Matrix3x3 operator*(const Matrix3x3& a){
        Matrix3x3 result;
        for(unsigned int k = 0; k < 3; k++)
            for(unsigned int i = 0; i < 3; i++) 
                for(unsigned int j = 0; j < 3; j++)
                    result.m[i][k] += m[k][j] * a.m[j][i];
         
        return result;
    }

    friend Vec3 operator*(const Matrix3x3& a, const Vec3& b){
        Vec3 result;
        result.x = a.m[0][0] * b.x + a.m[0][1] * b.y + a.m[0][2] * b.z;
        result.y = a.m[1][0] * b.x + a.m[1][1] * b.y + a.m[1][2] * b.z;
        result.z = a.m[2][0] * b.x + a.m[2][1] * b.y + a.m[2][2] * b.z;
        return result;
    }  

    friend Vec3 operator*(const Vec3& a, const Matrix3x3& b){
        Vec3 result;
        result.x = a.x * b.m[0][0] + a.y * b.m[1][0] + a.z * b.m[2][0];
        result.y = a.x * b.m[0][1] + a.y * b.m[1][1] + a.z * b.m[2][1];
        result.y = a.x * b.m[0][2] + a.y * b.m[1][2] + a.z * b.m[2][2];
        return result;
    }   
};


struct Matrix4x4 {
    float m[4][4] = {0};

    //Matrix4x4(){}

    Matrix4x4 operator+(const Matrix4x4& a) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = m[i][j] + a.m[i][j];
        return result;
    }

    Matrix4x4 operator-(const Matrix4x4& a) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = m[i][j] - a.m[i][j];
        return result;
    }

    friend Matrix4x4 operator+(const float& a,const Matrix4x4& b) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = a  + b.m[i][j];
        return result;
    }

    friend Matrix4x4 operator+(const Matrix4x4& b, const float& a) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = b.m[i][j] + a;
        return result;
    }

    friend Matrix4x4 operator-(const float& a,const Matrix4x4& b) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = a  - b.m[i][j];
        return result;
    }

    friend Matrix4x4 operator-(const Matrix4x4& b, const float& a) {
        Matrix4x4 result;
        for(unsigned int i = 0; i < 4; i++)
            for(unsigned int j = 0; j < 4; j++)
                result.m[i][j] = b.m[i][j] - a;
        return result;
    }

    Matrix4x4 operator*(const Matrix4x4& a){
        Matrix4x4 result;
        for(unsigned int k = 0; k < 4; k++)
            for(unsigned int i = 0; i < 4; i++) 
                for(unsigned int j = 0; j < 4; j++)
                    result.m[i][k] += m[k][j] * a.m[j][i];
         
        return result;
    }

    friend Vec4 operator*(const Matrix4x4& a, const Vec4& b){
        Vec4 result;
        result.x = a.m[0][0] * b.x + a.m[0][1] * b.y + a.m[0][2] * b.z + a.m[0][3] * b.w;
        result.y = a.m[1][0] * b.x + a.m[1][1] * b.y + a.m[1][2] * b.z + a.m[1][3] * b.w;
        result.z = a.m[2][0] * b.x + a.m[2][1] * b.y + a.m[2][2] * b.z + a.m[2][3] * b.w;
        result.w = a.m[3][0] * b.x + a.m[3][1] * b.y + a.m[3][2] * b.z + a.m[3][3] * b.w;
        return result;
    }  
    
    friend Vec4 operator*(const Vec4& a, const Matrix4x4& b){
        Vec4 result;
        result.x = a.x * b.m[0][0] + a.y * b.m[1][0] + a.z * b.m[2][0] + a.w * b.m[3][0];
        result.y = a.x * b.m[0][1] + a.y * b.m[1][1] + a.z * b.m[2][1] + a.w * b.m[3][1];
        result.z = a.x * b.m[0][2] + a.y * b.m[1][2] + a.z * b.m[2][2] + a.w * b.m[3][2];
        result.w = a.x * b.m[0][3] + a.y * b.m[1][3] + a.z * b.m[2][3] + a.w * b.m[3][3];
        return result;
    }   
};

namespace Math {
#define PI  3.14159274101257324219f

    Vec2 normalize(Vec2 a); 
    Vec3 normalize(Vec3 a); 
    Vec4 normalize(Vec4 a); 

    Vec3 crossProduct(const Vec3& a, const Vec3& b);

    float dotProduct(const Vec2& a, const Vec2& b);
    float dotProduct(const Vec3& a, const Vec3& b);
    float dotProduct(const Vec4& a, const Vec4& b);

    Matrix4x4 scale(const float x, const float y, const float z);
    Matrix4x4 scale(const Vec3 a);

    Matrix4x4 translate(const float x, const float y, const float z);
    Matrix4x4 translate(const Vec3 a);

    Matrix4x4 rotateX(const float radian);
    Matrix4x4 rotateY(const float radian);
    Matrix4x4 rotateZ(const float radian);

    Matrix4x4 rotate(const float x, const float y, const float z);
    Matrix4x4 rotate(const Vec3 rot);

    Matrix4x4 perspective(const float aspectRatio, const float fov, const float near, const float far);
    Matrix4x4 ortho(const float left, const float right, const float bottom, const float top, const float near, const float far);

    Matrix4x4 lookat(Vec3 eye, Vec3 at, Vec3 up);

    Matrix2x2 multiply(const Matrix2x2& a, bool transposeA, const Matrix2x2& b, bool transposeB);
    Matrix3x3 multiply(const Matrix3x3& a, bool transposeA, const Matrix3x3& b, bool transposeB);
    Matrix4x4 multiply(const Matrix4x4& a, bool transposeA, const Matrix4x4& b, bool transposeB);

    float toRadian(const float degree);
}

