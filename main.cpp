#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

int main()
{
    InitialScreen initialScreen = InitialScreen();
    initialScreen.UpdateScreen();
    initialScreen.runInitialScreen();
}