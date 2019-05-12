#pragma once
#include "json_utils.h"

namespace ravendb::client::tests::infrastructure::entities
{
	struct GeekPerson
	{
		std::string name;
		std::vector<int32_t> favorite_primes{};
		std::vector<int64_t> favorite_very_large_primes{};
	};

	inline void to_json(nlohmann::json& j, const GeekPerson& gp)
	{
		using ravendb::client::impl::utils::json_utils::set_val_to_json;

		set_val_to_json(j, "Name", gp.name);
		set_val_to_json(j, "FavoritePrimes", gp.favorite_primes);
		set_val_to_json(j, "FavoriteVeryLargePrimes", gp.favorite_very_large_primes);
	}

	inline void from_json(const nlohmann::json& j, GeekPerson& gp)
	{
		using ravendb::client::impl::utils::json_utils::get_val_from_json;

		get_val_from_json(j, "Name", gp.name);
		get_val_from_json(j, "FavoritePrimes", gp.favorite_primes);
		get_val_from_json(j, "FavoriteVeryLargePrimes", gp.favorite_very_large_primes);
	}
}

