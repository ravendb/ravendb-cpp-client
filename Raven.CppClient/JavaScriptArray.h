#pragma once

namespace ravendb::client::documents::session
{
	template<typename U>
	class JavaScriptArray
	{
	private:
		const int32_t _suffix;
		int32_t _args_counter{};

		const std::string _path_to_array;
		std::vector<std::string> _script_lines{};
		std::unordered_map<std::string, nlohmann::json> _parameters{};

		std::string get_next_argument_name()
		{
			std::ostringstream argument_name{};
			argument_name << "val_" << _args_counter++ << "_" << _suffix;
			return argument_name.str();
		}

	public:
		JavaScriptArray(int32_t suffix, std::string path_to_array)
			: _suffix(suffix)
			, _path_to_array(std::move(path_to_array))
		{}

		JavaScriptArray& add(U u)
		{
			auto argument_name = get_next_argument_name();

			std::ostringstream script_line{};
			script_line << "this." << _path_to_array << ".push(args." << argument_name << ");";

			_script_lines.push_back(script_line.str());
			_parameters.insert_or_assign(argument_name, std::move(u));

			return *this;
		}

		JavaScriptArray& add(const std::vector<U>& us)
		{
			return add(us.cbegin(), us.cend());
		}

		template<typename InputIt >
		JavaScriptArray& add(InputIt first, InputIt last)
		{
			static_assert(std::is_same_v<typename std::iterator_traits<InputIt>::value_type, U>, "Invalid iterator type");

			std::ostringstream args{};

			std::transform(first, last, std::ostream_iterator(args,","), [](const U& value)->std::string
			{
				auto argument_name = get_next_argument_name();
				_parameters.insert_or_assign(argument_name, value);
				return "args." + argument_name;
			});

			std::ostringstream script_line{};
			script_line << "this." << _path_to_array << ".push(" << args.str() << ");";
			_script_lines.push_back(script_line.str());

			return *this;
		}

		JavaScriptArray& remove_at(int32_t index)
		{
			auto argument_name = get_next_argument_name();

			std::ostringstream script_line{};
			script_line << "this." << _path_to_array << ".splice(args." << argument_name << ", 1);";
			_script_lines.push_back(script_line.str());
			_parameters.insert_or_assign(argument_name, index);

			return *this;
		}

		std::string get_script() const
		{
			std::ostringstream script{};
			std::copy(_script_lines.cbegin(), _script_lines.cend(), std::ostream_iterator<std::string>(script, "\r"));
			return script.str();
		}

		const std::unordered_map<std::string, nlohmann::json>& get_parameters() const
		{
			return _parameters;
		}
	};
}
