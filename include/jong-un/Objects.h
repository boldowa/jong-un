#pragma once
/**********************************************************
 *
 * Objects is responsible for ...
 *
 **********************************************************/

typedef struct ObjectProperty {
	bool	visible;	/* Visible */
	int	xsize;		/* XSize */
	int	ysize;		/* YSize */
	int	horzea;		/* HorzElongationAmount */
	int	vertea;		/* VertElongationAmount */
} ObjectProperty;

bool InsertObjects(RomFile*, const char*, const uint32, const InsertListStruct*, const InsertListGroupStruct*, List*, int*, List*);

