#pragma once

#include "IEventHandler.h"
#include "Debug.h"

namespace aoe {

template<typename TObject, typename... TParams>
using Callback = void(TObject::*)(TParams...);

template<typename TObject, typename... TParams>
class EventHandler : public IEventHandler<TParams...> {
public:
	using Callback = Callback<TObject, TParams...>;

	EventHandler(TObject& object, Callback method)
		: object_(object)
		, method_(method)
	{
		AOE_ASSERT_MSG(method != nullptr, "Method can't be nullptr.");
	}

	void Call(TParams... params) override {
		(object_.*method_)(params...);
	}

	bool IsEqual(const IEventHandler<TParams...>& other) const override {
		const ThisType* concrete = dynamic_cast<const ThisType*>(&other);
		return concrete != nullptr
			&& &object_ == &(concrete->object_)
			&& method_ == concrete->method_;
	}

private:
	using ThisType = EventHandler<TObject, TParams...>;
	using BaseType = IEventHandler<TParams...>;

	TObject& object_;
	Callback method_;
};

} // namespace aoe