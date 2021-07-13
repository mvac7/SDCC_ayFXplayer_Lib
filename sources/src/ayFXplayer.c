/* =============================================================================
   PSG ayFXplayer MSX SDCC Library (fR3eL Project)
   Version: 1.0 (7 July 2021)
   Authors: Original code: SapphiRe  <http://z80st.auic.es>
            This adaptation to SDCC: mvac7 
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C and Z80 assembler

   Description:                                                              
     Opensource library to launch sound effects for PSG AY-3-8910 or compatible
     
     Based in SapphiRe's ayFX REPLAYER Fixed Volume v1.31
     
     It does not use the BIOS so it can be used to program ROMs or 
     MSX-DOS executables.
     
     <http://www.z80st.es/blog/tags/ayfx>
     
   History of versions:
     v1.0 (7 July 2021) - First SDCC version 
     ---------------------------------------------------------------------------   
     ayFX REPLAYER (original Assembler version by SapphiRe)

	 v1.31	Fixed bug on previous version, only PSG channel C worked
	 v1.3	Fixed volume and Relative volume versions on the same file, 
            conditional compilation. Support for dynamic or fixed channel 
            allocation
	 v1.2f/r	ayFX bank support
	 v1.11f/r	If a frame volume is zero then no AYREGS update
	 v1.1f/r	Fixed volume for all ayFX streams
	 v1.1	Explicit priority (as suggested by AR)
	 v1.0f	Bug fixed (error when using noise)
	 v1.0	Initial release
============================================================================= */

#include "../include/AY38910.h"
#include "../include/ayFXplayer.h"



unsigned int ayFX_BANK;    // Current ayFX Bank
char ayFX_MODE;            // ayFX mode (0 fixed; 1 cyclic)
char ayFX_CHANNEL;         // PSG channel to play the ayFX stream (1=C;2=B;3=A)
char ayFX_PRIORITY;        // Current ayFX stream priotity

unsigned int ayFX_POINTER; // Pointer to the current ayFX stream
unsigned int ayFX_TONE;	   // Current tone of the ayFX stream
char ayFX_NOISE;           // Current noise of the ayFX stream
char ayFX_VOLUME;          // Current volume of the ayFX stream


/*
ayFX_MODE:	.ds	1			; ayFX mode
ayFX_BANK:	.ds	2			; Current ayFX Bank
ayFX_PRIORITY:	.ds	1	    ; Current ayFX stream priotity
ayFX_POINTER:	.ds	2		; Pointer to the current ayFX stream
ayFX_TONE:	.ds	2			; Current tone of the ayFX stream
ayFX_NOISE:	.ds	1			; Current noise of the ayFX stream
ayFX_VOLUME:	.ds	1		; Current volume of the ayFX stream
ayFX_CHANNEL:	.ds	1		; PSG channel to play the ayFX stream
IF ( AYFXRELATIVE == 1 )
	ayFX_VT:	.ds	2		; ayFX relative volume table pointer
ENDIF
*/




/* =============================================================================
 ayFX_SETUP

 Function : ayFX replayer setup
 Input    : [unsigned int] Samples Bank address
            [char] mode    (0=Fixed channel;0=Cycle channel)
            [char] channel (0=A;1=B;2=C)
 Output   : -
============================================================================= */
void ayFX_Setup(unsigned int BANKaddr, char mode, char channel) __naked
{
BANKaddr;
mode;
channel;
__asm
    push  IX
    ld    IX,#0
    add   IX,SP

;Clear PSG buffer
    call _AY_Init   ;can be found in the AY38910BF library
    
    ld   L,4(IX)
    ld   H,5(IX)
    
    ;ld   A,7(IX)
    ld   A,#3
    SUB  7(IX)      ;Reverse the order of the channels
    ld	(#_ayFX_CHANNEL),A
  
; ayFX_SETUP ---          ayFX replayer setup          ---
    ; --- INPUT: HL -> pointer to the ayFX bank ---
    ld	(#_ayFX_BANK),HL		; Current ayFX bank
    ld   A,6(IX)                ; 
    ld	(#_ayFX_MODE),A			; Initial mode: fixed channel
    ;inc	a				        ; Starting channel (=1)
    ;ld	(#_ayFX_CHANNEL),A		; Updated
    
ayFX_END:	; --- End of an ayFX stream ---
    ld	a,#255				    ; Lowest ayFX priority
    ld	(#_ayFX_PRIORITY),A		; Priority saved (not playing ayFX stream)   
  
    pop  IX
    ret
__endasm;
}




/* =============================================================================
 ayFX_SetChannel

 Function : Change output channel (useful only in Fixed channel mode)
 Input    : [char] channel (0=A;1=B;2=C)
 Output   : -
============================================================================= */
void ayFX_SetChannel(char channel) __naked
{
channel;
__asm
    push  IX
    ld    IX,#0
    add   IX,SP
    
    ld   A,#3
    SUB  7(IX)      ;Reverse the order of the channels
    ld	(#_ayFX_CHANNEL),A
    
    pop  IX
    ret
__endasm;
}
    


/* =============================================================================
  ayFX_Play (old name: ayFX_INIT)
 
 Function : Play FX
 Input    : [char] sound to be played (0 to 255)
            [char] sound priority  0 (highest)  to 15 (lowest)
 Output   : [char] 
          0: no errors, the new sample has been correctly initialized
          1: priority error, there's a sample with higher priority being played
          2: index error, the current bank don't have a sample with such index 
============================================================================= */
char ayFX_Play(char FXnumber, char FXpriority) __naked
{
FXnumber;
FXpriority;
__asm
    push  IX
    ld    IX,#0
    add   IX,SP
   
    ld    A,4(IX)  ;sound to be played
    ld    C,5(IX)  ;sound priority

;ayFX_INIT:	---     INIT A NEW ayFX STREAM     ---
		; --- INPUT: A -> sound to be played ---
		; ---        C -> sound priority     ---
		;push	bc				; Store bc in stack
		;push	de				; Store de in stack
		;push	hl				; Store hl in stack
        
		; --- Check if the index is in the bank ---
		ld	b,a				; b:=a (new ayFX stream index)
		ld	hl,(#_ayFX_BANK)			; Current ayFX BANK
		ld	a,(hl)				; Number of samples in the bank
		or	a				; If zero (means 256 samples)...
		jr	z,_CHECK_PRI			; ...goto _CHECK_PRI
		; The bank has less than 256 samples
		ld	a,b				; a:=b (new ayFX stream index)
		cp	(hl)				; If new index is not in the bank...
		ld	a,#2				; a:=2 (error 2: Sample not in the bank)
		jr	nc,_INIT_END			; ...we cant init it
        
_CHECK_PRI:	; --- Check if the new priority is lower than the current one ---
		; ---   Remember: 0 = highest priority, 15 = lowest priority  ---
		ld	a,b				; a:=b (new ayFX stream index)
		ld	a,(_ayFX_PRIORITY)		; a:=Current ayFX stream priority
		cp	c				; If new ayFX stream priority is lower than current one...
		ld	a,#1				; a:=1 (error 1: A sample with higher priority is being played)
		jr	c,_INIT_END			; ...we dont start the new ayFX stream
		; --- Set new priority ---
		ld	a,c				; a:=New priority
		and	#0x0F				; We mask the priority
		ld	(_ayFX_PRIORITY),A		; new ayFX stream priority saved in RAM

		; --- Calculate the pointer to the new ayFX stream ---
		ld	de,(_ayFX_BANK)			; de:=Current ayFX bank
		inc	de				; de points to the increments table of the bank
		ld	l,b				; l:=b (new ayFX stream index)
		ld	h,#0				; hl:=b (new ayFX stream index)
		add	hl,hl				; hl:=hl*2
		add	hl,de				; hl:=hl+de (hl points to the correct increment)
		ld	E,(HL)				; e:=lower byte of the increment
		inc	hl				; hl points to the higher byte of the correct increment
		ld	d,(HL)				; de:=increment
		add	hl,de				; hl:=hl+de (hl points to the new ayFX stream)
		ld	(_ayFX_POINTER),HL		; Pointer saved in RAM
		xor	a				; a:=0 (no errors)
        
_INIT_END:	
        ;pop	hl				; Retrieve hl from stack
		;pop	de				; Retrieve de from stack
		;pop	bc				; Retrieve bc from stack

  
  ld   L,A   ;return L (SDCC)
  
  pop  IX
  ret
__endasm;
}



/* =============================================================================
 ayFX_Decode (ASM name: ayFX_PLAY)

 Function : Play a frame of an ayFX stream
 Input    : -
 Output   : -
============================================================================= */
void ayFX_Decode() __naked
{
__asm
    push  IX
    
;ayFX_PLAY:
		ld	a,(#_ayFX_PRIORITY)		; a:=Current ayFX stream priority
		or	a				; If priority has bit 7 on...
		jp	M,END_ayFXdecode				; ...return
        
		; --- Calculate next ayFX channel (if needed) ---
		ld	a,(#_ayFX_MODE)			; ayFX mode
		and	#1				; If bit0=0 (fixed channel)...
		jr	z,_TAKECB			; ...skip channel changing
        
		ld	hl,#_ayFX_CHANNEL			; Old ayFX playing channel
		dec	(HL)				; New ayFX playing channel
		jr	nz,_TAKECB			; If not zero jump to _TAKECB
		ld	(HL),#3				; If zero -> set channel 3
        
_TAKECB:	; --- Extract control byte from stream ---
		ld	hl,(#_ayFX_POINTER)		; Pointer to the current ayFX stream
		ld	c,(HL)				; c:=Control byte
		inc	hl				; Increment pointer
		; --- Check if there s new tone on stream ---
		bit	5,c				; If bit 5 c is off...
		jr	z,_CHECK_NN			; ...jump to _CHECK_NN (no new tone)
		; --- Extract new tone from stream ---
		ld	e,(HL)				; e:=lower byte of new tone
		inc	hl				; Increment pointer
		ld	d,(HL)				; d:=higher byte of new tone
		inc	hl				; Increment pointer
		ld	(#_ayFX_TONE),de			; ayFX tone updated
        
_CHECK_NN:	; --- Check if theres new noise on stream ---
		bit	6,c				; if bit 6 c is off...
		jr	z,_SETPOINTER			; ...jump to _SETPOINTER (no new noise)
		; --- Extract new noise from stream ---
		ld	a,(HL)				; a:=New noise
		inc	HL				; Increment pointer
		cp	#0x20				; If its an illegal value of noise (used to mark end of stream)...
		jp	Z,ayFX_END			; ...jump to ayFX_END
		ld	(#_ayFX_NOISE),A			; ayFX noise updated
        
_SETPOINTER:	; --- Update ayFX pointer ---
		ld	(#_ayFX_POINTER),HL		; Update ayFX stream pointer
		; --- Extract volume ---
		ld	a,c				; a:=Control byte
		and	#0x0F				; lower nibble

		ld	(#_ayFX_VOLUME),A			; ayFX volume updated
		jr	z,END_ayFXdecode				; ...return (dont copy ayFX values in to AYREGS)
		; -------------------------------------
		; --- COPY ayFX VALUES IN TO AYREGS ---
		; -------------------------------------
		; --- Set noise channel ---
		bit	7,c				; If noise is off...
		jr	nz,_SETMASKS			; ...jump to _SETMASKS
		ld	a,(#_ayFX_NOISE)			; ayFX noise value
		ld	(#_AYREGS+6),A			; copied in to AYREGS (noise channel)
        
_SETMASKS:	; --- Set mixer masks ---
		ld	A,C				; a:=Control byte
		and	#0x90				; Only bits 7 and 4 (noise and tone mask for psg reg 7)
		cp	#0x90				; If no noise and no tone...
		jr	z,END_ayFXdecode				; ...return (dont copy ayFX values in to AYREGS)
        
		; --- Copy ayFX values in to ARYREGS ---
		rrca					; Rotate a to the right (1 TIME)
		rrca					; Rotate a to the right (2 TIMES) (OR mask)
		ld	d,#0xDB				; d:=Mask for psg mixer (AND mask)
        
		; --- Dump to correct channel ---
		ld	hl,#_ayFX_CHANNEL			; Next ayFX playing channel
		ld	b,(HL)				; Channel counter
_CHK1:		; --- Check if playing channel was 1 ---
		djnz	_CHK2				; Decrement and jump if channel was not 1
        
_PLAY_C:	; --- Play ayFX stream on channel C ---
		call	_SETMIXER			; Set PSG mixer value (returning a=ayFX volume and hl=ayFX tone)
		ld	(#_AYREGS+10),A			; Volume copied in to AYREGS (channel C volume)
		bit	2,c				; If tone is off...
		jr	nz,END_ayFXdecode				; ...return
		ld	(#_AYREGS+4),HL			; copied in to AYREGS (channel C tone)
		pop  IX
        ret
        
_CHK2:		; --- Check if playing channel was 2 ---
		rrc	d				; Rotate right AND mask
		rrca					; Rotate right OR mask
		djnz	_CHK3				; Decrement and jump if channel was not 2
        
_PLAY_B:	; --- Play ayFX stream on channel B ---
		call	_SETMIXER			; Set PSG mixer value (returning a=ayFX volume and hl=ayFX tone)
		ld	(#_AYREGS+9),A			; Volume copied in to AYREGS (channel B volume)
		bit	1,c				; If tone is off...
		ret	nz				; ...return
        
		ld	(#_AYREGS+2),HL			; copied in to AYREGS (channel B tone)
		pop  IX
        ret
        
_CHK3:		; --- Check if playing channel was 3 ---
		rrc	d				; Rotate right AND mask
		rrca					; Rotate right OR mask
        
_PLAY_A:	; --- Play ayFX stream on channel A ---
		call	_SETMIXER			; Set PSG mixer value (returning a=ayFX volume and hl=ayFX tone)
		ld	(#_AYREGS+8),A			; Volume copied in to AYREGS (channel A volume)
		bit	0,c				; If tone is off...
		ret	nz				; ...return
        
		ld	(#_AYREGS+0),HL			; copied in to AYREGS (channel A tone)
END_ayFXdecode:
		pop  IX
        ret
        					; Return
_SETMIXER:	; --- Set PSG mixer value ---
		ld	c,a				; c:=OR mask
		ld	A,(#_AYREGS+7)			; a:=PSG mixer value
		and	d				; AND mask
		or	c				; OR mask
		ld	(#_AYREGS+7),A			; PSG mixer value updated
		ld	A,(#_ayFX_VOLUME)			; a:=ayFX volume value
		ld	HL,(#_ayFX_TONE)			; ayFX tone value
        ret
  
__endasm;
}







/* =============================================================================
 PlayAY() 

 Function : Copy buffer to AY (internal or external).
 Input    : -
 Output   : -
============================================================================= */
/*void PlayAY() __naked
{
__asm
  push IX
  
  ld   HL,#_AYREGS
    
  ld   A,(#_isAYextern)
  CP   #1
  jr   Z,EXTERN_AY

;internal AY  
;control of I/O bits of register 7 in intern AY
  ld   A,(#_AYREGS + AY_Mixer)
  AND  #0b00111111
  ld   B,A
     
  ld   A,#AY_Mixer
  out  (#AY0index),A
  in   A,(#AY0read)
  and  #0b11000000	; Mask to catch two bits of joys 
  or   B		    ; I add the new mixer state collected from the buffer
  
  ld   (#_AYREGS + AY_Mixer),A
  
  xor  A	
  ld   B,#13
  ld   C,#AY0write  ;port
AY0_LOOP:
  out  (#AY0index),A
  inc  A
  outi          ; out(C),(HL) / inc HL / dec B
  JR   NZ,AY0_LOOP
  
  out  (#AY0index),A
  jr   SETSHAPE

  
EXTERN_AY:
  xor  A	
  ld   B,#13
  ld   C,#AY1write  ;port
AY1_LOOP:
  out  (#AY1index),A
  inc  A
  outi          ; out(C),(HL) / inc HL / dec B
  JR   NZ,AY1_LOOP
  
  out  (#AY1index),A

SETSHAPE:  
;Envelope shape (reg 13)    
  ld   A,(HL)
  or   A   ;CP #0
  jr   Z,END_playAY
  
  out  (C),A
  ld   (HL),#0

END_playAY:  
  pop  IX
  ret
__endasm;
}*/







