// CsgoCheat.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
#include <iostream>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include "csgo.hpp"
#include "MemMan.h"
#include "Xor.h"
#include <vector>
#include <string.h>
#include <string>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <math.h>

#define KEY_UP 0
#define KEY_DOWN -32768
#define PI 3.14159265358979323846

MemMan memory;

struct vector3d
{
	float x, y, z;
};

struct {

	std::vector<DWORD> enemieentity;
	std::vector<DWORD> pos;
	std::vector<bool> ragemode;

}enemies_info;

struct {
	int ActualTeam;
	DWORD localPlayer;
	DWORD Module;
	DWORD glowObject;
	DWORD engineAddress; 
	DWORD clientState;
}variables;


void Shoot()
{
	memory.writeMem<int>(variables.Module + hazedumper::signatures::dwForceAttack, 5);
	Sleep(20);
	memory.writeMem<int>(variables.Module + hazedumper::signatures::dwForceAttack, 4);

}

bool CheckWhenFire()
{
	int Crosshair = memory.readMem<int>(variables.localPlayer + hazedumper::netvars::m_iCrosshairId);
	

	if (Crosshair > 0 && Crosshair < 64)
	{
		DWORD entity = memory.readMem<DWORD>(variables.Module + hazedumper::signatures::dwEntityList + ((Crosshair) * 0x10));

		int eTeam = memory.readMem<int>(entity + hazedumper::netvars::m_iTeamNum);
		int eHealth = memory.readMem<int>(entity + hazedumper::netvars::m_iHealth);
		
		if (eTeam != variables.ActualTeam && eHealth > 0 && !memory.readMem<bool>(entity + hazedumper::netvars::m_bGunGameImmunity))
		{
			return true;
		}
	}
	return false;
}

void TriggerBot()
{
	if (CheckWhenFire())
	{
		Shoot();
	}
}

void WallHack()
{
	
	for (int i = 0; i < enemies_info.enemieentity.size(); i++)
	{
		DWORD entity = enemies_info.enemieentity[i];

		if (entity != NULL)
		{

			int glowIndexEntity = memory.readMem<int>(entity + hazedumper::netvars::m_iGlowIndex);
			if (!enemies_info.ragemode[i])
			{


				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x4), 5);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x8), 0);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0xC), 0);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x10), 0.75);
				// si alumbras aliados esto también 
			}
			else
			{
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x4), 2);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x8), 2);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0xC), 0);
				memory.writeMem<float>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x10), 0.75);
			}

			memory.writeMem<bool>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x24), true);
			memory.writeMem<bool>(variables.glowObject + ((glowIndexEntity * 0x38) + 0x25), false);
			
		}
	}
}


void MostrarEnemiesInfo()
{

	std::cout << "----- ENEMIES INFO -----" << std::endl << std::endl;
	for (int i = 0; i < enemies_info.enemieentity.size(); i++)
	{
		//gracias a la posición o la entidad, según como lo hagamos, por eso me guardo las dos cosas, sacar el nombre, de momento haré un cout de la entityID
		
		DWORD Radarbase = memory.readMem<DWORD>((DWORD)variables.Module + hazedumper::signatures::dwRadarBase);  
		DWORD Radar_hud = memory.readMem<DWORD>(Radarbase + 0x74);
		
		wchar_t cName[32];
		
		for (int j = 0; j < 32; j++)
		{
			cName[j] = memory.readMem<wchar_t>((Radar_hud + 0x300 + (0x174 * enemies_info.pos[i])) + j);
		}
		
		std::cout << i + 1 << ".  ->   ";

		for (int i = 0; i < 32; i++)
		{
			printf("%c", cName[i]);
		}

		if (enemies_info.ragemode[i])
		{
			std::cout << "      RAGE MODE: ON";
		}

		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
}

void Busqueda()
{
	enemies_info.enemieentity.clear();
	enemies_info.pos.clear();
	enemies_info.ragemode.clear(); 
	

	variables.glowObject = memory.readMem<DWORD>(variables.Module + hazedumper::signatures::dwGlowObjectManager);

	for (int i = 2; i < 32; i++)
	{
		DWORD entity = memory.readMem<DWORD>(variables.Module + hazedumper::signatures::dwEntityList + i * 0x10);

		if (entity != NULL)
		{

			int Team = memory.readMem<int>(entity + hazedumper::netvars::m_iTeamNum);
			if ((Team != variables.ActualTeam) && (Team != 0))
			{
				enemies_info.enemieentity.push_back(entity);
				enemies_info.pos.push_back(i-1);
				enemies_info.ragemode.push_back(false);

			}
		}
	}

	MostrarEnemiesInfo();
}

void BunnyHop()
{

	int flag = memory.readMem<int>(variables.localPlayer + hazedumper::netvars::m_fFlags);

	if (GetAsyncKeyState(VK_SPACE) && flag == 257)
	{
		memory.writeMem<int>(variables.Module + hazedumper::signatures::dwForceJump, 6);
	}
}

void NoFlash()
{

	if (memory.readMem<int>(variables.localPlayer + hazedumper::netvars::m_flFlashDuration) > 0)
	{
		memory.writeMem<int>(variables.localPlayer + hazedumper::netvars::m_flFlashDuration, 0);
	}

}

int findClosestEnemy(vector3d* final)
{
	vector3d enemiesHead;
	vector3d myHead;
	vector3d substract;
	float distance; 
	int position = -1;

	DWORD pta = NULL;

	double minDistance = 9999999;

	DWORD MyBaseBoneMatrix = memory.readMem<DWORD>(variables.localPlayer + hazedumper::netvars::m_dwBoneMatrix);

	myHead.x = memory.readMem<float>(MyBaseBoneMatrix + 0x30 * 8 + 0x0C);
	myHead.y = memory.readMem<float>(MyBaseBoneMatrix + 0x30 * 8 + 0x1C);
	myHead.z = memory.readMem<float>(MyBaseBoneMatrix + 0x30 * 8 + 0x2C);

	//std::cout << myHead.x << " " << myHead.y << " " << myHead.z << std::endl;

	for (int i = 0; i < enemies_info.enemieentity.size(); i++)
	{
		
		if ((memory.readMem<int>(enemies_info.enemieentity[i] + hazedumper::netvars::m_iHealth) > 0) && (!memory.readMem<bool>(enemies_info.enemieentity[i] + hazedumper::signatures::m_bDormant)) && (!memory.readMem<bool>(enemies_info.enemieentity[i] + hazedumper::netvars::m_bGunGameImmunity)) && (memory.readMem<bool>(enemies_info.enemieentity[i] + hazedumper::netvars::m_bSpotted)))
		{
			DWORD EnemieBaseBoneMatrix = memory.readMem<DWORD>(enemies_info.enemieentity[i] + hazedumper::netvars::m_dwBoneMatrix);

			enemiesHead.x = memory.readMem<float>(EnemieBaseBoneMatrix + 0x30 * 8 + 0x0C);
			enemiesHead.y = memory.readMem<float>(EnemieBaseBoneMatrix + 0x30 * 8 + 0x1C);
			enemiesHead.z = memory.readMem<float>(EnemieBaseBoneMatrix + 0x30 * 8 + 0x2C);

			//std::cout << enemiesHead.x << " " << enemiesHead.y << " " << enemiesHead.z << std::endl;

			substract.x = myHead.x - enemiesHead.x;
			substract.y = myHead.y - enemiesHead.y;
			substract.z = myHead.z - enemiesHead.z;

			//std::cout << substract.x << " " << substract.y << " " << substract.z << std::endl;

			distance = sqrt(pow(substract.x, 2) + pow(substract.y, 2) + pow(substract.z, 2));

			//std::cout << distance << std::endl;

			if (distance < minDistance)
			{
				minDistance = distance;

				position = i;

				final->x = substract.x;
				final->y = substract.y;
				final->z = substract.z;

				//std::cout << final->x << " " << final->y << " " << final->z << std::endl;

			}
		}
	}

	//std::cout << position << std::endl;

	return position;

}

void AimBot()
{
	vector3d final;
	final.x = 0;
	final.y = 0;
	final.z = 0;
	vector3d angles;
	vector3d punch; 

	int position = findClosestEnemy(&final);
	//std::cout << position << std::endl;
	//std::cout << final.x << " " << final.y << " " << final.z << std::endl;


	if (position >= 0 && position < enemies_info.enemieentity.size())
	{
		float hypo = sqrt(pow(final.x, 2) + pow(final.y, 2));

		angles.x = asinf(final.z / hypo) * (180.0f / PI);
		angles.y = atanf(final.y / final.x) * (180.0f / PI) + !((*(DWORD*)&final.x) >> 31 & 1) * 180.0f;
		angles.z = 0.0f;

		std::cout << angles.x << " " << angles.y << " " << angles.z << std::endl;

		punch = memory.readMem<vector3d>(variables.localPlayer + hazedumper::netvars::m_aimPunchAngle);

		if (!enemies_info.ragemode[position])
		{
			angles.x -= punch.x * 2.5;
			angles.y -= punch.y * 2.5;
			angles.z -= punch.z * 2.5;

			vector3d currentAngles = memory.readMem<vector3d>(variables.clientState + hazedumper::signatures::dwClientState_ViewAngles);

			angles.x -= currentAngles.x;
			angles.y -= currentAngles.y;
			angles.z -= currentAngles.z;

			angles.x = angles.x / 90;
			angles.y = angles.y / 90;
			angles.z = angles.z / 90;

			angles.x += currentAngles.x;
			angles.y += currentAngles.y;
			angles.z += currentAngles.z;

			
		}
		else
		{
			
			angles.x -= punch.x * 1.9;
			angles.y -= punch.y * 1.9;
			angles.z -= punch.z * 1.9;
		}

		memory.writeMem<vector3d>(variables.clientState + hazedumper::signatures::dwClientState_ViewAngles, angles);
	}

}

int main()
{	
	//Program variables 

	bool Trigger = false;
	bool Wallhack = false;
	bool PrimeraBusqueda = false;
	bool Bunny = true;
	bool Aim = false;
	int proc = memory.getProcess(XOR("csgo.exe"));



	variables.Module = memory.getModule(proc, XOR("client_panorama.dll"));
	variables.engineAddress = memory.getModule(proc, XOR("engine.dll"));
	variables.clientState = memory.readMem<DWORD>(variables.engineAddress + hazedumper::signatures::dwClientState);

	do 
	{
		variables.localPlayer = memory.readMem<DWORD>(variables.Module + hazedumper::signatures::dwLocalPlayer);

	}while (variables.localPlayer == NULL);
	

	while (!(GetAsyncKeyState(VK_F10) & 1))
	{
		variables.ActualTeam = memory.readMem<int>(variables.localPlayer + hazedumper::netvars::m_iTeamNum);
		

		if (GetAsyncKeyState(VK_F2) & 1)
			Trigger = !Trigger;
		if (GetAsyncKeyState(VK_F1) & 1)
			Wallhack = !Wallhack;
		
		if (GetAsyncKeyState(VK_F4) & 1)
			Aim = !Aim;


		if (Aim)
		{
			AimBot();
		}


		if (GetAsyncKeyState(0x46) && !Aim)
		{
			AimBot();
		}

		if ((GetAsyncKeyState(0x59) & 1 || GetAsyncKeyState(0x55) & 1) && Bunny)
		{
			Bunny = false;
		}

		if ((GetAsyncKeyState(VK_RETURN) & 1 || GetAsyncKeyState(VK_ESCAPE) & 1) && !Bunny)
		{
			Bunny = true;
		}

		if (Bunny)
		{
			BunnyHop();
		}

		if (Trigger)
		{
			TriggerBot();
		}

		if (GetAsyncKeyState(VK_RBUTTON) && !Trigger)
		{
			if (CheckWhenFire())
			{
				Shoot();
			}
		}
		
		if (Wallhack && PrimeraBusqueda)
		{
			WallHack();
		}

		NoFlash();

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			PrimeraBusqueda = true;
			Busqueda();
		}

		if (GetAsyncKeyState(VK_NUMPAD1) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 1)
		{
			enemies_info.ragemode[0] = !enemies_info.ragemode[0];
			MostrarEnemiesInfo();
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 2)
		{
			enemies_info.ragemode[1] = !enemies_info.ragemode[1];
			MostrarEnemiesInfo();
		}

		if (GetAsyncKeyState(VK_NUMPAD3) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 3)
		{
			enemies_info.ragemode[2] = !enemies_info.ragemode[2];
			MostrarEnemiesInfo();
		}
		
		if (GetAsyncKeyState(VK_NUMPAD4) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 4)
		{
			enemies_info.ragemode[3] = !enemies_info.ragemode[3];
			MostrarEnemiesInfo();
		}

		if (GetAsyncKeyState(VK_NUMPAD5) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 5)
		{
			enemies_info.ragemode[4] = !enemies_info.ragemode[4];
			MostrarEnemiesInfo();
		}
		if (GetAsyncKeyState(VK_NUMPAD6) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 6)
		{
			enemies_info.ragemode[5] = !enemies_info.ragemode[5];
			MostrarEnemiesInfo();
		}

		if (GetAsyncKeyState(VK_NUMPAD7) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 7)
		{
			enemies_info.ragemode[6] = !enemies_info.ragemode[6];
			MostrarEnemiesInfo();
		}
		if (GetAsyncKeyState(VK_NUMPAD8) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 8)
		{
			enemies_info.ragemode[7] = !enemies_info.ragemode[7];
			MostrarEnemiesInfo();
		}
		if (GetAsyncKeyState(VK_NUMPAD9) & 1 && PrimeraBusqueda && enemies_info.ragemode.size() >= 9)
		{
			enemies_info.ragemode[8] = !enemies_info.ragemode[8];
			MostrarEnemiesInfo();
		}

	}

}

// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
