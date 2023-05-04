/**
 * @file pointers.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "base/HashTable.hpp"
#include "function_types.hpp"
#include "gta/fwddec.hpp"
#include "gta/replay.hpp"
#include "gta_pointers.hpp"
#include "memory/batch.hpp"
#include "memory/byte_patch.hpp"
#include "memory/module.hpp"
#include "socialclub/ScInfo.hpp"
#include "services/gta_data/cache_file.hpp"
#include "util/compile_time_helpers.hpp"
#include "gta_pointers.hpp"
#ifdef ENABLE_SOCIALCLUB
	#include "sc_pointers.hpp"
#endif // ENABLE_SOCIALCLUB

namespace big
{
	class pointers
	{
	private:
		template<cstxpr_str batch_name, auto batch_hash, size_t offset_of_cache_begin_field, size_t offset_of_cache_end_field, memory::batch batch>
		void write_to_cache_or_read_from_cache(cache_file& cache_file, const memory::module& mem_region)
		{
			static_assert(batch_hash > 0);

			cache_file.set_cache_version(batch_hash);

			const uintptr_t pointer_to_cacheable_data_start = reinterpret_cast<uintptr_t>(this) + offset_of_cache_begin_field;

			if (!is_pointers_cache_up_to_date<batch_name>(cache_file, mem_region))
			{
				run_batch<batch_name>(batch, mem_region);

				const uintptr_t pointer_to_cacheable_data_end = reinterpret_cast<uintptr_t>(this) + offset_of_cache_end_field;
				write_pointers_to_cache<batch_name, offset_of_cache_begin_field, offset_of_cache_end_field>(cache_file, pointer_to_cacheable_data_start, pointer_to_cacheable_data_end, mem_region);
			}
			else
			{
				load_pointers_from_cache(cache_file, pointer_to_cacheable_data_start, mem_region);
			}

			cache_file.free();
		}

		void load_pointers_from_cache(const cache_file& cache_file, const uintptr_t pointer_to_cacheable_data_start, const memory::module& mem_region);

		template<cstxpr_str batch_name, size_t offset_of_cache_begin_field, size_t offset_of_cache_end_field>
		void write_pointers_to_cache(cache_file& cache_file, const uintptr_t pointer_to_cacheable_data_start, const uintptr_t pointer_to_cacheable_data_end, const memory::module& mem_region)
		{
			constexpr size_t data_size = offset_of_cache_end_field - offset_of_cache_begin_field;

			cache_data cache_data_ptr = std::make_unique<std::uint8_t[]>(data_size);

			// multiple things here:
			// - iterate each cacheable field of the pointers instance
			// - substract the base module address so that we only keep the offsets
			// - save that to the cache
			uintptr_t* cache_data = reinterpret_cast<uintptr_t*>(cache_data_ptr.get());

			size_t i = 0;
			for (uintptr_t field_ptr = pointer_to_cacheable_data_start; field_ptr != pointer_to_cacheable_data_end; field_ptr += sizeof(uintptr_t))
			{
				const uintptr_t field_value = *reinterpret_cast<uintptr_t*>(field_ptr);

				if (mem_region.contains(memory::handle(field_value)))
				{
					const uintptr_t offset = field_value - mem_region.begin().as<uintptr_t>();
					cache_data[i]          = offset;
				}
				else
				{
					LOG(FATAL) << "Just tried to save to cache a pointer supposedly within the " << batch_name.str << " module range but isn't! Offset from start of pointers instance: " << (field_ptr - reinterpret_cast<uintptr_t>(this));
				}

				i++;
			}

			LOG(INFO) << "Pointers cache: saved " << (data_size / sizeof(uintptr_t)) << " fields to the cache";

			cache_file.set_data(std::move(cache_data_ptr), data_size);

			cache_file.set_header_version(mem_region.size());
			cache_file.write();
		}

		template<cstxpr_str batch_name>
		bool is_pointers_cache_up_to_date(cache_file& cache_file, const memory::module& mem_region)
		{
			cache_file.load();

			if (cache_file.up_to_date(mem_region.size()))
			{
				LOG(INFO) << batch_name.str << " pointers cache is up to date, using it.";

				return true;
			}

			return false;
		}

		static constexpr auto get_gta_batch();
#ifdef ENABLE_SOCIALCLUB
		static constexpr auto get_sc_batch();
#endif // ENABLE_SOCIALCLUB

		template<cstxpr_str batch_name, size_t N>
		void run_batch(const memory::batch<N>& batch, const memory::module& mem_region)
		{
			if (!memory::batch_runner::run(batch, mem_region))
			{
				const std::string error_message =
				    std::string("Failed to find some patterns for ") + std::string(batch_name.str);
				throw std::runtime_error(error_message);
			}
		}

	public:
		explicit pointers();
		~pointers();

	private:
		cache_file m_gta_pointers_cache;
#ifdef ENABLE_SOCIALCLUB
		cache_file m_sc_pointers_cache;
#endif // ENABLE_SOCIALCLUB

	public:
		HWND m_hwnd{};

		gta_pointers m_gta;

#ifdef ENABLE_SOCIALCLUB
		socialclub_pointers m_sc;
#endif // ENABLE_SOCIALCLUB		
	};

	inline pointers* g_pointers{};
}
