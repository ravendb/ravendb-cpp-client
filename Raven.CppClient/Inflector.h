#pragma once
#include <regex>

namespace ravendb::client::impl
{
	//Class responsible for pluralizing words
	class Inflector
	{
	private:
		class Rule
		{
		private:
			const std::regex _regex;
			const std::string _replacement;

		public:
			Rule(const std::string& pattern, std::string replacement);

			std::string apply(const std::string& word) const;
		};

		static std::vector<Rule> plurals;
		static std::vector<Rule> singulars;
		static std::vector<std::string> uncountables;

		static struct FillRules{FillRules(); } rules_filler;

	private:
		static std::string apply_rules(const std::vector<Rule>& rules, const std::string& word);

	public:
		Inflector() = delete;
		~Inflector() = delete;

		static std::string pluralize(const std::string& word);
		static std::string singularize(const std::string& word);
		static std::string capitalize(const std::string& word);

		static void add_uncountable(const std::string& word);
		static void add_plural(const std::string& rule, const std::string& replacement);
		static void add_singular(const std::string& rule, const std::string& replacement);

		static void add_irregular(const std::string& singular, const std::string& plural);
	};
}
