#pragma once

namespace ravendb::client::primitives
{
	class EventHandler;
}

namespace ravendb::client::impl
{
	class IDisposalNotification
	{
	public:
		virtual ~IDisposalNotification() = 0;

		virtual void add_after_close_listener(primitives::EventHandler handler) = 0;
		virtual void remove_after_close_listener(const primitives::EventHandler& handler) = 0;

		virtual bool is_disposed() const = 0;
	};

	inline IDisposalNotification::~IDisposalNotification() = default;
}
