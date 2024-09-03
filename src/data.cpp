/**
 * @file data.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ethercat_interface/data.hpp"

#include "ethercat_interface/ec_common_defs.hpp"

namespace ec
{
    namespace data
    {

        Data::Data()
        {

        }

        Data::~Data()
        {
            
        }

        DataMap::DataMap()
        {
            
        }

        DataMap::DataMap(const std::vector<PDO_Entry>& entries)
            : m_EntryInfoArr(entries)
        {
            
        }

        DataMap::~DataMap()
        {

        }

        bool DataMap::init(const std::vector<PDO_Entry>& entries)
        {
            const std::size_t numEntries = entries.size();

            for(std::size_t i = 0; i < numEntries; i++)
            {
                const auto entry = entries.at(i);
                m_Data[entry.entryName] = std::make_shared<Data>();


            }

            return true;
        }

        bool DataMap::init()
        {
            
            if(m_EntryInfoArr.empty()){
                return false;
            }

            return init(m_EntryInfoArr);
        }
        

    } // End of namespace data
} // End of namespace ec