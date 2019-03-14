#include "stdafx.h"
#include "LazyMultiLoaderWithInclude.h"
#include "LazyLoaderWithInclude.h"

namespace ravendb::client::documents::session::loaders
{
	LazyLoaderWithInclude LazyMultiLoaderWithInclude::create(std::shared_ptr<DocumentSessionImpl> session)
	{
		auto lazy_multi_loader = std::shared_ptr<LazyMultiLoaderWithInclude>(new LazyMultiLoaderWithInclude(session));

		// here we create a weak ptr to ourselves in order to to be able to pass a shared ptr based on it to
		// generated LazyLoaderWithInclude,
		lazy_multi_loader->_weak_this = lazy_multi_loader;

		return LazyLoaderWithInclude(lazy_multi_loader);
	}

	LazyLoaderWithInclude LazyMultiLoaderWithInclude::include(std::string path)
	{
		_includes.emplace_back(std::move(path));

		// here pass to LoaderWithInclude a shared ptr to ourselves, in order to be able to track usages of MultiLoaderWithInclude safely
		return LazyLoaderWithInclude(_weak_this.lock());
	}

}
