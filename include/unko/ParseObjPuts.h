#pragma once
/**
 * ParseObjPuts.h
 */

bool ParseObjPuts(
		const char* str,
		void* data,
		const char* searchStr,
		bool (*valueAnalyzer)(const char*, char*, void*)
		);

bool MatchExportValue(const char* str, char* declaration, void* data);
bool MatchBooleanValue(const char* str, char* declaration, void* data);
bool MatchIntegerValue(const char* str, char* declaration, void* data);
