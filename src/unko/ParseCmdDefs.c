/**
 * @file ParseCmdDefs.c
 */
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <bolib.h>
#include "common/puts.h"
#include "common/Funex.h"
typedef struct _RomFile RomFile; /* for Asarctl.h */
#include "unko/Asarctl.h"
#include "unko/ParseCmdDefs.h"

static Define* MakeDefine(const char* srcname, const char* srcval)
{
	size_t len;
	size_t i;
	size_t st;
	Define* def = NULL;
	char* name = NULL;
	char* val = NULL;

	if((NULL == srcname) || (NULL == srcval))
	{
		return NULL;
	}

	len = strlen(srcname);
	i = 0;

	SkipSpaces(srcname, &i, len);
	st = i;
	for(i=len; 0<i; i--)
	{
		if(false == IsSpace(srcname[i]))
		{
			break;
		}
	}
	if(i == st) return NULL;

	len = i - st;

	name = calloc(len+1, sizeof(char));
	val = calloc(strlen(srcname)+1, sizeof(char));
	def = calloc(1, sizeof(Define));

	if(NULL == def || NULL == val || NULL == name)
	{
		free(def);
		free(val);
		free(name);
		return NULL;
	}

	strncpy_s(name, len+1, &srcname[st], len);
	strcpy_s(val, strlen(srcname)+1, srcval);

	def->name = name;
	def->val = val;

	return def;
}

void* CloneDefine(const void* srcv)
{
	Define* src;
	Define* clone;
	char* name;
	char* val;

	if(NULL == srcv) return NULL;

	src = (Define*)srcv;

	name = Str_copy(src->name);
	val = Str_copy(src->val);
	clone = calloc(1, sizeof(Define));

	if((NULL == name) || (NULL == val) || (NULL == clone))
	{
		free(name);
		free(val);
		free(clone);
		return NULL;
	}
	strcpy_s(name, strlen(src->name)+1, src->name);
	strcpy_s(val, strlen(src->val)+1, src->val);
	clone->name = name;
	clone->val = val;

	return clone;
}

void DelDefine(void* tgt)
{
	Define* def;
	if(NULL == tgt) return;

	def = (Define*)tgt;
	free(def->name);
	free(def->val);
	free(def);
}


enum {
	Match_Empty = 0,
	Match_IntegerDefine,
	Match_ParamDefine,
	Unmatch
};
static bool MatchEmpty(const char* cmd, void* p)
{
	size_t i;
	size_t len;

	len = strlen(cmd);

	for(i=0; i<len; i++)
	{
		if(false == IsSpace(cmd[i]))
		{
			return false;
		}
	}
	if(i != len) return false;

	return true;
}
static bool IntegerInjection(const char* cmd)
{
	size_t i;
	size_t len;

	len = strlen(cmd);
	i = 0;

	/* check hex */
	if('$' == cmd[0])
	{
		i++;
		for(; i<len; i++)
		{
			if(false == ishex(cmd[i]))
			{
				break;
			}
		}

		if(i != len)
		{
			return false;
		}

		putdebug("Match Hex value: %s", cmd);
		return true;
	}

	/* check integer */
	if('-' == cmd[i]) i++;
	for(; i<len; i++)
	{
		if(false == isdigit(cmd[i]))
		{
			break;
		}
	}
	if(i != len)
	{
		return false;
	}

	putdebug("Match Int value: %s", cmd);
	return true;
}
static bool MatchParam_Shared(const char* cmd, void* defv, bool (*injection)(const char*))
{
	size_t i;
	size_t len;
	size_t st;
	char* name;
	size_t namelen;
	char* val;
	size_t vallen;
	Define *def = (Define*)defv;

	len = strlen(cmd);
	i = 0;

	SkipSpaces(cmd, &i, len);
	st = i;

	SkipUntilChar(cmd, &i, '=', len);
	if(i == len) return false;

	namelen = i - st;
	name = calloc((size_t)namelen+1, sizeof(char));
	if(NULL == name)
	{
		putfatal("%s: memory error.", __func__);
		return false;
	}
	strncpy_s(name, namelen+1, &cmd[st], namelen);

	/* '=' */
	i++;

	SkipSpaces(cmd, &i, len);
	if(i == len)
	{
		free(name);
		return false;
	}
	st = i;

	/* search value */
	SkipUntilSpaces(cmd, &i, len);
	if(i != len)
	{
		free(name);
		return false;
	}

	/* get define value */
	vallen = i - st;
	val = calloc(vallen+1, sizeof(char));
	if(NULL == val)
	{
		putfatal("%s: memory error.", __func__);
		free(name);
		return false;
	}
	strncpy_s(val, vallen+1, &cmd[st], vallen);

	/* shirk space */
	CutOffTailSpaces(val);

	vallen = strlen(val);

	/* check integer injection */
	if(NULL == injection)
	{
		char* newval = NULL;

		vallen += 2;
		newval = calloc((size_t)vallen+1, sizeof(char));
		if(NULL == newval)
		{
			putfatal("%s: memory error.", __func__);
			free(name);
			free(val);
			return false;
		}
		newval[0] = '"';
		strcpy_s(&newval[1], vallen-1, val);
		newval[strlen(val)+1] = '"';
		free(val);
		val = newval;
	}
	else
	{
		if(false == injection(val))
		{
			free(name);
			free(val);
			return false;
		}
	}

	/* shirk space */
	CutOffTailSpaces(name);

	def->name = name;
	def->val = val;

	return true;
}
static bool MatchIntegerDefine(const char* cmd, void* defv)
{
	return MatchParam_Shared(cmd, defv, IntegerInjection);
}
static bool MatchParamDefine(const char* cmd, void* defv)
{
	return MatchParam_Shared(cmd, defv, NULL);
}
static bool SearchDefine(const void* s1, const void* s2)
{
	const char* name = (const char*)s1;
	Define *d = (Define*)s2;

	return(0 == strcmp(name, d->name));
}
bool ParseCmdDefs(void* dst, const char* cmdline)
{
	List* defineList;
	Define def;
	Define* d;
	Iterator* lnode;

	FunexStruct funex[] = {
		{ MatchEmpty, NULL },
		{ MatchIntegerDefine, &def },
		{ MatchParamDefine, &def },
		{ NULL, NULL }
	};

	if(NULL == dst)
	{
		putfatal("%s: Program logic error: dst is NULL", __func__);
		return false;
	}

	assert(NULL != dst);

	defineList = *((List**)dst);

	/* parse define */
	switch(FunexMatch(cmdline, funex))
	{
		case Match_Empty:
			return false;

		case Match_IntegerDefine:
		case Match_ParamDefine:
			putdebug("Match param define: !%s = %s", def.name, def.val);

			d = malloc(sizeof(Define));
			if(NULL == d)
			{
				putdebug("%s : memory error.", __func__);
				return false;
			}
			memcpy(d, &def, sizeof(Define));

			lnode = defineList->search(defineList, d->name, SearchDefine);
			if(NULL != lnode)
			{
				Define* p = lnode->data(lnode);
				putdebug("update %s %s -> %s", d->name, p->val, d->val);
				free(p->val);
				p->val = d->val;
				break;
			}
			defineList->push(defineList, d);
			break;

		default:
			putdebug("Match non-param define");
			d = MakeDefine(cmdline, "1");
			if(NULL == d)
			{
				putdebug("%s : memory error.", __func__);
				return false;
			}
			lnode = defineList->search(defineList, d->name, SearchDefine);
			if(NULL != lnode)
			{
				putdebug("popoipo~i %s", d->name);
				free(d);
				break;
			}
			putdebug("defined: !%s = 1", d->name);
			defineList->push(defineList, d);
			break;
	}
	return true;
}
