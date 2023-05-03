/******************************************************************************/
/*
  Author  - icemesh
*/
/******************************************************************************/
#include "package.h"
#include "../utils/utils.h"
#include "package-def.h"
#include "../utils/stringid.h"
#include "../pak-entries/geometry1.h"
#include "../pak-entries/spawner-group.h"

#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>

uintptr_t g_fileBase = 0;

const char* ResolverPakItemName(StringId64 itemId);

static inline uint32_t ReadU32(void const* pMem)
{
#ifdef WIN32
	//using immintrinsics
	return _load_be_u32(pMem);
#else 
	return *reinterpret_cast<const uint32_t*>(pMem);
#endif
}

static inline uint32_t ReadU16(void const* pMem)
{
#ifdef WIN32
	//using immintrinsics
	return _load_be_u16(pMem);
#else 
	return *reinterpret_cast<const uint16_t*>(pMem);
#endif
}

Package::Package(const char* pakName)
{
	m_pLoadedFile = nullptr;
	m_currentPageIdx = 0;
	m_status = PackageStatus::kPackageStatusEmpty;
	m_textureBaseOffset = 0;
	FILE* fh;
	fopen_s(&fh, pakName, "rb");
	if (fh)
	{
		fseek(fh, 0x0, SEEK_END);
		size_t fsize = ftell(fh);
		m_fileSize = fsize;
		fseek(fh, 0x0, SEEK_SET);
		void* pFile = malloc(fsize);
		if (pFile)
		{
			fread(pFile, fsize, 0x1, fh);
			fclose(fh);
			m_pLoadedFile = pFile;
			g_fileBase = reinterpret_cast<uintptr_t>(pFile);
			snprintf(m_realFileName, 128, "%s", pakName);
		}
		else
		{
			MsgErr("Failed to allocate file");
			fclose(fh);
		}
	}
	else
	{
		MsgErr("Could not open %s! - Does it even exist?\n", pakName);
	}
}

Package::~Package()
{
	if (m_pLoadedFile != nullptr)
	{
		free(m_pLoadedFile);
	}
}

int Package::PackageLogin()
{
	//m_pageSize <= 512 * 1024;
	PakHeader* pHdr = reinterpret_cast<PakHeader*>(m_pLoadedFile);
	if (pHdr)
	{
		m_status = PackageStatus::kPackageStatusLoadingPakHeader;
		while (1)
		{
			switch (m_status)
			{
				case PackageStatus::kPackageStatusLoadingPakHeader:
				{
					uint32_t magic = ReadU32(&pHdr->m_magic);
					if (magic == 0xA7D || magic == 0x10A79) // normal or -dict.pak
					{
						if (ReadU32(&pHdr->m_pageCt) < LoadingHeap::kBigPs3MaxPages)
						{
							m_hdr.m_magic = magic;
							m_hdr.m_pageCt = ReadU32(&pHdr->m_pageCt); //m_loadingHeapNumPages
							m_hdr.m_hdrSize = ReadU32(&pHdr->m_hdrSize);
							m_hdr.m_pakLoginTableIdx = ReadU32(&pHdr->m_pakLoginTableIdx);
							m_hdr.m_pakLoginTableOffset = ReadU32(&pHdr->m_pakLoginTableOffset);
							m_hdr.m_numPointerFixUpPages = ReadU32(&pHdr->m_numPointerFixUpPages);
							m_hdr.m_pointerFixUpTableOffset = ReadU32(&pHdr->m_pointerFixUpTableOffset);
							PakPageEntry* pCurrentPage = reinterpret_cast<PakPageEntry*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + pHdr->m_pakPageEntryTableOffset);
							for (uint32_t i = 0; i < m_hdr.m_pageCt; i++)
							{
								m_pageHdrs[i] = ReadU32(&pCurrentPage->m_resPageOffset);
#ifdef _DEBUG
								printf("m_pageHdrs[%i]: 0x%08X\n", i, m_pageHdrs[i]);
#endif
								pCurrentPage++;
							}
							m_textureBaseOffset = m_fileSize - pHdr->m_dataSize;
							m_status = PackageStatus::kPackageStatusLoadingPages;
						}
						else
						{
							MsgErr("Level [%s] contains too many pages!", m_realFileName);
							return 0;
						}
					}
					else
					{
						MsgErr("Level [%s] is an older version, found 0x%08x instead of 0x%08x!", m_realFileName, magic, 0xA79);
						return 0;
					}
					break;
				}

				case PackageStatus::kPackageStatusLoadingPages:
				{
					/*
					printf("Loading RAM pages for %s\n", m_realFileName);
					PakPageEntry* pCurrentPage = reinterpret_cast<PakPageEntry*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + pHdr->m_pakPageEntryTableOffset);
					pCurrentPage[pHdr->m_pakLoginTableIdx].m_resPageOffset + pHdr->m_pakLoginTableOffset;
					ResItem* pPakLoginTable = reinterpret_cast<ResItem*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + pCurrentPage[pHdr->m_pakLoginTableIdx].m_resPageOffset + pHdr->m_pakLoginTableOffset);
					printf("pPakLoginTable: #%016llX\n", pPakLoginTable->m_itemNameId);
					ResPage* pCurrentPageHdr = reinterpret_cast<ResPage*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + m_pageHdrs[m_currentPageIdx]);
					uint32_t numEntries = pCurrentPageHdr->m_numPageHeaderEntries;
					if (numEntries != 0)
					{
						//start login items
						uint32_t counter = 0;
						ResPageEntry* pPageEntry = reinterpret_cast<ResPageEntry*>(reinterpret_cast<uint8_t*>(pCurrentPageHdr) + 0x40);
						do {
							//ld        r0, 0(r10) 
							//add       r0, r8, r0    
							uintptr_t resPageEntryName = reinterpret_cast<uintptr_t>(swapU64(pPageEntry->m_name) + reinterpret_cast<uint8_t*>(pCurrentPageHdr));
							//stw       r0, 0(r10)
							pPageEntry->m_name = resPageEntryName;
							//lwz       r11, 8(r10)
							//add       r11, r11, r8
							ResItem* pResItem = reinterpret_cast<ResItem*>(ReadU32(&pPageEntry->m_resItemOffset) + reinterpret_cast<uint8_t*>(pCurrentPageHdr));
							//ld        r0, 0(r11) 
							//stw       r0, 0(r11)   
							//const char* resItemName = reinterpret_cast<const char*>(swapU64(pResItem->m_itemNameOffset) + reinterpret_cast<uint8_t*>(pCurrentPageHdr) );
							//pResItem->m_itemNameOffset = reinterpret_cast<uintptr_t>(resItemName);
							//ld        r9, 8(r11)
							//stw       r9, 8(r11) 
							//const char* resItemType = reinterpret_cast<const char*>(swapU64(pResItem->m_itemTypeOffset)+ reinterpret_cast<uint8_t*>(pCurrentPageHdr));
							//pResItem->m_itemTypeOffset = reinterpret_cast<uintptr_t>(resItemType);
							printf("Logged in item #%016llX of type #%.016llX\n", pResItem->m_itemNameId, pResItem->m_unused);
							counter++;
							pPageEntry++;
						} while (counter < numEntries);
					}
					*/
					m_currentPageIdx++;
					if (m_currentPageIdx == m_hdr.m_pageCt)
					{
						FixPointers();
						m_status = PackageStatus::kPackageStatusLoadingVram;
					}
					break;
				}

				case PackageStatus::kPackageStatusLoadingVram:
				{
					m_status = PackageStatus::kPackageStatusDoingLogin;
					break;
				}

				case PackageStatus::kPackageStatusDoingLogin:
				{
					printf("Login for %s\n", m_realFileName);
					uint32_t pakLoginTableOffset = m_hdr.m_pakLoginTableOffset;
					ResItem* pPakLoginTable = reinterpret_cast<ResItem*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + m_pageHdrs[m_hdr.m_pakLoginTableIdx] + pakLoginTableOffset);
					uint8_t* pData = (reinterpret_cast<uint8_t*>(pPakLoginTable) + 0x30); //t2 case
					PakLoginTableEntry* pEntry = nullptr;
					uint32_t max = ReadU32(pData);
#ifdef _DEBUG
					printf("Max: %d\n", max);
#endif // _DEBUG
					uint32_t counter = 0;
					do {
						if (max > counter)
						{
							pEntry = reinterpret_cast<PakLoginTableEntry*>(pData + (counter << 0x3));
							uint32_t pageIdx = ReadU32(&pEntry->m_wantedPage);
							uint32_t itemOffset = ReadU32(&pEntry->m_resItemOffset);
/*
#ifdef _DEBUG
							printf("Page requested %d\nItemOffset 0x%08X\n", pageIdx, itemOffset);
#endif
*/
							ResPage* pPage = reinterpret_cast<ResPage*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + m_pageHdrs[pageIdx]);
							ResItem* pItem = reinterpret_cast<ResItem*>(reinterpret_cast<uint8_t*>(pPage) + itemOffset);
							//printf("%s - %s\n", (const char*)pItem->m_itemNameOffset, (const char*)pItem->m_itemTypeOffset);
							Login(pItem, pPage, this);
							counter++;
						}
					} while (counter != max);
					return 1;
					break;
				}
			} // end switch
		} // end main while
	}
	else
	{
		return 0;
	}
}

void Package::FixPointers()
{
	uint32_t numLoginPages = m_hdr.m_numPointerFixUpPages;
	if (numLoginPages == 0x8)
	{
		PointerFixUpPage* pLoginPage = reinterpret_cast<PointerFixUpPage*>(reinterpret_cast<uint8_t*>(m_pLoadedFile)+ m_hdr.m_pointerFixUpTableOffset);
		uint32_t startOffset = pLoginPage->m_dataOffset;
		uint32_t numTotalEntries = 0;
		//grab the total entries
		for (int i = 0; i < 8; i++)
		{
			numTotalEntries += pLoginPage->m_numLoginPageEntries;
			pLoginPage++;
		}
		//given that the fixup table is contiguos (at least from the packages i've seen)
		//we can simply store the total number of entries and do a simple for loop
		PointerFixUpPageEntry* pEntry = reinterpret_cast<PointerFixUpPageEntry*>(reinterpret_cast<uint8_t*>(m_pLoadedFile) + startOffset);
		for (uint32_t j = 0; j < numTotalEntries; j++)
		{
			//lhz       r9, 0(r7)
			uint16_t page1Idx = pEntry->m_page1Idx;
			//lhz       r11, 2(r7)
			uint16_t page2Idx = pEntry->m_page2Idx;
			//lwz       r10, 4(r7)
			uint32_t unkData = pEntry->m_offset;
			//lwzx      r8, r5, r9
			uint8_t* pPage1 = reinterpret_cast<uint8_t*>(m_pLoadedFile) + m_pageHdrs[page1Idx];
			//lwzx      r9, r5, r11
			uint8_t* pPage2 = reinterpret_cast<uint8_t*>(m_pLoadedFile) + m_pageHdrs[page2Idx];
			//add       r10, r10, r8
			pPage1 += unkData;
			//lwz       r0, 0(r10)
			/*
#ifdef _DEBUG
			printf("tmp: 0x%08X\n", ReadU32(pPage1));
#endif
*/
//add       r0, r0, r9
			pPage2 += ReadU32(pPage1);
			//stw       r0, 0(r10)
			*reinterpret_cast<uintptr_t*>(pPage1) = reinterpret_cast<uintptr_t>(pPage2);
			//addi      r7, r7, 8
			pEntry++;
		}
	}
	else
	{
		puts("Wrong format!");
	}

}

bool Login(ResItem* pResItem, ResPage* pResPage, Package* pPackage)
{
	StringId64 typeId = pResItem->m_itemNameId;
	switch (typeId)
	{
		case SID("GEOMETRY_1"):
		{
			Geometry1::DumpInfo(reinterpret_cast<uint8_t*>(pResItem) + 0x30);
			break;
		}

		case SID("SPAWNER_GROUP"):
		{
			EntitySpawnerGroup::DumpInfo(reinterpret_cast<uint8_t*>(pResItem) + 0x30);
			break;
		}

		default:
		{
			printf("Found #%.16llX -> %s\n", typeId, ResolverPakItemName(typeId));
			break;
		}
	};
	return true;
}

const char* ResolverPakItemName(StringId64 itemId)
{
	switch (itemId)
	{
		case SID("EFFECT_TABLE"):
		{
			return "EFFECT_TABLE";
		}

		case SID("MATERIAL_TABLE_1"):
		{
			return "MATERIAL_TABLE_1";
		}

		case SID("TEXTURE_TABLE"):
		{
			return "TEXTURE_TABLE";
		}

		case SID("TAG_INT"):
		{
			return "TAG_INT";
		}

		case SID("FOREGROUND_SECTION_2"):
		{
			return "FOREGROUND_SECTION_2";
		}

		case SID("TAG_FLOAT"):
		{
			return "TAG_FLOAT";
		}

		case SID("LEVEL_BOUNDING_BOX_DATA"):
		{
			return "LEVEL_BOUNDING_BOX_DATA";
		}

		case SID("GEOMETRY_1"):
		{
			return "GEOMETRY_1";
		}

		case SID("LEVEL_INFO_3"):
		{
			return "LEVEL_INFO_3";
		}

		case SID("SPLINE_TABLE"):
		{
			return "SPLINE_TABLE";
		}

		case SID("REGION_TREE"):
		{
			return "REGION_TREE";
		}

		default:
		{
			return "Unknown";
		}
	}
}
