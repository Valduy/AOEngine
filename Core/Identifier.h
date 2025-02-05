#pragma once

namespace aoe {

using TypeId = size_t;

template<typename TOwner>
class IdentifierBase {
public:
	IdentifierBase() = delete;

	template<typename T>
	static TypeId GetTypeId() {
		static TypeId id = current_id_++;
		return id;
	}

private:
	static inline TypeId current_id_ = 0;
};

class Identifier : public IdentifierBase<Identifier> {};

} //  namespace aoe

