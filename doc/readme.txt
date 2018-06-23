+----------------------------------------------------------+
| UNKO - UNKnown Object  (Object Insertion Tool)           |
| ----------------------------------------------           |
|              ... ~~he is very fond of standing out~~ ... |
|                                                          |
|   - ver. 1.31                                            |
|                                                          |
|   Repository : https://github.com/boldowa/unko/          |
|   Web        : https://boldowa.github.io/unko/           |
|                                                          |
|   Changelog)                                             |
|                                                          |
|     05-17-2017 [v0.60] : test ver.                       |
|                                                          |
|     05-19-2017 [v1.00] : tool release                    |
|                                                          |
|     05-20-2017 [v1.10] : added define option,            |
|                          object property syntax          |
|                          and some issue fix              |
|                                                          |
|     05-21-2017 [v1.11] : It can now be compiled          |
|                          with MSVC.                      |
|                          I checked it with MSVC 9.0.     |
|                                                          |
|     06-05-2017 [v1.20] : Change program internal design  |
|                          and some mammap issue fix       |
|                                                          |
|     08-06-2017 [v1.21] : (RPG Hacker)                    |
|                          + Updated to newest Asar        |
|                            version                       |
|                          + Fixed a crash when trying to  |
|                            set a define with Asar        |
|                          + Fixed SA-1 support, should    |
|                            now actually work             |
|                          + Other minor code fixes        |
|                                                          |
|     08-31-2017 [v1.22] : Fixed some bugs.                |
|                            Github issue : #2,  #3,  #4,  |
|                                           #5,  #6,  #10  |
|                                                          |
|     09-02-2017 [v1.30] : Improved library function       |
|                          and add object variable         |
|                            Github issue : #9             |
|       (same)   [v1.31] : Fixed issue which abnormally    |
|                          exits when label name can't     |
|                          be resolved.                    |
|                            Github issue : #12            |
|                                                          |
+----------------------------------------------------------+

[What is this?]
  This tool is the object customize tool for Super Mario World.
  This tool requires LunarMagic 1.81 or higher.

  This tool supports these rom map type:
    - LoRom
    - HiRom (?)
    - ExLoRom
    - ExHiRom (?)
    - SA-1Rom (probably).


[Supported system]
  - Windows
  - Linux


[Folder structure]
  |
  +-- sys                              ... System file directory.
  |    |
  |    +-- unko.asm                    ... Unko's init patch. 
  |    |                                   This patch will apply to rom automatically.
  |    |
  |    +-- smwlibs.asm                 ... smw jsl libraries declaration file.
  |
  +-- libraries                        ... Libraries directory.
  |    |                                   If you put any asm in this foler,
  |    +-- commonlib.asm                   it will install automatically.
  |    +-- foolib.asm
  |    +-- barlib.asm
  |           :
  |
  +-- objects                          ... Objects directory.
  |    |                                   Probably, most simple asm is "CementWall.asm", I think...
  |    +-- swblk
  |    |     |
  |    |     +-- InvYellowBlock.asm
  |    |     +-- InvGreenBlock.asm
  |    |     +-- InvBlueBlock.asm
  |    |     +-- InvRedBlock.asm
  |    |
  |    +-- ExMidwaySpawn.asm
  |    +-- CementGate.asm
  |    +-- CementWall.asm
  |            :
  |
  +-- option
  |    |
  |    +-- ...                         ... option patches.
  |                                        these patches are stand-alone asar patch.
  |
  +-- asar_licenses                    ... Asar's license files.
  |
  +-- list.txt                         ... Object list.
  +-- libasar.so or asar.dll           ... library for asm patch, asar v1.5.0
  +-- unko.exe                         ... main program. (compiled with Visual Studio 2008)
  +-- unko_mingw.exe                   ... compiled with MinGW-w64(i686).
  +-- readme.txt                       ... It's file that just you are reading.
  +-- LICENSE                          ... license file
  |
  +-- sources.tar.gz                   ... Unko source files


[Command-line option]
Usage: unko [options] <rom>
***
  -l           <file name>     ... specify the object list file name.
  --list       <file name>         default is "list.txt".
***
  -L           <file name>     ... specify the libraries directory name.
  --library    <file name>         default is "libraries".
***
  -o           <file name>     ... specify the objects directory name.
  --object     <file name>         default is "objects".
***
  -D           (var)=(value)   ... add define for objects.
  --define     (var)               but, Asar doesn't have syntax like "ifndef" (maybe).
                                   so, it can't be used effectively.
***
  -u                           ... uninstall all of data about Unko from rom.
  --uninstall
***
  -d                           ... enable to output debug info.
  --debug
***
  -v                           ... show program version info.
  --version
***
  -?                           ... show program usage.
  --help
***


[How to use object in LunarMagic]
  1) Open LunarMagic, and set Layer1(or Layer2) editing mode.

  2) Menu [Edit] -> [Insert manual...]
     or push "insert" key.

  3) Input object info

     If you want to ExtendObject, set parameter in this way.
       [Command] = 00
       [Size/Type/Ext] = <number of object that you want use>

     If you want to use 2DObject, set parameter in this way.
       [Command] = 2D
       [Extension] = NNxx
                     *** NN = number of object that you want use.
                         xx = extended information.


[List file syntax]
+------------------------------------------------+
|//----------                                    |
|[Normal]                                        |
|29  white_switch_blk.asm                        |
|2a  super_note_blk.asm                          |
|      :                                         |
|      :                                         |
+------------------------------------------------+
  - comment
    "//"  ... You can use line comment.

  - insert group
    "[Normal]"
    "[Castle]"
    "[Rope]"
    "[Underground]"
    "[GhostHouse]"
    "[ExtendObject]"
    "[Object2D]"

  - list
    "xx  ssss.asm"   ... hex number(1byte) + some spaces(or tabs) + file name


[Prohibited matter]

    This file name is prohibited from setting.
      "temp.asm"


[ASM format]

  Unko require "main" label.
  So you must define "main" label in object asm file.

  example:
    --- [ ASM start ] ---
    
    ; this area is out of free code.
    ; so, you can check prerequisites here.

    assert read24($0d90ab) != $00ddcc

    ; It'll be useful for setting tile number.
    org $123456
        db    $25, $01


    ; It'll be starts free code from here.
    main:
        ldy.b   $57
        lda.b   #$30
	jsl     Store1to6E
	rtl

    --- [ ASM end   ] ---


[Share code feature]
  2 ways.

    1) use libraries...
        The ASM in the libraries folder will insert as a shared code.

    2) use export feature...
        You can export any labels in object ASM code.

	<example>
	    lda.b	#$00
	    sta.b	[$68]
	  > print	"export ShareCode = $",pc
	    sep		#$30
	           :
        In this case, "ShareCode" label will export.
	"$" may or may not be present. (allow : print "export Share = ", pc)


[SMW libraries]
  SMW libraries are declarated in sys/smwlibs.asm.
  You can use it in this way.
    jsl  SMW_KillMario     ; jsl library
    lda  SMW_IndexToBit,x  ; table


[MapMode]
  Unko detects mapmode automatically.
  if you want to get mapmode in asm, you can get rom type from "!map" define.
    case
      0 ... Unknown
      1 ... LoRom
      2 ... HiRom
      3 ... SA1Rom
      4 ... ExLoRom
      5 ... ExHiRom

[Object valiable]
  You can these variables in object asm.

    - !object_group  - Group number of the object currently being asssembled.
    - !object_number - Object number of    - same -

    The group number means
      0 ... Normal
      1 ... Castle
      2 ... Rope
      3 ... Underground
      4 ... GhostHouse
      5 ... ExtendObject
      6 ... Object2D


[Search path]
  Unko searches list file and library sources and object sources in
    (1) Current directory     +  .. Search rank High
    (2) ROM file directory    |
    (3) unko exe directory    +  .. Search rank Low

  If file names conflict, 
    (objects list) Insert one with the highest search rank.
    (objects)      Insert one with the highest search rank.
    (libraries)    Insert what is being used from objects.


[Others]
  Unko can get the object's property from print code.
  This featur was created assuming extension of Lunar Magic.
  Currently you can't make effective use.
  Currently it's possible to check in debug outputs.

  +----- Property list -----+
  Visible                   ... bool  [default: true ]
  XSize                     ... int   [default:  1   ]
  YSize                     ... int   [default:  1   ]
  HorzElongationAmount      ... int   [default:  1   ]
  VertElongationAmount      ... int   [default:  1   ]


[Unko program library]
  Unko uses this library.

    - Asar ...   (C)Alcaro
                    License: LGPLv3

[Unko program License]
  MIT License.


[Thanks]
    - Alcaro                : He provided we a useful library.

    - 1024(0x400)           : Original object tool patch creator

    - Pingu                 : UnOfficial object tool creator. some object based on his code.

    - imamelia              : I referenced 2D object codes (from v0.4).

    - RPGHacker             : He fixed some minor bugs.


