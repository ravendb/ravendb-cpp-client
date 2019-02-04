#include "stdafx.h"
#include "MultiLoaderWithInclude.h"
#include "LoaderWithInclude.h"

namespace ravendb::client::documents::session::loaders
{
	LoaderWithInclude MultiLoaderWithInclude::create(DocumentSessionImpl& session)
	{
		auto multi_loader = std::shared_ptr<MultiLoaderWithInclude>(new MultiLoaderWithInclude(session));

		// here we create a weak ptr to ourselves in order to to be able to pass a shared ptr based on it to generated LoaderWithInclude,
		// as we do in MultiLoaderWithInclude::include
		multi_loader->_weak_this = multi_loader;

		return LoaderWithInclude(multi_loader);
	}

	LoaderWithInclude MultiLoaderWithInclude::include(std::string path)
	{
		_includes.emplace_back(std::move(path));

		// here pass to LoaderWithInclude a sharedPtr to ourselves, in order to be able to track usages of MultiLoaderWithInclude safely
		return LoaderWithInclude(_weak_this.lock());
	}

}
