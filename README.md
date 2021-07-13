# PSG ayFXplayer MSX SDCC Library (fR3eL Project)

```
Architecture: MSX
Format: C Object (SDCC .rel)
Programming language: C and Z80 assembler

Authors: 
        Original code: SapphiRe  <http://z80st.auic.es>
        This adaptation to SDCC: mvac7 
        
In examples/test_ROM software, PT3 songs:
- "Mathematical brain" by Makinavaja (for XI MICROCOMPO AY) 
```


---

## Description

This project is an opensource library to launch sound effects for the PSG AY-3-8910 or compatible. 

It is an adaptation of ayFX REPLAYER Fixed Volume v1.31 made by SapphiRe. http://www.z80st.es/blog/tags/ayfx

The names of the functions have been changed to adapt it to the nomenclature of the sound libraries of the fR3eL project.

To function it requires the AY38910BF library. The function that dumps the registers buffer to the sound processor (PlayAY), 
has been moved to AY38910BF library because it is applicable to other music libraries (such as PT3player).
You also get the possibility of selecting to which sound processor the data processed by the players should be directed (internal or external AY).

The [AY38910BF](https://github.com/mvac7/SDCC_AY38910BF_Lib), [PT3player](https://github.com/mvac7/SDCC_AY38910BF_Lib) and ayFXplayer libraries are designed to work together, so you will have a system to provide music and effects in game development.

![AY Sound System](https://raw.githubusercontent.com/mvac7/SDCC_AY38910BF_Lib/master/doc/AYlibs.png)

In the source code (\examples), you can find applications for testing and learning purposes.

![TEST AYfxplayer](https://raw.githubusercontent.com/mvac7/SDCC_ayFXplayer/master/examples/test02_ayFX_PT3/GFX/TESTFX2.png)

This library is part of the [MSX fR3eL Project](https://github.com/mvac7/SDCC_MSX_fR3eL).

Enjoy it!                           



## Features

* Designed for developing MSX applications using Small Device C Compiler (SDCC).
* It does not use the BIOS so it can be used to program both ROMs or MSX-DOS executables.
* Up to 256 effects per bank. Multiple banks can be defined and changed from the initialization function.
* Priority parameter. The effects with the highest priority will be executed on the ones with the lowest.
* 2 modes of operation:
   1) Fixed. They will be heard on the indicated channel.
   2) Cyclical. The ayFX sample will be mixed on a different PSG channel in each frame.




## History of versions

* v1.0 (7 July 2021) First SDCC version.



## Requirements

### For compile:

* Small Device C Compiler (SDCC) v4.1 http://sdcc.sourceforge.net/
* Hex2bin v2.5 http://hex2bin.sourceforge.net/
* [PSG AY-3-8910 BF MSX SDCC Library](https://github.com/mvac7/SDCC_AY38910BF_Lib)


### For create a FXs bank
 
* [AY Sound FX Editor](https://shiru.untergrund.net/software.shtml) by Shiru
* [AY Sound FX Editor (Improved)](https://github.com/Threetwosevensixseven/ayfxedit-improved) by Shiru and Robin Verhagen-Guest



---

## Definitions

### ayFX Operating modes

This parameter can be changed from the `ayFX_Setup` function or directly by writing in the static variable `ayFX_MODE`. 

Label | Value
-- | -- 
ayFX_FIXED | 0
ayFX_CYCLE | 1


  
## Functions

* `ayFX_Setup(unsigned int BANKaddr, char mode, char channel)` ayFX player setup
* `ayFX_SetChannel(char channel)` Change output channel (useful only in Fixed channel mode)
* `char ayFX_Play(char FXnumber, char FXpriority)` Play FX
* `ayFX_Decode()` Generates the values of registers of a frame of the ayFX in progress. It has to be executed in every frame, not necessarily in the interrupt routine.



---

## Acknowledgments
  
I want to give a special thanks to all those who freely share their knowledge with the MSX developer community.

* Sapphire/Z80ST > [WEB](http://z80st.auic.es/)
* Shiru > [WEB](https://shiru.untergrund.net/software.shtml)
* Avelino Herrera > [(WEB)](http://msx.avelinoherrera.com)
* Nerlaska > [Blog](http://albertodehoyonebot.blogspot.com.es)
* Marq/Lieves!Tuore > [Marq](http://www.kameli.net/marq/) [Lieves!Tuore](http://www.kameli.net/lt/)
* MSXKun/Paxanga soft > [(WEB)](http://paxangasoft.retroinvaders.com/)
* [Fubukimaru](https://github.com/Fubukimaru) > [Blog](http://www.gamerachan.org/fubu/)
* Andrear > [Blog](http://andrear.altervista.org/home/msxsoftware.php)
* Ramones > [MSXblog](https://www.msxblog.es/tutoriales-de-programacion-en-ensamblador-ramones/) - [MSXbanzai](http://msxbanzai.tni.nl/dev/faq.html)
* Fernando García > [(Curso)](http://www.z80st.es/cursos/bitvision-assembler)
* Eric Boez > [gitHub](https://github.com/ericb59)
* MSX Assembly Page > [WEB](http://map.grauw.nl/resources/msxbios.php)
* Portar MSX Tech Doc > [WEB](http://nocash.emubase.de/portar.htm)
* MSX Resource Center > [Wiki](https://www.msx.org/wiki/)
* Karoshi MSX Community (RIP 2007-2020)
* BlueMSX emulator >> [WEB](http://www.bluemsx.com/)
* OpenMSX emulator >> [WEB](http://openmsx.sourceforge.net/)
* Meisei emulator >> ?
