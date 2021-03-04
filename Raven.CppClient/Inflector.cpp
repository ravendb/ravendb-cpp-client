#include "stdafx.h"
#include "Inflector.h"
#include "utils.h"
#include "GetClusterTopologyCommand.h"

namespace ravendb::client::impl
{
	std::vector<Inflector::Rule> Inflector::plurals{};
	std::vector<Inflector::Rule> Inflector::singulars{};
	std::vector<std::string> Inflector::uncountables{};

	Inflector::FillRules Inflector::rules_filler{};

	Inflector::Rule::Rule(const std::string& pattern, std::string replacement)
		: _regex(pattern, std::regex_constants::ECMAScript | std::regex_constants::icase)
		, _replacement(std::move(replacement))
	{}

	std::string Inflector::Rule::apply(const std::string& word) const
	{
		std::smatch match;
		if(!std::regex_search(word, match, _regex))
		{
			return {};
		}
		return std::regex_replace(word, _regex, _replacement);
	}

	Inflector::FillRules::FillRules()
	{
        add_plural("$", "s");
        add_plural("s$", "$1s");
		add_plural("(ax|test)is$", "$1es");
		add_plural("(octop|vir)us$", "$1i");
		add_plural("(alias|status)$", "$1es");
		add_plural("(bu)s$", "$1ses");
		add_plural("(buffal|tomat)o$", "$1oes");
		add_plural("([ti])um$", "$1a");
		add_plural("(.*)sis$", "$1ses");
		add_plural("(?:([^f])fe|([lr])f)$", "$1$2ves");
		add_plural("(hive)$", "$1s");
		add_plural("([^aeiouy]|qu)y$", "$1ies");
		add_plural("(x|ch|ss|sh)$", "$1es");
		add_plural("(matr|vert|ind)ix|ex$", "$1ices");
		add_plural("([m|l])ouse$", "$1ice");
		add_plural("^(ox)$", "$1en");
		add_plural("(quiz)$", "$1zes");

		add_singular("(.*)s$", "");
		add_singular("(n)ews$", "$1ews");
		add_singular("([ti])a$", "$1um");
		add_singular("((a)naly|(b)a|(d)iagno|(p)arenthe|(p)rogno|(s)ynop|(t)he)ses$", "$1$2sis");
		add_singular("(^analy)ses$", "$1sis");
		add_singular("([^f])ves$", "$1fe");
		add_singular("(hive)s$", "$1");
		add_singular("(tive)s$", "$1");
		add_singular("([lr])ves$", "$1f");
		add_singular("([^aeiouy]|qu)ies$", "$1y");
		add_singular("(s)eries$", "$1eries");
		add_singular("(m)ovies$", "$1ovie");
		add_singular("(x|ch|ss|sh)es$", "$1");
		add_singular("([m|l])ice$", "$1ouse");
		add_singular("(bus)es$", "$1");
		add_singular("(o)es$", "$1");
		add_singular("(shoe)s$", "$1");
		add_singular("(cris|ax|test)es$", "$1is");
		add_singular("(octop|vir)i$", "$1us");
		add_singular("(alias|status)es$", "$1");
		add_singular("^(ox)en", "$1");
		add_singular("(vert|ind)ices$", "$1ex");
		add_singular("(matr)ices$", "$1ix");
		add_singular("(quiz)zes$", "$1");

		add_irregular("person", "people");
		add_irregular("man", "men");
		add_irregular("child", "children");
		add_irregular("sex", "sexes");
		add_irregular("move", "moves");

		add_uncountable("equipment");
		add_uncountable("information");
		add_uncountable("rice");
		add_uncountable("money");
		add_uncountable("species");
		add_uncountable("series");
		add_uncountable("fish");
		add_uncountable("sheep");
	}

	std::string Inflector::apply_rules(const std::vector<Rule>& rules, const std::string& word)
	{
		std::string result = word;
		if(std::find(uncountables.cbegin(), uncountables.cend(), utils::to_lower_str(word)) ==
			uncountables.cend() && !rules.empty())
		{
			for(size_t i = rules.size() - 1; i >= 0; --i)
			{
				auto&& rule = rules.at(i);
				if(!(result = rule.apply(word)).empty())
				{
					break;
				}
			}
		}
		return result;
	}

	std::string Inflector::pluralize(const std::string& word)
	{
		return apply_rules(plurals, word);
	}

	std::string Inflector::singularize(const std::string& word)
	{
		return apply_rules(singulars, word);
	}

	std::string Inflector::capitalize(const std::string& word)
	{
		auto result = word;
		if(!word.empty())
		{
			result[0] = std::toupper(word[0]);
		}
		return result;
	}

	void Inflector::add_uncountable(const std::string& word)
	{
		uncountables.emplace_back(utils::to_lower_str(word));
	}

	void Inflector::add_plural(const std::string& rule, const std::string& replacement)
	{
		plurals.emplace_back(rule, replacement);
	}

	void Inflector::add_singular(const std::string& rule, const std::string& replacement)
	{
		singulars.emplace_back(rule, replacement);
	}

	void Inflector::add_irregular(const std::string& singular, const std::string& plural)
	{
		{
			std::ostringstream rule{};
			std::ostringstream replacement{};

			rule << "(" << singular.at(0) << ")" << singular.substr(1) << "$";
			replacement << "$1" << plural.substr(1);

			add_plural(rule.str(), replacement.str());
		}
		{
			std::ostringstream rule{};
			std::ostringstream replacement{};

			rule << "(" << plural.at(0) << ")" << plural.substr(1) << "$";
			replacement << "$1" + singular.substr(1);

			add_singular(rule.str(), replacement.str());
		}
	}
}
