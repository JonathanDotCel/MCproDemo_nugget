// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
// 
// Standard kinda fare though, no explicit or implied warranty.
// Any issues arising from the use of this software are your own.
// 
// https://github.com/JonathanDotCel
//

#ifndef  MAINC
#define MAINC

// variadic logging
#include <stdarg.h>
#include <stdint.h>
#include "main.h"

//
// Includes
//

#include "littlelibc.h"
#include "utility.h"
#include "drawing.h"
#include "pads.h"
#include "config.h"
#include "gpu.h"
#include "hwregs.h"
#include "cyblib/mcpro.h"

#define bool int
#define true 1
#define false 0

static uint32_t wasInCritical = 0;
static uint32_t lastIRQMask = 0;

void DoStuff();


void StartDraw(){
    DrawBG();
    ClearScreenText();
}

void EndDraw(){
    Draw();
}

void QuickMessage( char * message, int param0, int param1 ){

    while ( !Released( PADRdown ) ){
        DrawBG();
        ClearScreenText();
        Blah( message, param0, param1 );
        Blah( "\n\n Press X to continue" );
        MonitorPads();
        Draw();
    }
    
}

// About a second
void Delay1s()
{

    int i;

    for (i = 0; i < 7000000; i++)
    {
        __asm__("nop\n");
    }
}

// Call before and after sending to MCPro to give time to hit record
// on the Logic Analyser, without bus noise from other peripherals
void Countdown(bool isPreCounter)
{

    int i;
    int timerStartVal = 3;

    // Finish the current frame and clear the screen
    StartDraw();
    Blah("Countdown....");
    EndDraw();

    // Do this after the
    if (isPreCounter)
    {
        int wasInCritical = 0;

        wasInCritical = EnterCritical();
        lastIRQMask = pIMASK;
        pIMASK = 0;
    }

    for (i = 3; i > 0; i--)
    {

        StartDraw();
        if (isPreCounter)
        {
            Blah("Sending in %d (Start Logic Analsyer now)", i, 0);
        }
        else
        {
            Blah("Restoring control in %d (Stop Logic Analyser now)", i, 0);
        }
        EndDraw();

        Delay1s();
    }

    if (!isPreCounter)
    {
        // If we were already in a critical section, don't re-enable interrupts
        // as we may be nested
        if (!wasInCritical)
        {
            ExitCritical();
            pIMASK = lastIRQMask;
        }
    }
}

// Little wrapper
void SendGameID(int port01, char *gameID)
{

    int returnVal = 0;
    Countdown(true);
    returnVal = MemCardPro_SendGameID(port01, strlen(gameID), gameID);
    Countdown(false);

    QuickMessage("Got return val %d\n", returnVal, 0);
}

void DrawLoop(){

    NewPrintf( "Program started...\n" );

	while(1)
	{
        

        DrawBG();
        
        ClearScreenText();

        MonitorPads();

        Blah( "\n\n" );

        Blah("\n");

        Blah("MCPRO/PSIO Test Program\n");
        Blah(" \n");

        Blah("Key Combos:\n\n");
        Blah("    X: Ping Card\n");
        Blah("   []: Send cdrom:SCUS_944.55;1\n");
        Blah("   /\\: Send SCUS_944.55\n");
        Blah("    O: Send VeryLongFileNameTestEndingWith...\n");
        Blah("L1/R1: Change channel\n");
        Blah("L2/R2: Change game\n");
        Blah("start: Reboot\n");
        Blah(" \n");


        // prev chan
        if (Released(PADL1))
        {
            int returnVal = 0;
            Countdown(true);
            returnVal = MemCardPro_PrevCH(MCPRO_PORT_0);
            Countdown(false);
        }

        // next chan
        if (Released(PADR1))
        {
            int returnVal = 0;
            Countdown(true);
            returnVal = MemCardPro_NextCH(MCPRO_PORT_0);
            Countdown(false);
        }

        // prev dir
        if (Released(PADL2))
        {
            int returnVal = 0;
            Countdown(true);
            returnVal = MemCardPro_PrevDIR(MCPRO_PORT_0);
            Countdown(false);
        }

        // next dir
        if (Released(PADR2))
        {
            int returnVal = 0;
            Countdown(true);
            returnVal = MemCardPro_NextDIR(MCPRO_PORT_0);
            Countdown(false);
        }

        // reboot
        if (Released(PADstart))
        {
            goto *(ulong *)0xBFC00000;
        }

        // ping
        if (Released(PADRdown))
        {
            int returnVal = 0;

            Countdown(true);
            returnVal = MemCardPro_Ping(MCPRO_PORT_0);
            Countdown(false);

            QuickMessage("Got return val %d\n", returnVal, 0);
        }

        // GameID
        if (Released(PADRleft))
        {
            // Gran Turismo 2
            SendGameID(MCPRO_PORT_0, "cdrom:SCUS_944.55;1");
        }

        // GameID
        if (Released(PADRup))
        {
            // Gran Turismo 2
            SendGameID(MCPRO_PORT_0, "SCUS_944.55");
        }

        // GameID
        if (Released(PADRright))
        {
            SendGameID(MCPRO_PORT_0, "VeryLongFileNameTestEndingWithAPredictableBytePattern010101");
        }
        
        Draw();
		
	}


}


int main(){
	
	//ResetEntryInt();
	ExitCritical();

	// Clear the text buffer
	InitBuffer();
	
	// Init the pads after the GPU so there are no active
	// interrupts and we get one frame of visual confirmation.
	
	NewPrintf( "Init GPU...\n" );
	InitGPU();

	NewPrintf( "Init Pads...\n" );
	InitPads();
    
    DrawLoop();
	
}




#endif // ! MAINC

