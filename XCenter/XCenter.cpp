#include "stdafx.h"
#include "XCenter.h"

XCenter* g_pCenter = NULL;

int main(int argc, char *argv[])
{
    g_pCenter = new XCenter();
    g_pCenter->Init("XCenter", false);
    g_pCenter->Run();
    g_pCenter->UnInit();
    XY_DELETE(g_pCenter);
	return 0;
}

void XCenter::Activate()
{
    if (g_pCenter->GetFrame() % GAME_FPS == 0)
        Log("Capacity:<%d> Act:%f Free:%f", g_pCenter->GetFrame(), g_pCenter->GetActiveCost(), g_pCenter->GetFreeCost());
}
