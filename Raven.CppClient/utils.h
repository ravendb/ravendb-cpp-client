#pragma once

namespace raven {
	namespace impl {
		class Utils {
		public:
			static size_t push_headers(char *buffer, size_t size, size_t nitems, void *userdata);
			static size_t push_to_buffer(char* contents, size_t size, size_t nmemb, void* output_buffer_void);
			static std::string url_escape(CURL* curl, std::string& val);
		};
	}
}
