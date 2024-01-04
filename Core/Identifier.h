#pragma once

namespace aoe {

using TypeId = size_t;

class Identifier {
public:
	static const TypeId kInvalidId = 0;

	Identifier() = delete;

	template<typename T>
	static TypeId GetTypeId() {
		static TypeId id = ++current_id_;
		return id;
	}

private:
	static inline TypeId current_id_ = 1;
};

} //  namespace aoe

