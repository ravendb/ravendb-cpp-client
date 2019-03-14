#include "stdafx.h"
#include "LazySessionOperationsImpl.h"
#include "LazySessionOperations.h"

namespace ravendb::client::documents::session::operations::lazy
{
	LazySessionOperations LazySessionOperationsImpl::create(std::shared_ptr<DocumentSessionImpl> session)
	{
		auto object = std::shared_ptr<LazySessionOperationsImpl>(new LazySessionOperationsImpl(session));
		
		//here we create a weak ptr to ourselves in order to to be able to pass a shared ptr based on it
		//to generated LazySessionOperationsImpl
		object->_weak_this = object;

		return LazySessionOperations(object);
	}
}
