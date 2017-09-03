#pragma once

/**
 * Application Name
 */
#define _AppName	UNKO
#define AppName		"UNKO"

/**
 * Version
 */
#define AppVersion		1.30
#define ResVersion		1,3,0,0
#define FileVersion		7

/**
 * UNKO main asm version
 *   You have to check these sources if you change it.
 *     - src/unko.c
 *         ... UninstallUnko() function
 *             It detects action for uninstall.
 *     - sys/unko.asm
 *         ... It's necesary to match the version info
 *             of this one.
 */
#define CodeVersion		0x0110

