/**
 * @file data.hpp
 * @author Eren Naci Odabasi (enaciddabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include <queue>
#include <any>
#include <variant>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>

#include "ec_common_defs.hpp"

namespace ec
{
    namespace data
    {

        typedef std::variant<
            uint8_t,
            uint16_t,
            uint32_t,
            uint64_t,
            int8_t,
            int16_t,
            int32_t,
            int64_t,
            float,
            double,
            bool
        > DataVar;

        class Data
        {

            public:

            Data();

            ~Data();

            template<typename T>
            bool set(const T& val)
            {   
                // If the queue is not empty, check the first element to validate the data type.
                if(!m_DataQueue.empty()){
                    const auto fVal = m_DataQueue.front();
                    if(!std::holds_alternative<T>(fVal)){
                        return false;
                    }
                }
                m_DataQueue.push(val);

                return true;
            }

            template<typename T>
            std::optional<T> get()
            {
                if(m_DataQueue.empty()){
                    return std::nullopt;
                }

                const auto val = m_DataQueue.front();
                if(!std::holds_alternative<T>(val)){
                    return std::nullopt;
                }

                m_DataQueue.pop();

                return std::get<T>(val);
            }

            private:
            
            std::queue<DataVar> m_DataQueue;

        };

        class DataMap
        {

            public:

            /**
             * @brief Default constructor, the init function must be called with a PDO_Entry argument if this constructor is used.
             * 
             */
            DataMap();

            /**
             * @brief Constructs a new DataMap with the given entries
             * 
             * @param entries A vector of PDO_Entry structs.
             */
            DataMap(const std::vector<PDO_Entry>& entries);

            ~DataMap();

            /**
             * @brief 
             * 
             * @return true 
             * @return false 
             */
            bool init();

            /**
             * @brief 
             * 
             * @param entries 
             * @return true 
             * @return false 
             */
            bool init(const std::vector<PDO_Entry>& entries);

            /**
             * @brief 
             * 
             * @tparam T 
             * @param data_name 
             * @return std::optional<T> 
             */
            template <typename T>
            std::optional<T> get(const std::string& data_name)
            {
                const auto& entry = m_Data.find(data_name);
                if(entry == m_Data.end()){
                    return std::nullopt;
                }

                const std::optional<T> data = entry->second->get<T>();
                if(!data){
                    return std::nullopt;
                }

                return data.value();

            }

            /**
             * @brief 
             * 
             * @tparam T 
             * @param data_name 
             * @param val 
             * @return true 
             * @return false 
             */
            template<typename T>
            bool set(const std::string& data_name, const T& val)
            {
                const auto& entry = m_Data.find(data_name);
                if(entry == m_Data.end()){
                    return false;
                }

                return entry->second->set<T>(val);
            }

            private:

            std::unordered_map<std::string, std::unique_ptr<Data>> m_Data;

            std::vector<PDO_Entry> m_EntryInfoArr;

        };

    } // End of namespace data
}
// End of namespace ec

#endif // DATA_HPP_

