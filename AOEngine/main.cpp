#include <iostream>

#include "../Core/Event.h"

class Subject {
public:
	aoe::Event<Subject> ActionHappened;

	void Foo() {
		ActionHappened.Notify();
	}
};

class Observer {
public:
	void Bar() {
		std::cout << "Bar" << std::endl;
	}
};

int main() {
	Subject subject;
	Observer observer;

	subject.ActionHappened.Attach(observer, &Observer::Bar);
	subject.Foo();

	return 0;
}