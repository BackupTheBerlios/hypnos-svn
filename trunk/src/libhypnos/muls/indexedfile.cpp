/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
| Copyright (c) 2004 - Hypnos Project                                      |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/

#include "libhypnos/muls/indexedfile.h"
#include "libhypnos/exceptions.h"

namespace nLibhypnos {
namespace nMULFiles {
	
	template<class cData> tplIndexFile<cData>::tplIndexFile(std::string filename)
		: tplMMappedFile(filename)
	{
	}
	
	/*!
	\brief Gets the offset of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The offset of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	uint32_t template<class cData> tplIndexFile<cData>::getLookup(uint16_t index) const
	{
		if ( index >= getCount() )
			throw eOutOfBound(getCount()-1, index);
		
		return array[index].getLookup();
	}
	
	/*!
	\brief Gets the length of the given record in the indexed file
	\param index ID of the record (index to search for). Zero-based.
	\return The length of the requested record in the indexed file
	\throw eOutOfBound If the requested index is greater than the count of records
	*/
	uint32_t template<class cData> tplIndexFile<cData>::getSize(uint16_t index) const
	{
		if ( index >= getCount() )
			throw eOutOfBound(getCount()-1, index);
		
		return array[index].getSize();
	}
	
} }
