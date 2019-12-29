#pragma once

#include "itzam\itzam.h"


class itzamDatabase
{
protected:
	itzam_btree itzambt;
	bool bOpened{ false };
public:

	itzamDatabase();
	~itzamDatabase();

	bool open(const char* path, itzam_key_comparator* comparator);
	template<typename record, uint16_t order = 16>
	bool create(const char* path, itzam_key_comparator* comparator);
	template<typename record, uint16_t order = 16>
	bool openOrCreate(const char* path, itzam_key_comparator* comparator);
	bool close(void);
};

