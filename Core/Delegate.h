#pragma once

#include <list>
#include <algorithm>

#include "EventHandler.h"

namespace aoe {

template<typename... TParams>
class Delegate {
public:
	Delegate() 
		: handlers_()
		, current_it_(handlers_.end())
		, is_detached_(false)
	{}

	~Delegate() {
		for (IEventHandler<TParams...>* handler : handlers_) {
			delete handler;
		}

		handlers_.clear();
	}

	template<typename TObject>
	bool Attach(TObject& object, Callback<TObject, TParams...> method) {
		IEventHandler<TParams...>* handler = new EventHandler(object, method);
		Iterator it = FindIterator(*handler);

		if (it == handlers_.end()) {
			handlers_.push_back(handler);
			return true;
		}

		delete handler;
		return false;
	}

	template<typename TObject>
	bool Detach(TObject& object, Callback<TObject, TParams...> method) {
		EventHandler<TObject, TParams...> handler(object, method);
		auto it = FindIterator(handler);

		if (it == handlers_.end()) {
			return false;
		}
		
		if (it == current_it_) {
			is_detached_ = true;
		} else {
			DeleteIterator(it);
		}

		return true;
	}

	void Notify(TParams... params) {
		current_it_ = handlers_.begin();
		is_detached_ = false;

		while (current_it_ != handlers_.end()) {
			IEventHandler<TParams...>* handler = *current_it_;
			handler->Call(params...);

			if (is_detached_) {
				is_detached_ = false;
				Iterator detached = current_it_;
				++current_it_;
				DeleteIterator(detached);
			} else {
				++current_it_;
			}
		}
	}

private:
	using Iterator = std::list<IEventHandler<TParams...>*>::const_iterator;

	std::list<IEventHandler<TParams...>*> handlers_;

	Iterator current_it_;
	bool is_detached_;

	Iterator FindIterator(const IEventHandler<TParams...>& handler) {
		auto predicate = [&handler](const IEventHandler<TParams...>* other) {
			return other->IsEqual(handler);
		};

		return std::find_if(handlers_.begin(), handlers_.end(), predicate);
	}

	void DeleteIterator(Iterator it) {
		delete *it;
		handlers_.erase(it);
	}
};

} // namespace aoe