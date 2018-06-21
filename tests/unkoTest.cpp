/**
 * @file unkoTest.cpp
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <bolib.h>
#include <bolib/file/RomFile.h>
extern "C"
{
#include "common/puts.h"
int Unko(int, char**);
}


#include "CppUTest/TestHarness.h"

TEST_GROUP(unko)
{
	char* romPath = (char*)"testdata/test.smc"; 
	RomFile* rom;

	void fw(size_t ad, const void* data, size_t len, FILE* f)
	{
		fseek(f, ad, SEEK_SET);
		fwrite(data, 1, len, f);
	}

	FILE* baseGen()
	{
		FILE *f = fopen(romPath, "wb");

		/* expand rom */
		fw(1024*1024, "", 1, f);
		fw(0x8000-0x2b, "\x20\x02\x0a", 3, f);

		/* write checksum */
		fw(0x8000-0x24, "\x00\x00\xff\xff", 4, f);
		return f;
	}

	void Smw(FILE* f)
	{
		/* for smw rom detection */
		fw(0x6f, "\x58\xe6\x13\x20\x22\x93", 6, f);
		fw(0x1329, "\x91\x93\x0f\x94\x6f\x9f\xae\x96", 8, f);
	}

	void rw(size_t ad, int len, FILE* f)
	{
		fseek(f, ad+0x7b, SEEK_SET);
		for(int i=0; i<len; i++)
		{
			fwrite("\xe3\xb3\x0d", 1, 3, f);
		}
	}

	void LM(FILE* f)
	{
		const char LMSig[] = "Lunar Magic Version 1.81";
		const size_t LMSigLen = strlen(LMSig);
		fw(0x07f0a0, LMSig, LMSigLen, f);

		/* executeptr */
		fw(0x06a452, "\xfa\x86\x00", 3, f);
		fw(0x06c197, "\xfa\x86\x00", 3, f);
		fw(0x06cd97, "\xfa\x86\x00", 3, f);
		fw(0x06d997, "\xfa\x86\x00", 3, f);
		fw(0x06e897, "\xfa\x86\x00", 3, f);
		fw(0x06a10c, "\xfa\x86\x00", 3, f);

		/* object pointer */
		rw(0x06a452,  7, f);
		rw(0x06c197, 11, f);
		rw(0x06cd97,  9, f);
		rw(0x06d997, 11, f);
		rw(0x06e897,  5, f);

		/* exobj pointer */
		fseek(f, 0x06a2d7, SEEK_SET);
		for(int i=0; i<104; i++)
		{
			fwrite("\xd1\xa6\x0d", 1, 3, f);
		}
	}

	void genEmpty()
	{
		FILE* f;
		f = baseGen();
		fclose(f);
	}

	void genSmw()
	{
		FILE* f;
		f = baseGen();
		Smw(f);
		fclose(f);
	}

	void genLM()
	{
		FILE* f;
		f = baseGen();
		Smw(f);
		LM(f);
		fclose(f);
	}

	void setup()
	{
	}

	void teardown()
	{
		remove(romPath);
	}
};

TEST(unko, install_succ)
{
	char* argv[6] = {
		(char*)"./unko",
		romPath
	};

	/* smw, LM ok, install */
	genLM();
	argv[2] = (char*)"-l";
	argv[3] = (char*)"./testdata/unko/empty.txt";
	LONGS_EQUAL(0, Unko(4, argv));

	/* update */
	argv[2] = (char*)"-l";
	argv[3] = (char*)"./testdata/unko/min.txt";
	argv[4] = (char*)"-D";
	argv[5] = (char*)"UNKO";
	LONGS_EQUAL(0, Unko(6, argv));

	/* Uninstall */
	argv[2] = (char*)"-u";
	LONGS_EQUAL(0, Unko(3, argv));
}

TEST(unko, install_fail)
{
	char* argv[2] = {
		(char*)"./unko",
		romPath
	};

	/* no args */
	LONGS_EQUAL(0, Unko(1, argv));

	/* invalid rom */
	genEmpty();
	LONGS_EQUAL(-1, Unko(2, argv));

	/* smw. but LM isn't installed */
	genSmw();
	LONGS_EQUAL(-1, Unko(2, argv));

}

TEST(unko, ver_help)
{
	char* argv[3] = {
		(char*)"./unko",
		(char*)"-?",
		(char*)"--version"
	};

	/* check no error */
	LONGS_EQUAL(0, Unko(3, argv));
}

TEST(unko, uninstall)
{
	/* nothing to do */
}

