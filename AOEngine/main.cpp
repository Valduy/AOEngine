#include <iostream>

#include "../Core/Event.h"

class Subject {
public:
	aoe::Event<Subject,size_t> ActionHappened;

	void Foo() {
		ActionHappened.Notify(1);
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