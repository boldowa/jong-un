/**
 * Objects.h
 */
#pragma once

#define NrmObjEmpty		0x0db3e3

typedef struct ObjectProperty {
	bool	visible;	/* Visible */
	int	xsize;		/* XSize */
	int	ysize;		/* YSize */
	int	horzea;		/* HorzElongationAmount */
	int	vertea;		/* VertElongationAmount */
} ObjectProperty;

bool InsertObjects(RomFile*, const char*, const uint32, const InsertListStruct*, List*, int*, List*);
bool UninstallObjects(RomFile*, const uint32);

