//
// Created by Copper on 2019/10/20.
//

#ifndef CPPPLAYGROUND06_UTIL_H
#define CPPPLAYGROUND06_UTIL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"

#define SET_PRESS_FLAG(key, flag, code) if (glfwGetKey(window, key) == GLFW_PRESS) flag = code;
#define array_4(target, arg0, arg1, arg2, arg3) { target[0] = arg0; target[1] = arg1; target[2] = arg2; target[3] = arg3; }

const double pi = 3.141592653589793238462643383279502884;

typedef int RotatingCubes[4];
typedef int CubePosition[8];
typedef glm::mat4 CubeRotation[8];
typedef struct
{
    CubePosition position;
    CubeRotation rotation;
} CubeState;

void print_mat4(glm::mat4 mat)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << mat[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}

template <typename T>
T between(T x, T maximum, T minimum)
{
    if (x > maximum)
        return maximum;
    if (x < minimum)
        return minimum;
    return x;
}

int inverseRotation(int rotType)
{
    if (rotType % 2 == 0)
        return rotType + 1;
    else
        return rotType - 1;
}

void initState(CubeState& state)
{
    for (int i = 0; i < 8; i++)
    {
        state.position[i] = i;
        state.rotation[i] = glm::mat4(1.0f);
    }
}

void initRotation(CubeRotation rotation)
{
    for (int i = 0; i < 8; i++)
        rotation[i] = glm::mat4(1.0f);
}

void inferRotating(int face, int side, RotatingCubes rotating)
{

    switch (face)
    {
        case 0:
            if (side) array_4(rotating, 0, 1, 2, 3)
            else array_4(rotating, 4, 5, 6, 7)
            break;
        case 1:
            if (side) array_4(rotating, 0, 1, 4, 5)
            else array_4(rotating, 2, 3, 6, 7)
            break;
        case 2:
            if (side) array_4(rotating, 0, 2, 4, 6)
            else array_4(rotating, 1, 3, 5, 7)
            break;
    }
}

void changeRotation(int face, int direction, float angle, const RotatingCubes rotatingCubes, CubeRotation rotation)
{
    int d = direction ? 1 : -1;

    glm::mat4 _middle(1.0f);

    switch (face)
    {
        case 0:
            _middle = glm::rotate(_middle, angle, glm::vec3(d * 1.0f, 0.0f, 0.0f));
            break;
        case 1:
            _middle = glm::rotate(_middle, angle, glm::vec3(0.0f, d * 1.0f, 0.0f));
            break;
        case 2:
            _middle = glm::rotate(_middle, angle, glm::vec3(0.0f, 0.0f, d * 1.0f));
            break;
    }

//    for (int r : rotatingCubes)
//        rotation[r] = _middle * rotation[r];

    for (int i = 0; i < 4; i++)
    {
        int r = rotatingCubes[i];
        rotation[r] = _middle * rotation[r];
    }

}

void changePosition(int rotType, CubePosition position)
{
    int temp[4];

    for (int i = 0; i < 4; i++)
        temp[i] = position[rotationTransform[rotType][i + 4]];

    for (int i = 0; i < 4; i++)
        position[rotationTransform[rotType][i]] = temp[i];
}

int keyToRotType(int code, int shift)
{
    int rotType = -2;

    switch (code)
    {
        case 0:
            break;
        case 'W':
            rotType = 10;
            break;
        case 'S':
            rotType = 8;
            break;
        case 'D':
            rotType = 6;
            break;
        case 'A':
            rotType = 4;
            break;
        case 'Q':
            rotType = 2;
            break;
        case 'E':
            rotType = 0;
            break;
        default:
            rotType = -2;
    }

    return rotType + shift;
}

void rotateCube(int rotType, CubeState& state)
{
    if (rotType < 0 || rotType >= 12)
    {
        std::cerr << "Invalid Rotation Type: " << rotType << "!" << std::endl;
        return;
    }

    int face = (rotType / 4) % 3;
    int side = (rotType / 2) % 2;
    int direction = rotType % 2;

    RotatingCubes rotatingCubes;

    inferRotating(face, side, rotatingCubes);

    for (int& i : rotatingCubes)
        i = state.position[i]; // update to true rotating cubes

    changeRotation(face, direction, pi / 2, rotatingCubes, state.rotation);
    changePosition(rotType, state.position);
}

void animateCube(int rotType, const CubeState state, float angle, CubeRotation animation)
{
    if (rotType < 0 || rotType >= 12)
    {
        std::cerr << "Invalid Rotation Type: " << rotType << "!" << std::endl;
        return;
    }

    int face = (rotType / 4) % 3;
    int side = (rotType / 2) % 2;
    int direction = rotType % 2;

    CubePosition position;
    RotatingCubes rotatingCubes;

    for (int i = 0; i < 8; i++)
        position[i] = state.position[i];

    inferRotating(face, side, rotatingCubes);

    for (int& i : rotatingCubes)
        i = position[i]; // update to true rotating cubes

    for (int i = 0; i < 8; i++)
        animation[i] = glm::mat4(1.0f);

    changeRotation(face, direction, angle, rotatingCubes, animation);
}

glm::vec3 viewFrom(glm::vec3 view)
{
    float length = view[0];
    float theta = view[1];
    float phi = view[2];

    float x = length * cos(theta) * cos(phi);
    float y = length * cos(theta) * sin(phi);
    float z = length * sin(theta);

    return glm::vec3(x, y, z);
}


#endif //CPPPLAYGROUND06_UTIL_H
