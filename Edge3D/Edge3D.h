#pragma once

#include "Application.h"
#include "EntryPoint.h" // main function is here
#include "Log.h"

#include "Window/Window.h"

// Events
#include "Event/Event.h"
#include "Event/WindowEvent.h"
#include "Event/MouseEvent.h"
#include "Event/KeyEvent.h"

//Scene System
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

// Opengl Functions
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/Texture.h"
#include "OpenGL/FrameBuffer.h"

// Math libray for opengl
#include "Math/Math.h"

#include "Input/InputHandler.h"

#include "Utility/ImGuiLayer.h"
#include "Utility/Camera.h"
#include "Utility/OrthographicCamera.h"
#include "Utility/PerspectiveCamera.h"
#include "Utility/ShapeRenderer2D.h"
#include "Utility/SpriteBatch.h"
#include "Utility/Utility.h"

//#include "Model/Capsule.h"
//#include "Model/Circle3D.h"
//#include "Model/Cone.h"
//#include "Model/Cube.h"
//#include "Model/Cylinder.h"
//#include "Model/Plane.h"
//#include "Model/Shape.h"
//#include "Model/Sphere.h"
//#include "Model/Terrain.h"

#include "Skybox/SpherecalSkybox.h"

//Meshes
#include "Model/Meshes/SphereMesh.h"
#include "Model/Meshes/ConeMesh.h"
#include "Model/Meshes/CapsuleMesh.h"
#include "Model/Meshes/CylinderMesh.h"
#include "Model/Meshes/TorusMesh.h"


//Random classes
#include "Random/Xorshift.h"
#include "Random/Xorshift64.h"
#include "Random/MersenneTwister.h"
#include "Random/MersenneTwister64.h"
#include "Random/AddWithCarry.h"
#include "Random/LaggedFibonacciGenerator.h"
#include "Random/LaggedFibonacciGenerator64.h"
#include "Random/MultiplyWithCarry.h"

//Bit converter, from various int types to float and double
#include "Random/BitConverter.h"