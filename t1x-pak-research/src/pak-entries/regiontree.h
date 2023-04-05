#pragma once

#include <cinttypes>

struct Region;
struct QuadList;

struct RegionTree 
{
	void*		m_unkPtr;	///< <c>0x00</c>:
	Region*		m_aRegion[0];	///< <c>0x08</c>: 
};

struct Region
{
	const char*		m_regionName;		///< <c>0x00</c>: region name
	uint64_t		m_regionId;			///< <c>0x08</c>: StringId64 of the reigon name
	void*			field_10;			///< <c>0x10</c>:
	uint64_t		field_18;			///< <c>0x18</c>:
	const char*		m_unkRegionName;	///< <c>0x20</c>: parent region maybe 
	uint64_t		m_regionId2;		///< <c>0x28</c>: StringId64 of the parent region ?
	QuadList*		m_pQuadList;		///< <c>0x30</c>:
	void*			field_38;			///< <c>0x38</c>:
	void*			field_40;			///< <c>0x40</c>:
	uint64_t		field_48;			///< <c>0x48</c>:
	void*			field_50;			///< <c>0x50</c>:
	void*			field_58;			///< <c>0x58</c>:
	uint64_t		field_60;			///< <c>0x60</c>:
	uint32_t		field_64;			///< <c>0x64</c>:
	uint32_t		field_68;			///< <c>0x68</c>:
	uint16_t		m_flags;			///< <c>0x20</c>:
	uint32_t		field_72;			///< <c>0x20</c>:
	char			field_76;			///< <c>0x20</c>:
	char			field_77;			///< <c>0x20</c>:
	uint64_t		field_78;			///< <c>0x20</c>:
	uint64_t		field_80;			///< <c>0x20</c>:
	uint64_t		field_88;			///< <c>0x20</c>:
	uint64_t		field_90;			///< <c>0x20</c>:
	uint64_t		field_98;			///< <c>0x20</c>:
	uint64_t		field_A0;			///< <c>0x20</c>:
	uint64_t		field_A8;			///< <c>0x20</c>:
	uint64_t		field_B0;			///< <c>0x20</c>:
	uint64_t		field_B8;			///< <c>0x20</c>:
};


struct QuadList
{
	uint64_t	field_0;	///< <c>0x00</c>:
	uint64_t	field_8;	///< <c>0x08</c>:
	uint16_t	field_10;	///< <c>0x10</c>:
	uint8_t		m_count;	///< <c>0x12</c>:
	uint8_t		m_flag;		///< <c>0x13</c>: flag |= (1 << iter);
	uint32_t	field_14;	///< <c>0x14</c>:
	uint32_t	field_18;	///< <c>0x18</c>:
	uint32_t	field_1C;	///< <c>0x1C</c>:
};

struct Quad //0x70
{

};
