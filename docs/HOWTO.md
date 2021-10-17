# How to use the PSG ayFX Player MSX SDCC Library

---

## Index

- [1 Description](#1-Description)
- [2 Features](#2-Features)
- [3 Requirements](#3-Requirements)
- [4 AY Sound System](#4-AY-Sound-System)
- [5 Definitions](#5-Definitions)
   - [5.1 ayFX Operating modes](#51-ayFX-Operating-modes)
- [6 Functions](#6-Functions)
   - [6.1 ayFX_Setup](#61-ayFX_Setup)
   - [6.3 ayFX_SetChannel](#62-ayFX_SetChannel)
   - [6.4 ayFX_Play](#63-ayFX_Play)
   - [6.5 ayFX_Decode](#64-ayFX_Decode)
- [7 How to use](#7-How-to-use)

<br/>

---

## 1 Description

Library for launch sound effects for the PSG AY-3-8910 or compatible. 

It is an adaptation of [ayFX REPLAYER](http://www.z80st.es/blog/tags/ayfx) Fixed Volume v1.31 made by SapphiRe.

The names of the functions have been changed to adapt it to the nomenclature of the sound libraries of the fR3eL project.

To function it requires the AY38910BF library. The function that dumps the registers buffer to the sound processor (PlayAY), 
has been moved to AY38910BF library because it is applicable to other music libraries (such as PT3player).
You also get the possibility of selecting to which sound processor the data processed by the players should be directed (internal or external AY).

Use them for developing MSX applications using Small Device C Compiler (SDCC).

This project is an Open Source library. 
You can add part or all of this code in your application development or include it in other libraries/engines.

This library is part of the [`MSX fR3eL Project`](https://github.com/mvac7/SDCC_MSX_fR3eL).

Enjoy it!                           


<br/>

---

## 2 Features

- Designed for developing MSX applications using Small Device C Compiler (SDCC).
- It does not use the BIOS so it can be used to program both ROMs or MSX-DOS executables.
- Up to 256 effects per bank. Multiple banks can be defined and changed from the initialization function.
- Priority parameter. The effects with the highest priority will be executed on the ones with the lowest.
- 2 modes of operation:
   1) Fixed. They will be heard on the indicated channel.
   2) Cyclical. The ayFX sample will be mixed on a different PSG channel in each frame.



<br/>

---

## 3 Requirements

### 3.1 For compile:

- [Small Device C Compiler (SDCC) v4.1](http://sdcc.sourceforge.net/)
- [Hex2bin v2.5](http://hex2bin.sourceforge.net/)
- [PSG AY-3-8910 BF MSX SDCC Library](https://github.com/mvac7/SDCC_AY38910BF_Lib)


### 3.2 For create a FXs bank
 
- [AY Sound FX Editor](https://shiru.untergrund.net/software.shtml) by Shiru
- [AY Sound FX Editor (Improved)](https://github.com/Threetwosevensixseven/ayfxedit-improved) by Shiru and Robin Verhagen-Guest

<br/>

---

## 4 AY Sound System

The [`AY38910BF`](https://github.com/mvac7/SDCC_AY38910BF_Lib), [`PT3player`](https://github.com/mvac7/SDCC_AY38910BF_Lib) and [`ayFXplayer`](https://github.com/mvac7/SDCC_ayFXplayer) libraries are designed to work together, so you will have a system to provide music and effects in game development.

![AY Sound System](https://raw.githubusercontent.com/mvac7/SDCC_AY38910BF_Lib/master/docs/AYlibs.png)

<br/>

---

## 5 Definitions

### 5.1 ayFX Operating modes

This parameter can be changed from the `ayFX_Setup` function or directly by writing in the static variable `ayFX_MODE`. 

Label      | Value
:---       | ---: 
ayFX_FIXED | 0
ayFX_CYCLE | 1


  
## 6 Functions

### 6.1 ayFX_Setup

<table>
<tr><th colspan=3 align="left">ayFX_Setup</th></tr>
<tr><td colspan=3>ayFX player setup</td></tr>
<tr><th>Function</th><td colspan=2>ayFX_Setup(unsigned int BANKaddr, char mode, char channel)</td></tr>
<tr><th rowspan=3>Input</th><td>[unsigned int]</td><td>Samples Bank memory address</td></tr>
<tr><td>[char]</td><td>mode (0=Fixed channel;0=Cycle channel)</td></tr>
<tr><td>[char]</td><td>Channel number (0=A;1=B;2=C)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
<tr><th>Example:</th><td colspan=2><pre>ayFX_Setup((unsigned int) BANK00, ayFX_FIXED ,AY_Channel_C);</pre></td></tr>
</table>


### 6.2 ayFX_SetChannel

<table>
<tr><th colspan=3 align="left">ayFX_SetChannel</th></tr>
<tr><td colspan=3>Change output channel (useful only in Fixed channel mode).</td></tr>
<tr><th>Function</th><td colspan=2>ayFX_SetChannel(char channel)</td></tr>
<tr><th>Input</th><td>[char]</td><td>Channel number (0=A;1=B;2=C)</td></tr>
<tr><th>Output</th><td colspan=2> --- </td></tr>
<tr><th>Example:</th><td colspan=2><pre>VPRINTN("Alea iacta est",10);</pre></td></tr>
</table>

### 6.3 ayFX_Play

<table>
<tr><th colspan=3 align="left">ayFX_Play</th></tr>
<tr><td colspan=3>Play FX</td></tr>
<tr><th>Function</th><td colspan=2>ayFX_Play(char FXnumber, char FXpriority)</td></tr>
<tr><th rowspan=2>Input</th><td>[char]</td><td>FX to be played (0 to 255)</td></tr>
<tr><td>[char]</td><td>FX priority (0 to 15)</td></tr>
<tr><th>Output</th><td>[char]</td><td>0: no errors, the new sample has been correctly initialized<br/>
1: priority error, there's a sample with higher priority being played<br/>
2: index error, the current bank don't have a sample with such index</td></tr>
<tr><th>Example:</th><td colspan=2><pre>ayFX_Play(5,0);</pre></td></tr>
</table>


### 6.4 ayFX_Decode

<table>
<tr><th colspan=2 align="left">ayFX_Decode</th></tr>
<tr><td colspan="2">Generates the values of registers of a frame of the ayFX in progress.<br/>It has to be executed in every frame, not necessarily in the interrupt routine.</td></tr>
<tr><th>Function</th><td>ayFX_Decode()</td></tr>
<tr><th>Input</th><td> --- </td></tr>
<tr><th>Output</th><td> --- </td></tr>
<tr><th>Examples:</th><td><pre>ayFX_Decode();</pre></td></tr>
</table>


<br/>

---


## 7 How to use

#### coming soon...


<br/>

---

![Creative Commons License](https://i.creativecommons.org/l/by-nc/4.0/88x31.png) 
<br/>This document is licensed under a [Creative Commons Attribution-NonCommercial 4.0 International License](http://creativecommons.org/licenses/by-nc/4.0/).