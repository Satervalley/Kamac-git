#include "itzamDatabase.h"


//----------------------------------------------------------------------------------------------------------------------
itzamDatabase::itzamDatabase()
{
}


//----------------------------------------------------------------------------------------------------------------------
itzamDatabase::~itzamDatabase()
{
	close();
}


//----------------------------------------------------------------------------------------------------------------------
bool itzamDatabase::open(const char* path, itzam_key_comparator * comparator)
{
	close();
	itzam_state state = ::itzam_btree_open(&itzambt, path, comparator, nullptr, false, false);
	return (bOpened = (state == ITZAM_OKAY));
}


//----------------------------------------------------------------------------------------------------------------------
template<typename record, uint16_t order>
bool itzamDatabase::create(const char* path, itzam_key_comparator* comparator)
{
	close();
	itzam_state state = ::itzam_btree_create(&itzambt, path, order, sizeof(record), comparator, nullptr);
	return (bOpened = (state == ITZAM_OKAY));
}


//----------------------------------------------------------------------------------------------------------------------
template<typename record, uint16_t order>
bool itzamDatabase::openOrCreate(const char* path, itzam_key_comparator* comparator)
{
	close();
	bool bRes = open(path, comparator);
	if (!bRes)
		bRes = create<record, order>(path, comparator);
	return bRes;
}


//----------------------------------------------------------------------------------------------------------------------
bool itzamDatabase::close(void)
{
	bool bRes{ false };
	if (bOpened)
	{
		bRes = ::itzam_btree_close(&itzambt) == ITZAM_OKAY;
		bOpened = false;
	}
	return bRes;
}
