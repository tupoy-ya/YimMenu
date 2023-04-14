/**
 * @file cache_file.hpp
 * 
 * @copyright GNU General Public License Version 2.
 */

#pragma once
#include "file_manager/file.hpp"

namespace big
{
	class cache_header final
	{
	public:
		std::uint32_t m_cache_version;
		std::uint32_t m_game_version;
		float m_online_version;
		std::uint64_t m_data_size;
	};

	using cache_data = std::unique_ptr<std::uint8_t[]>;
	class cache_file final
	{
	public:
		/**
		 * @param cache_file FileMgr file object
		 * @param cache_version Internal version, use this to invalidate the cache when changing the structure of the data
		*/
		cache_file(file cache_file, std::uint32_t cache_version);

		/**
		 * @brief Frees any memory used to hold the cached data.
		 */
		void free();

		/**
		 * @brief Attempts to load the cache from disk
		 * 
		 * @return True after successfully loading the data, false if the file didn't exist.
		 */
		bool load();

		/**
		 * @brief Writes the cache to disk
		 */
		bool write() const;

		std::uint8_t* data() const;
		std::uint64_t data_size() const;

		/**
		 * @brief Check if the cache file is up to date with the expected versions.
		 * 
		 * @param game_version Current Game version
		 * @param online_version Current Online version
		 * @return True if cache is up to date, false otherwise. 
		 */
		bool up_to_date(std::uint32_t game_version, float online_version) const;


		void set_data(cache_data&& data, std::uint64_t data_size);
		/**
		 * @brief Sets the version information of the cache header.
		 * 
		 * @param game_version Game Build
		 * @param online_version Online Version
		 */
		void set_header_version(std::uint32_t game_version, float online_version);

	private:
		file m_cache_file;

		std::uint32_t m_cache_version;

		cache_header m_cache_header;
		cache_data m_data;
	};
}