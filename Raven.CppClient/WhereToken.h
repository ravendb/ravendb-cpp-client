#pragma once
#include "QueryToken.h"
#include "SearchOperator.h"
#include "ShapeToken.h"
#include "WhereOperator.h"

namespace ravendb::client::documents::session::tokens
{
	class WhereToken : public QueryToken
	{
	public:
		enum class MethodsType
		{
			CMP_X_CHG
		};

		struct WhereMethodCall
		{
			MethodsType method_type{};
			std::vector<std::string> parameters{};
			std::optional<std::string> property{};
		};

		struct WhereOptions
		{
			std::optional<queries::SearchOperator> search_operator{};
			std::string from_parameter_name{};
			std::string to_parameter_name{};
			std::optional<double> boost{};
			std::optional<double> fuzzy{};
			std::optional<int32_t> proximity{};
			bool exact = false;
			std::optional<WhereMethodCall> method{};
			std::shared_ptr<ShapeToken> where_shape{};
			double distance_error_pct{};

		private:
			WhereOptions();

		public:
			static WhereOptions default_options();

			explicit WhereOptions(bool exact_param);

			WhereOptions(bool exact_param, std::string from, std::string to);

			explicit WhereOptions(queries::SearchOperator search);

			WhereOptions(std::shared_ptr<ShapeToken> shape, double distance);

			WhereOptions(MethodsType method_type, std::vector<std::string> parameters, std::string property, bool exact_param = false);
		};

	private:
		std::weak_ptr<WhereToken> _weak_this{};

	public:
		std::string field_name{};
		WhereOperator where_operator{};
		std::string parameter_name{};
		WhereOptions options = WhereOptions::default_options();

	private:
		bool write_method(std::ostringstream& writer) const;

		void write_inner_where(std::ostringstream& writer) const;

		void special_operator(std::ostringstream& writer) const ;

	protected:
		WhereToken();

	public:
		~WhereToken() override = default;

		static std::shared_ptr<WhereToken> create(WhereOperator op, std::string field_name,
			std::string parameter_name, std::optional<WhereOptions> options = {});

		std::shared_ptr<WhereToken> add_alias(const std::string& alias);

		void write_to(std::ostringstream& writer) const override;
	};
}
