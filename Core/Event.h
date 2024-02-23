#pragma once

#include "Delegate.h"

namespace aoe{

template<typename... TParams>
class EventBase {
public:
	template<typename TObject>
	bool Attach(TObject& object, Callback<TObject, TParams...> method) {
		return delegate_.Attach(object, method);
	}

	template<typename TObject>
	bool Detach(TObject& object, Callback<TObject, TParams...> method) {
		return delegate_.Detach(object, method);
	}

protected:
	void Notify(TParams... params) {
		delegate_.Notify(params...);
	}

private:
	Delegate<TParams...> delegate_;
};

template<typename TOwner, typename... TParams>
class Event : public EventBase<TParams...> {
private:
	friend typename TOwner;
};

} // namespace aoe