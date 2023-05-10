#pragma once

#include <windows.h>
#include <cstdint>
#include <cmath>

typedef uint64_t Void;
typedef uint64_t Any;
typedef unsigned int uint;
typedef unsigned int Hash;

typedef int AnimScene;
typedef int Blip;
typedef int Cam;
typedef int CoverPoint;
typedef int Entity;
typedef int FireId;
typedef int Group;
typedef int Interior;
typedef int ItemSet;
typedef Hash Model;
typedef int Object;
typedef int Ped;
typedef int PersChar;
typedef int Pickup;
typedef uint Player;
typedef int PopZone;
typedef int Prompt;
typedef int PropSet;
typedef int Rope;
typedef int Scenario;
typedef int ScrHandle;
typedef int Sphere;
typedef int TaskSequence;
typedef int Train;
typedef int Vehicle;
typedef int Volume;
typedef Hash Weapon;

typedef char			   s8;
typedef short              s16;
typedef int                s32;
typedef long long          s64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#define ALIGN8 __declspec(align(8))

struct Vector3
{
	ALIGN8 float x;
	ALIGN8 float y;
	ALIGN8 float z;
};
static_assert(sizeof(Vector3) == 24, "");

struct Vector2
{
	ALIGN8 float x;
	ALIGN8 float y;
};
static_assert(sizeof(Vector2) == 16, "");
