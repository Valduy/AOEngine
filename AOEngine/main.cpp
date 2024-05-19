#include <iostream>

#include "../Core/Event.h"

class ISubject {
public:
	aoe::Event<ISubject, size_t> ActionHappened;

protected:
	void NotifyActionHappened() {
		ActionHappened.Notify(1);
	}
};

class Subject : public ISubject {
public:
	void Foo() {
		NotifyActionHappened();
	}
};

class Observer {
public:
	void Bar(size_t num) {
		std::cout << "Bar " << num << std::endl;
	}
};

int main() {
	Subject subject;
	Observer observer;

	subject.ActionHappened.Attach(observer, &Observer::Bar);
	subject.Foo();

	return 0;
}