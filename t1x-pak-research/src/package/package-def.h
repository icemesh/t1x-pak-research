/******************************************************************************/
/*
  Author  - icemesh
*/
/******************************************************************************/
#pragma once

#include <stdint.h>

struct PakHeader //size 0x3C
{
	uint32_t m_magic;					//0x0 0xA7D
	uint32_t m_hdrSize;					//0x4 header size
	uint32_t m_pakLoginTableIdx;		//0x8 idx of the page storing the PakLoginTable
	uint32_t m_pakLoginTableOffset;		//0xC relative offset PakLoginTable = PakPageHeader + m_pakLoginTableOffset; //its a ResItem 
	uint32_t m_pageCt;					//0x10 page count. Total number of pages in the package
	uint32_t m_pakPageEntryTableOffset;	//0x14 ptr to the PakPageEntry array/table
	uint32_t m_numPointerFixUpPages;	//0x18 always 0x8
	uint32_t m_pointerFixUpTableOffset; //0x1C ptr to the PointerFixUpTable table
	uint32_t m_dataSize;				//0x20 size of the data from the end of the last page to the EOF
	uint32_t m_unk;						//0x24 no idea
	uint32_t m_unk2;					//0x28 no idea
	uint32_t m_unk3;					//0x2C no idea
	uint32_t m_unk4;					//0x30 no idea
	uint32_t m_unk5;					//0x34 no idea
	uint32_t m_unk6;					//0x38 no idea
};

//ok
struct PakPageEntry //0xC 
{
	uint32_t m_resPageOffset;						//0x0 absolute offset to the ResPage
	uint32_t m_pageSize;							//0x4 page size
	uint32_t m_owningPackageIndex;					//0x8 
};

struct ResPage;

/*
//TODO IGNORE THIS for the moment its probably wrong or UNUSED
struct ResPage //0x40
{
	const char* m_name;								//0x0  always null ?
	uint64_t m_unused;								//0x8  0x0001234500012345	
	uint32_t m_pageSize;							//0x10 size from the end of the PakPageHeader
	uint32_t m_unused2;								//0x14 0x00012345
	uint64_t m_unk;									//0x18 always 0x0?
	uint64_t m_itemNameId;	//0x20
	uint64_t m_unused3;								//0x28 always 0x0001234500012345
	uint32_t m_numPageHeaderEntries;				//0x30 number of pointers in the table
	uint32_t m_unkNumber;
	uint64_t m_unk1;
};
*/

struct ResPageEntry //0x10 
{
	uint64_t m_name;								//0x0 same as the ResItem name... This field is usually used by animations
	uint32_t m_resItemOffset;						//0x8 relative offset. -> ResItem = PakPageHeader + m_pakLoginItemOffset
	uint32_t m_padding;								//0xC always 00 01 23 45
};

//ok
struct ResItem //0x30
{
	void* m_unkPtr;								//0x0 
	uint64_t m_unused;							//0x8 always 0x0001234500012345
	uint32_t m_pageSize;						//0x10 size from the end of the PakPageHeader
	uint32_t m_unused2;							//0x14 0x00012345
	uint64_t m_unk;								//0x18 always 0x0?
	uint64_t m_itemNameId;						//0x20 StringId64 of the item name
	uint64_t m_unused3;							//0x28 always 0x0001234500012345
};

// ok
struct PointerFixUpPage //0xC
{
	uint32_t m_unk;									// 0x0 always 0 ?
	uint32_t m_dataOffset;							// 0x4 ptr to the table
	uint32_t m_numLoginPageEntries;					// 0x8 number of login pages entries
};

//ok
struct PointerFixUpPageEntry //0x8
{
	uint16_t m_page1Idx;							// 0x0 idx in the page table
	uint16_t m_page2Idx;							// 0x2 idx in the page table
	uint32_t m_offset;								// 0x4 offset
};

//
struct PakLoginTableEntry //0x10
{
	uint32_t m_unk;									// 0x0
	uint32_t m_unk0;								// 0x4
	uint32_t m_wantedPage;							// 0x8 page idx in the table
	uint32_t m_resItemOffset;						// 0xC offset of the res item
};