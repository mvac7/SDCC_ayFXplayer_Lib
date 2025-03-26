/* =============================================================================
Test 2 SDCC ayFXplayer Lib with PT3player Lib
Version: 1.1 (13/03/2025)
Author: mvac7/303bcn
Architecture: MSX
Format:		ROM 16K
Language:	C and Z80 assembler
Compiler:	SDCC v4.4 or newer

  
PT3 song:
- "Mathematical brain" by Makinavaja (for XI MICROCOMPO AY) 
 
Description:
Test the different functionalities of the SDCC adaptation of the ayFXplayer.

History of versions: (dd/mm/yyyy)
- v1.1 (13/03/2025) update to SDCC (4.1.12) Z80 calling convention 
                    test PSG_ayFXplayer v1.2 library
- 1.0 (25/6/2021)
============================================================================= */

#include "../include/newTypes.h"
#include "../include/msxSystemVariables.h"
#include "../include/msxBIOS.h"

#include "../include/interruptM1_Hooks.h"
#include "../include/memory_Z80.h"
#include "../include/keyboard_MSX.h"
#include "../include/VDP_TMS9918A_MSXBIOS.h"
#include "../include/VDP_PRINT.h"
#include "../include/unWRLE2toVRAM.h"

#include "../include/PSG_AY38910BF.h"
#include "../include/PSG_ayFXplayer.h"

#include "../include/PT3player.h"
#include "../include/PT3player_NoteTable2.h"

#include "../include/ayFX_data.h"
#include "../include/Test_GFX.h"
#include "../include/maki_CompoAY19v2_PT3.h"




#define VUMETERSPRBUFF 12*4


// Function Declarations -------------------------------------------------------
void my_TIMI(void);

void DumpOAM(void);

void WAIT(uint cicles);
void LOCATE(char x, char y);

void InitVumeter(void);
void ShowVumeter(char channel, char value);

void Pause(void);

void PlayFX(char number);

void ChangeMode(void);
void ChangeChannel(char channel);
void ShowChannel(void);
//void ShowAYtype(void);


// constants  ------------------------------------------------------------------
const char text01[] = "TESTFX2 ayFXplayer+PT3player";
//const char text02[] = "v1.0 (25/6/2021)";

//const char presskey[] = "Press a key to Play";

const char aytypeSTRINGS[2][7]={"INTERN","EXTERN"};
const char ayfxmodeSTRINGS[2][6]={"FIXED","CYCLE"};
const char channelSTRINGS[3][2]={"A","B","C"};
//const char vfreqSTRINGS[2][5]={"NTSC","PAL "};

const char PT3help[]={
0x70,0x74,0x33,0x70,0x6C,0x61,0x79,0x65,0x72,0x20,0xC7,0xC8,0xC9,0x50,0x61,0x75,
0x73,0x65,0x2F,0x52,0x65,0x73,0x74,0x6F,0x72,0x65,0};


// global variable definition --------------------------------------------------

char VALUE;

char SPRBUFFER[VUMETERSPRBUFF];  //20*4 =72B

// pressure control variables
boolean Row0pressed;
boolean Row1pressed;
boolean Row2pressed;
boolean Row3pressed;  
boolean Row4pressed;
boolean Row5pressed;
boolean Row6pressed;
boolean Row7pressed;
boolean Row8pressed;

boolean _loopState;

//char _isPlay;

uint firstPATaddr;




// Functions -------------------------------------------------------------------


//
void main(void)
{
	char keyPressed;
	char result;
	char AYtype = 0;
	 
	uint conta = 0;

	//initialize pressure control variables
	Row0pressed=false;
	Row1pressed=false;
	Row2pressed=false;
	Row3pressed=false;  
	Row4pressed=false;
	Row5pressed=false;
	Row6pressed=false;
	Row7pressed=false;
	Row8pressed=false;

	_loopState = false;


	ayFX_Setup((unsigned int) BANK00, ayFX_FIXED ,AY_Channel_C); // Initialize the ayFXplayer

	Player_Init();  // Initialize the PT3player

	POKE(LINL32,32); //Width(32)
	COLOR(WHITE,DARK_BLUE,LIGHT_BLUE);          
	SCREEN(1);    
	SetSpritesSize(1);

	unWRLE2toVRAM16((uint) GUI_G1_TSET_PAT, BASE7);
	unWRLE2toVRAM16((uint) GUI_G1_TSET_COL, BASE6);
	unWRLE2toVRAM16((uint) Frame_MAP,BASE5);
	 
	VLOCATE(1,1);
	VPRINT(text01);
//	VLOCATE(1,2);
//	VPRINT(text02);

	VLOCATE(1,8);
	VPRINT("Do you want to use the");
	VLOCATE(1,9);
	VPRINT("external AY? ([Y] for Yes)");

	LOCATE(1,10);
	result = INKEY();

	if (result==89 || result==121){
		AY_IOport=AY_EXTERNAL;
		AYtype++;
	}else	AY_IOport=AY_INTERNAL;
  
// -----------------------------------------------------------------------------
  
	unWRLE2toVRAM16((uint) GUI_MAP,BASE5+64); //show screen map
	
	VLOCATE(1,4);
	VPRINT(PT3help);

	//VLOCATE(1,6);
	//VPRINT("[F1]-[F3] Channel / [TAB] Mode"); 

	VLOCATE(6,13);
	VPRINT(ayfxmodeSTRINGS[0]);
	ShowChannel();
	
	//ShowAYtype();
	VLOCATE(8,15);
	VPRINT(PSGtypeTiles[AYtype]);
	  
	InitVumeter();

	Player_InitSong((unsigned int) SONG01, (unsigned int) NT2, ON);

	VLOCATE(1,5);
	VPRINT("Name  :");
	//VLOCATE(8,4);
	VPRINTN(SONG01_name,23);

	VLOCATE(1,6);
	VPRINT("Author:");
	//VLOCATE(8,5);
	VPRINTN(SONG01_author,23);   

	Install_TIMI(my_TIMI);

	while(1)
	{
		HALT;
			
		ShowVumeter(0,AYREGS[AY_AmpA]);
		ShowVumeter(1,AYREGS[AY_AmpB]);
		ShowVumeter(2,AYREGS[AY_AmpC]);


		keyPressed = GetKeyMatrix(0);  
		if (keyPressed!=0xFF)  //pressure control of the keys
		{
		  if(Row0pressed==false)
		  {
			//if (!(keyPressed&Bit0)) {Row0pressed=true;}; // [0]
			if (!(keyPressed & Bit1)) PlayFX(0); // [1]
			if (!(keyPressed & Bit2)) PlayFX(1); // [2]
			if (!(keyPressed & Bit3)) PlayFX(2); // [3]
			if (!(keyPressed & Bit4)) PlayFX(3); // [4]
			if (!(keyPressed & Bit5)) PlayFX(4); // [5] 
			//if (!(keyPressed&Bit6)) {Row0pressed=true;}; // [6]
			//if (!(keyPressed&Bit7)) {Row0pressed=true;}; // [7]
		  }      
		}else Row0pressed=false;        


		// Keyboard row 2
		keyPressed = GetKeyMatrix(2);
		if (keyPressed!=0xFF)
		{
		  if(Row2pressed==false)
		  {
			//if (!(keyPressed&Bit0)) {Row2pressed=true;}; // [']
			//if (!(keyPressed&Bit1)) {Row2pressed=true;}; // [&] 
			//if (!(keyPressed&Bit2)) {Row2pressed=true;}; // [,]
			//if (!(keyPressed&Bit3)) {Row2pressed=true;}; // [.] 
			//if (!(keyPressed&Bit4)) {Row2pressed=true;}; // [/]
			//if (!(keyPressed&Bit5)) {Row2pressed=true;}; // [Dead key]
			if (!(keyPressed&Bit6)) PlayFX(10); // [A]
			if (!(keyPressed&Bit7)) PlayFX(19); // [B]
		  }      
		}else Row2pressed=false;


		// Keyboard row 3
		keyPressed = GetKeyMatrix(3);
		if (keyPressed!=0xFF)
		{
		  if(Row3pressed==false)
		  {
			if (!(keyPressed&Bit0)) PlayFX(17); // [C]
			if (!(keyPressed&Bit1)) PlayFX(12); // [D]
			if (!(keyPressed&Bit2)) PlayFX(7); // [E]
			if (!(keyPressed&Bit3)) PlayFX(13); // [F]
			if (!(keyPressed&Bit4)) PlayFX(14); // [G]
			//if (!(keyPressed&Bit5)) {Row3pressed=true;}; // [H]
			//if (!(keyPressed&Bit6)) {Row3pressed=true;}; // [I]
			//if (!(keyPressed&Bit7)) {Row3pressed=true;}; // [J]
		  }      
		}else Row3pressed=false;


		// Keyboard row 4
		keyPressed = GetKeyMatrix(4);
		if (keyPressed!=0xFF)
		{
		  if(Row4pressed==false)
		  {
			//if (!(keyPressed&Bit0)) {Row4pressed=true;}; // [K]
			//if (!(keyPressed&Bit1)) {Row4pressed=true;}; // [L]
			//if (!(keyPressed&Bit2)) {Row4pressed=true;}; // [M]
			//if (!(keyPressed&Bit3)) {Row4pressed=true;}; // [N]
			//if (!(keyPressed&Bit4)) {Row4pressed=true;}; // [O]
			//if (!(keyPressed&Bit5)) {Row4pressed=true;}; // [P]
			if (!(keyPressed&Bit6)) PlayFX(5); // [Q]
			if (!(keyPressed&Bit7)) PlayFX(8); // [R]
		  }      
		}else Row4pressed=false; 


		// Keyboard row 5
		keyPressed = GetKeyMatrix(5);
		if (keyPressed!=0xFF)
		{
		  if(Row5pressed==false)
		  {
			if (!(keyPressed&Bit0)) PlayFX(11); // [S]
			if (!(keyPressed&Bit1)) PlayFX(9); // [T]
			//if (!(keyPressed&Bit2)) {Row5pressed=true;}; // [U]
			if (!(keyPressed&Bit3)) PlayFX(18); // [V]
			if (!(keyPressed&Bit4)) PlayFX(6); // [W]
			if (!(keyPressed&Bit5)) PlayFX(16); // [X]
			//if (!(keyPressed&Bit6)) {Row5pressed=true;}; // [Y]
			if (!(keyPressed&Bit7)) PlayFX(15); // [Z]
		  }      
		}else Row5pressed=false;  


		// Keyboard row 6
		keyPressed = GetKeyMatrix(6);
		if (keyPressed!=0xFF)
		{
		  if(Row6pressed==false)
		  {
			//if (!(keyPressed&Bit0)) {Row6pressed=true;}; // [SHIFT]
			//if (!(keyPressed&Bit1)) {Row6pressed=true;}; // [CTRL]
			//if (!(keyPressed&Bit2)) {Row6pressed=true;}; // [GRAPH]
			//if (!(keyPressed&Bit3)) {Row6pressed=true;}; // [CAPS]
			//if (!(keyPressed&Bit4)) {Row6pressed=true;}; // [CODE]
			if (!(keyPressed&Bit5)) {Row6pressed=true;ChangeChannel(0);}; // [F1]
			if (!(keyPressed&Bit6)) {Row6pressed=true;ChangeChannel(1);}; // [F2]
			if (!(keyPressed&Bit7)) {Row6pressed=true;ChangeChannel(2);}; // [F3]
		  }      
		}else Row6pressed=false;




		// Keyboard row 7
		keyPressed = GetKeyMatrix(7);
		if (keyPressed!=0xFF)
		{
		  if(Row7pressed==false)
		  {
			//if (!(keyPressed&Bit0)) {Row7pressed=true;}; // [F4]
			//if (!(keyPressed&Bit1)) {Row7pressed=true;}; // [F5]
			//if (!(keyPressed&Bit2)) {Row7pressed=true;}; // [ESC]
			if (!(keyPressed&Bit3)) {Row7pressed=true;ChangeMode();};  // [TAB]
			if (!(keyPressed&Bit4)) {Row7pressed=true;Pause();}; // [STOP]
			//if (!(keyPressed&Bit5)) {Row7pressed=true;}; // [BS]
			//if (!(keyPressed&Bit6)) {Row7pressed=true;}; // [SELECT]
			//if (!(keyPressed&Bit7)) {Row7pressed=true;PlaySong(0);}; // [RETURN]
		  }      
		}else Row7pressed=false;


		Player_Decode();  //PT3 Process the next step in the song sequence
		ayFX_Decode();

	}

}



void my_TIMI(void) 
{
	PUSH_AF;

	PlayAY();

	DumpOAM();

	POP_AF;     
}



void DumpOAM(void)
{
__asm
	ld   HL,#_SPRBUFFER
	ld   DE,#BASE13  
	ld   BC,#VUMETERSPRBUFF
	call BIOS_LDIRVM	;Copy a block from memory to VRAM
__endasm;
}




void PlayFX(char number)
{ 
	char priory;
	char result;

	Row0pressed=true;

	priory = number/5;

	result = ayFX_Play(number,priory);

	if (result==0)
	{
		VLOCATE(9,10);
		VPrintFNumber(number+1,0x20,2);
		VLOCATE(9,11);
		VPrintFNumber(priory,0x20,2); 
	}
  
}



void Pause(void)
{    
	if ((PT3_state & Bit1)) Player_Pause();
	else Player_Resume();
}



void ChangeMode(void)
{
	ayFX_MODE = !ayFX_MODE;

	if(ayFX_MODE==ayFX_CYCLE){
		VLOCATE(10,12);
		VPRINT("-");
	}else ShowChannel();
    
	VLOCATE(6,13);
	VPRINT(ayfxmodeSTRINGS[ayFX_MODE]);
}



void ChangeChannel(char channel)
{
    ayFX_SetChannel(channel);
    ShowChannel();
    
    ayFX_MODE=ayFX_FIXED;
    VLOCATE(6,13);
	VPRINT(ayfxmodeSTRINGS[0]);
}



void ShowChannel(void)
{
    char channel = 3-ayFX_CHANNEL;
    
    VLOCATE(10,12);
	VPRINT(channelSTRINGS[channel]);
}



/*	void ShowAYtype()
{
    uint MEMaddr = (uint) AYtype_viewer[AY_TYPE];
    uint VRAMaddr = 0x19E8;
    if(AY_TYPE>0) VRAMaddr += 32;
    CopyToVRAM(MEMaddr,VRAMaddr,3);
}*/



/* =============================================================================
   WAIT
   Generates a pause in the execution of n interruptions.
   PAL: 50=1second. ; NTSC: 60=1second.
   Input    : [unsigned int] cicles 
   Output   : -
============================================================================= */ 
void WAIT(uint cicles)
{
  uint i;
  for(i=0;i<cicles;i++) HALT;
  return;
}



/* =============================================================================
LOCATE

Function:	Moves the cursor to the specified location.
Input:		[char] Position X of the cursor. (0 to 31 or 79)
			[char] Position Y of the cursor. (0 to 23)         
Output:   -
============================================================================= */
void LOCATE(char x, char y) __naked
{
x;	//A
y;	//L
__asm
	push IX

	inc  A       ;incrementa las posiciones para que se situen correctamente en la pantalla
	ld   H,A
	inc  L
	call BIOS_POSIT

	pop  IX
	ret
__endasm;
}



void ShowVumeter(char channel, char value) __naked
{
channel;	//A
value;		//L
__asm
  
	ld   C,A
	ld   A,L


;C = num channel
;A = value  
;showVumeter:

	ld   (_VALUE),A

	SLA  C
	SLA  C

	ld	 B,#0
L00107:
	ld	 a,c
	ld	 l,a
	rla
	sbc	 a,a
	ld	 h,a
	add	 hl,hl
	add	 hl,hl

    ld   DE,#_SPRBUFFER
    ADD  HL,DE
    ex   DE,HL
  
	inc	 DE
	inc	 DE

	ld   A,(_VALUE)
	cp	 #0
	jr	 NZ,L00102
	xor  A
	ld	 (DE),A
	jr   L00105
  
L00102:                       
	ld   A,(_VALUE)
	cp   #4
	jr	 C,L00104
	ld	 A,#16
	ld	 (DE),A

	ld    A,(_VALUE)
	sub   #4
	ld   (_VALUE),A
	jr	 L00105
L00104:
	ld   A,(_VALUE)
	add	 a,a
	add	 a,a
	ld	(DE),A
	xor  A
	ld   (_VALUE),A
L00105:
	inc	 C
	inc	 B
	ld	 A,B
    cp   #4
	jr	 C,L00107

	ret
  
__endasm;
}



void InitVumeter(void) __naked
{
__asm

  ld   HL,#VUMETER_SPRPAT
  ld   DE,#BASE14  
  ld   BC,#32*5
  call BIOS_LDIRVM	;Copy a block from memory to VRAM

  ld   HL,#VUMETER_OAM
  ld   DE,#_SPRBUFFER
  ld   BC,#VUMETERSPRBUFF
  ldir
  
  ret
  

VUMETER_SPRPAT:
; 0-vum0
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
; 1-vum1
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00
; 2-vum2
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00
; 3-vum3
.db 0x00,0x00,0x00,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00
; 4-vum4
.db 0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00,0x7F,0x7F,0x7F,0x00
.db 0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00,0xFE,0xFE,0xFE,0x00



;SPRITE ATRIBUTE DATAS #########################################################
; for adjust colors, changue the last valor in line


;Y,X,SPR,COLOR
VUMETER_OAM:
.db 167,8,0,2
.db 151,8,0,2
.db 135,8,0,10
.db 119,8,0,8

.db 167,24,0,2
.db 151,24,0,2
.db 135,24,0,10             
.db 119,24,0,8 

.db 167,40,0,2
.db 151,40,0,2
.db 135,40,0,10
.db 119,40,0,8
;END SPRITE ATRIBUTE DATAS #####################################################
__endasm;
}



