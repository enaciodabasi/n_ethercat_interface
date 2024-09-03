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
#include <shared_mutex>

#include "ec_common_defs.hpp"

namespace ec
{
    namespace data
    {

        typedef std::variant<
            std::monostate,
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
                if(std::holds_alternative<std::monostate>(m_Data)){
                    m_Data = val;
                    return true;
                }
                else if(!std::holds_alternative<T>(m_Data)){ // If the value inside m_DataVar does not hold the same type as the template argument:
                    return false;
                }
            
                m_Data = val;
                
                return true;
            }

            template<typename T>
            const std::optional<T> get()
            {   
                //if(!std::holds_alternative<T>(m_Data.value())){
                //    return std::nullopt;
                //}
//
                //return std::get<T>(m_Data);

                if(!std::holds_alternative<T>(m_Data)){
                    return std::nullopt;
                }
                
                T data = std::get<T>(m_Data);
                m_Data = std::monostate();
                    
                return data;

            }   

            private:
            
            DataVar m_Data;

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
                
                std::optional<T> data;
                if(m_DataShMutex.try_lock_shared()){
                    data = entry->second->get<T>();
                    m_DataShMutex.unlock_shared();
                }
            
                return data;

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
                
                bool setOp = false;

                if(m_DataShMutex.try_lock()){
                    setOp = entry->second->set<T>(val);   
                    m_DataShMutex.unlock();
                }
                
                return setOp;
            }

            private:

            std::unordered_map<std::string, std::shared_ptr<Data>> m_Data;

            std::vector<PDO_Entry> m_EntryInfoArr;

            std::shared_mutex m_DataShMutex;
            

        };

    } // End of namespace data
}
// End of namespace ec

#endif // DATA_HPP_

