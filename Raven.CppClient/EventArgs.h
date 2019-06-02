#pragma once

namespace ravendb::client::primitives
{
	class VoidArgs;

	//Parent class for all EventArgs
	class EventArgs
	{
		//empty by design

	public:
		static const VoidArgs EMPTY;
	};

	class VoidArgs : public EventArgs
	{};
}
