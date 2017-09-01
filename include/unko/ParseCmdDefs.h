/**
 * ParseCmdDefs.h
 */
#pragma once

bool ParseCmdDefs(void* dst, const char* cmdline);

/**
 * For list create/destroy
 */
void* CloneDefine(const void* srcv);
void DelDefine(void* tgt);

