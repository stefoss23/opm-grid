//===========================================================================
//
// File: Indexsets.hpp
//
// Created: Fri May 29 23:30:01 2009
//
// Author(s): Atgeirr F Rasmussen <atgeirr@sintef.no>
//            B�rd Skaflestad     <bard.skaflestad@sintef.no>
//
// $Date$
//
// $Revision$
//
//===========================================================================

/*
Copyright 2009, 2010 SINTEF ICT, Applied Mathematics.
Copyright 2009, 2010 Statoil ASA.

This file is part of The Open Porous Media project  (OPM).

OPM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OPM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPM_INDEXSETS_HEADER
#define OPM_INDEXSETS_HEADER

#include <vector>
#include <dune/common/nullptr.hh>
#include <dune/geometry/type.hh>
#include <opm/core/utility/ErrorMacros.hpp>
namespace Dune
{
    namespace cpgrid
    {

	/// @brief
	/// @todo Doc me!
	/// @tparam
	class IndexSet
	{
	public:
	    /// @brief
	    /// @todo Doc me!
	    typedef int IndexType;

	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    IndexSet(const CpGridData& grid)
		: grid_(grid)
	    {
		GeometryType t;
		t.makeCube(3);
		geom_types_[0].push_back(t);
		t.makeCube(0);
		geom_types_[3].push_back(t);
	    }

            /// \brief Destructor.
            ~IndexSet()
            {}
            
	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    /// @return
	    const std::vector<GeometryType>& geomTypes(int codim) const
	    {
		return geom_types_[codim];
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    /// @return
	    int size(GeometryType type) const
	    {
		return grid_.size(type);
	    }


	    /// @brief
	    /// @todo Doc me!
	    /// @param
	    /// @return
	    int size(int codim) const
	    {
		return grid_.size(codim);
	    }


	    /// @brief
	    /// @todo Doc me!
	    /// @tparam
	    /// @return
	    /// @param
	    template<int cd>
	    IndexType index(const cpgrid::Entity<cd>& e) const 
	    {
		return e.index(); 
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam
	    /// @return
	    /// @param
	    template<class EntityType>
	    IndexType index(const EntityType& e) const 
	    {
		return e.index();
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam
	    /// @return
	    /// @param
	    template <int cc>
	    IndexType subIndex(const cpgrid::Entity<0>& e, int i) const 
	    {
		return index(e.template subEntity<cc>(i));
	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam
	    /// @return
	    /// @param
	    IndexType subIndex(const cpgrid::Entity<0>& e, int i, unsigned int cc) const 
	    {
		switch(cc) {
		case 0: return index(e.subEntity<0>(i));
		case 1: return index(e.subEntity<1>(i));
		case 2: return index(e.subEntity<2>(i));
		case 3: return index(e.subEntity<3>(i));
		default: OPM_THROW(std::runtime_error, "Codimension " << cc << " not supported.");
		}

	    }

	    /// @brief
	    /// @todo Doc me!
	    /// @tparam
	    /// @return
	    /// @param
	    template <class EntityType>
	    bool contains(const EntityType& e) const
	    {
		return index(e) >= 0 && index(e) < grid_.size(EntityType::codimension); //EntityType::codimension == 0;
	    }

	private:
	    const CpGridData& grid_;
	    std::vector<GeometryType> geom_types_[4];
	};


	class IdSet
	{
	public:
	    typedef int IdType;

	    IdSet(const CpGridData& grid)
		: grid_(grid)
	    {
	    }

	    template<int cc>
	    IdType id(const cpgrid::Entity<cc>& e) const 
	    {
		return id(e);
	    }

	    template<class EntityType>
	    IdType id(const EntityType& e) const 
	    {
        IdType myId = 0;
        for( int c=0; c<EntityType::codimension; ++c ) 
          myId += grid_.indexSet().size( c );
        return  myId + e.index();
	    }

	    template<int cc>
	    IdType subId(const cpgrid::Entity<0>& e, int i) const 
	    {
		return id(e.template subEntity<cc>(i));
	    }

	    IdType subId(const cpgrid::Entity<0>& e, int i, int cc) const
	    {
		switch (cc) {
		case 0: return id(e.subEntity<0>(i));
		case 1: return id(e.subEntity<1>(i));
		case 2: return id(e.subEntity<2>(i));
		case 3: return id(e.subEntity<3>(i));
		default: OPM_THROW(std::runtime_error, "Cannot get subId of codimension " << cc);
		}
		return -1;
	    }
	private:
	    const CpGridData& grid_;
	};


	class GlobalIdSet
	{
	public:
	    typedef int IdType;

	    void swap(std::vector<int>& cellMapping,
                      std::vector<int>& faceMapping,
                      std::vector<int>& pointMapping)
	    {
                idSet_=nullptr;
                cellMapping_.swap(cellMapping);
                faceMapping_.swap(faceMapping);
                pointMapping_.swap(pointMapping);
	    }
            GlobalIdSet(const IdSet* ids)
            : idSet_(ids)
            {}
            GlobalIdSet()
                : idSet_()
            {}
	    template<int cc>
	    IdType id(const cpgrid::Entity<cc>& e) const 
	    {
		return id(e);
	    }

	    template<class EntityType>
	    IdType id(const EntityType& e) const 
	    {
                if(idSet_)
                    return idSet_->id(e);
                else 
                    return getMapping(e)[e.index()];
	    }

	    template<int cc>
	    IdType subId(const cpgrid::Entity<0>& e, int i) const 
	    {
		return id(e.template subEntity<cc>(i));
	    }

	    IdType subId(const cpgrid::Entity<0>& e, int i, int cc) const
	    {
		switch (cc) {
		case 0: return id(*e.subEntity<0>(i));
		case 1: return id(*e.subEntity<1>(i));
		case 2: return id(*e.subEntity<2>(i));
		case 3: return id(*e.subEntity<3>(i));
		default: OPM_THROW(std::runtime_error, "Cannot get subId of codimension " << cc);
		}
		return -1;
	    }
	private:
            const IdSet* idSet_;
            std::vector<int> cellMapping_;
            std::vector<int> faceMapping_;
            std::vector<int> pointMapping_;
            
            const std::vector<int>& getMapping(const EntityRep<0>&) const
            {
                return cellMapping_;
            }
            const std::vector<int>& getMapping(const EntityRep<1>&) const
            {
                return faceMapping_;
            }
            const std::vector<int>& getMapping(const EntityRep<3>&) const
            {
                return pointMapping_;
            }
	};


    } // namespace cpgrid
} // namespace Dune

#endif // OPM_INDEXSETS_HEADER
