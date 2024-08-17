#include "pch.h"

#include "../Core/Delegate.h"

namespace aoe_tests {
namespace core_tests {

template<typename... TParams>
class TestObserver {
public:
	TestObserver()
		: calls_count_(0)
		, delegate_(nullptr)
	{}

	void SetDelegate(aoe::Delegate<TParams...>* delegate) {
		delegate_ = delegate;
	}

	size_t GetCallsCount() const {
		return calls_count_;
	}

	void Callback(TParams... params) {
		calls_count_ += 1;

		if (delegate_ != nullptr) {
			delegate_->Detach(*this, &TestObserver<TParams...>::Callback);
		}
	}

private:
	size_t calls_count_;
	aoe::Delegate<TParams...>* delegate_;
};

class DelegateFixture : public testing::TestWithParam<size_t> {};

template<typename... TParams>
std::vector<TestObserver<TParams...>*> CreateObservers(size_t size);

template<typename... TParams>
void DeleteObservers(std::vector<TestObserver<TParams...>*>& observers);

bool IsAllCallbacksFiredCorrectly(std::vector<TestObserver<>*>& observers, size_t times);

TEST_P(DelegateFixture, Notify_AttachActionToDelegate_ActionsAreFired) {
	aoe::Delegate<> delegate;
	std::vector<TestObserver<>*> observers(CreateObservers(GetParam()));

	for (TestObserver<>* observer : observers) {
		delegate.Attach(*observer, &TestObserver<>::Callback);
	}

	delegate.Notify();

	EXPECT_TRUE(IsAllCallbacksFiredCorrectly(observers, 1));

	DeleteObservers(observers);
}

TEST_P(DelegateFixture, Notify_AttachAndDetachActionToDelegate_ActionsAreDetached) {
	aoe::Delegate<> delegate;
	std::vector<TestObserver<>*> observers(CreateObservers(GetParam()));

	for (TestObserver<>* observer : observers) {
		delegate.Attach(*observer, &TestObserver<>::Callback);
	}

	for (TestObserver<>* observer : observers) {
		delegate.Detach(*observer, &TestObserver<>::Callback);
	}

	delegate.Notify();

	EXPECT_TRUE(IsAllCallbacksFiredCorrectly(observers, 0));

	DeleteObservers(observers);
}

TEST_P(DelegateFixture, Notify_DetachDuringNotification_ActionsFiredCorrectly) {
	aoe::Delegate<> delegate;
	std::vector<TestObserver<>*> observers(CreateObservers(GetParam()));
	std::vector<TestObserver<>*> single_notification;
	std::vector<TestObserver<>*> multiple_notification;
	
	for (size_t i = 0; i < observers.size(); ++i) {
		TestObserver<>* observer = observers[i];

		if (i % 2 == 0) {
			single_notification.push_back(observer);
			observer->SetDelegate(&delegate);
		} else {
			multiple_notification.push_back(observer);
		}
	}

	for (TestObserver<>* observer : observers) {
		delegate.Attach(*observer, &TestObserver<>::Callback);
	}

	delegate.Notify();
	delegate.Notify();

	EXPECT_TRUE(IsAllCallbacksFiredCorrectly(single_notification, 1));
	EXPECT_TRUE(IsAllCallbacksFiredCorrectly(multiple_notification, 2));

	DeleteObservers(observers);
}

template<typename... TParams>
std::vector<TestObserver<TParams...>*> CreateObservers(size_t size) {
	std::vector<TestObserver<TParams...>*> observers(size);

	for (size_t i = 0; i < size; ++i) {
		observers[i] = new TestObserver<TParams...>();
	}

	return observers;
}

template<typename... TParams>
void DeleteObservers(std::vector<TestObserver<TParams...>*>& observers) {
	for (const auto* observer : observers) {
		delete observer;
	}
	
	observers.clear();
}

bool IsAllCallbacksFiredCorrectly(std::vector<TestObserver<>*>& observers, size_t times) {
	auto predicate = [&](const TestObserver<>* o) {
		return o->GetCallsCount() == times;
	};

	return std::all_of(observers.begin(), observers.end(), predicate);
}

INSTANTIATE_TEST_CASE_P(
	Delegate,
	DelegateFixture,
	testing::Values(0, 1, 2, 3, 4, 5), );

} // namespace delegate_tests
} // namespace aoe_tests