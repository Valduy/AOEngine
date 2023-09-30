#pragma once

#include <new>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>

#include "IComponent.h"

namespace aoe {

class Entity {
private:
	using LookupIndex = int32_t;

public:
	template<typename TComponent>
	class ComponentHandler {
		friend class Entity;

	public:
		bool IsValid() const {
			if (lookup_index_ == kInvalidIndex || source_->table_.GetSize() <= lookup_index_) {
				return false;
			}

			Lookup lookup = source_->table_[lookup_index_];

			if (lookup.type_id_ != type_id_) {
				return false;
			}

			return true;
		}

		TComponent* operator->() const {
			if (!IsValid()) {
				assert(false && "Handler is invalid.");
				return nullptr;
			}

			Lookup lookup = source_->table_[lookup_index_];
			NodeHandler node(source_, lookup.offset_);
			return node.GetComponent<TComponent>();
		}

	private:
		Entity* source_;
		const TypeId type_id_;
		const LookupIndex lookup_index_;

		ComponentHandler(
			Entity* source,
			TypeId type_id,
			LookupIndex lookup_index)
			: source_(source)
			, type_id_(type_id)
			, lookup_index_(lookup_index)
		{}
	};

	Entity()
		: root_(this, kInvalidOffset)
		, buffer_(nullptr)
		, size_(0)
		, capacity_(0)
		, table_()
	{}

	~Entity() {
		if (buffer_ == nullptr) {
			return;
		}

		for (NodeHandler temp = GetFirstNode(); !IsEnd(temp); temp = GetNextNode(temp)) {
			temp.FreeComponent();
		}
		
		free(buffer_);
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Add() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		NodeHandler handler = GetOrCreateNode(key, sizeof(TComponent));
		handler.SetComponent<TComponent>();
		return { this, key, handler.GetLookupIndex() };
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Get() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		NodeHandler handler = GetNode(key);

		if (handler.IsValid()) {
			return { this, handler.GetKey(), handler.GetLookupIndex() };
		}

		return { this, Identifier::kInvalidId, kInvalidIndex };
	}

	template<typename TComponent>
	void Remove() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		NodeHandler removed(this, kInvalidOffset);
		root_ = Remove(root_, key, removed);
		
		if (removed.IsValid()) {
			Compress(removed);
		}
	}

#pragma region Debug

	void DebugPrint() {
		if (root_.IsValid()) {
			PrintTree(root_, "", true);
		}	
	}

#pragma endregion Debug

private:
	using Offset = int32_t;
	using Height = char8_t;

	struct Node {
		TypeId key;
		size_t size;
		Height height;
		LookupIndex lookup_index;

		Offset parent_offset;
		Offset left_offset;
		Offset right_offset;

		Node()
			: Node(Identifier::kInvalidId, 0, kInvalidIndex)
		{}

		Node(TypeId key, Offset size, LookupIndex lookup_index)
			: key(key)
			, size(size)
			, height(1)
			, lookup_index(lookup_index)
			, parent_offset(kInvalidOffset)
			, left_offset(kInvalidOffset)
			, right_offset(kInvalidOffset)
		{}
	};
	
	class NodeHandler {
	public:
		Node* GetNode() const {
			return reinterpret_cast<Node*>(GetAddress());
		}

		bool IsValid() const {
			return offset_ != kInvalidOffset;
		}

		Offset GetOffset() const {
			return offset_;
		}

		void* GetAddress() const {
			assert(IsValid() && offset_ <= owner_->size_ && "Memory corruption.");
			return reinterpret_cast<char*>(owner_->buffer_) + offset_;
		}

		TypeId GetKey() const {
			return GetNode()->key;
		}

		size_t GetSize() const {
			return GetNode()->size;
		}

		LookupIndex GetLookupIndex() const {
			return GetNode()->lookup_index;
		}

		NodeHandler GetParent() const {
			return { owner_, GetNode()->parent_offset };
		}

		void ResetParent() {
			GetNode()->parent_offset = kInvalidOffset;
		}

		NodeHandler GetLeft() const {
			return { owner_, GetNode()->left_offset };
		}

		void SetLeft(NodeHandler node) {
			if (node.IsValid()) {
				node.SetParent(*this);
			}
			
			GetNode()->left_offset = node.GetOffset();
		}

		NodeHandler GetRight() const {
			return { owner_, GetNode()->right_offset };
		}

		void SetRight(NodeHandler node) {
			if (node.IsValid()) {
				node.SetParent(*this);
			}
			
			GetNode()->right_offset = node.GetOffset();
		}

		Height GetHeight() {
			return IsValid() ? GetNode()->height : 0;
		}

		void SetHeight(Height height) {
			GetNode()->height = height;
		}

		int GetBalanceFactor() {
			return GetRight().GetHeight() - GetLeft().GetHeight();
		}

		void FixHeight() {
			Height left_height = GetLeft().GetHeight();
			Height right_height = GetRight().GetHeight();
			Height height = std::max(left_height, right_height) + 1;
			SetHeight(height);
		}

		IComponent* GetComponent() const {
			char* temp = reinterpret_cast<char*>(GetAddress());
			temp += sizeof(Node);
			return reinterpret_cast<IComponent*>(temp);
		}

		template<typename TComponent>
		TComponent* GetComponent() const {
			IComponent* component = GetComponent();
			return reinterpret_cast<TComponent*>(component);
		}

		template<typename TComponent>
		void SetComponent() const {
			char* temp = reinterpret_cast<char*>(GetAddress());
			temp += sizeof(Node);
			new (temp) TComponent;
		}

		void FreeComponent() {
			GetComponent()->~IComponent();
		}

		NodeHandler(Entity* owner, Offset offset)
			: owner_(owner)
			, offset_(offset)
		{}

		Node* operator->() const {
			return GetNode();
		}
		
	private:
		Entity* owner_;
		Offset offset_;

		void SetParent(NodeHandler node) {
			GetNode()->parent_offset = node.GetOffset();
		}
	};

	struct Lookup {
		TypeId type_id_{ Identifier::kInvalidId };
		Offset offset_{ kInvalidOffset};

		bool IsValid() const {
			return type_id_ != Identifier::kInvalidId;
		}
	};

	class LookupTable {
	public:
		size_t GetSize() {
			return table_.size();
		}
		
		LookupTable()
			: table_()
			, head_(kInvalidIndex)
		{}

		LookupIndex CreateLookup(TypeId type_id, Offset offset) {
			if (head_ == kInvalidIndex) {
				Lookup& lookup = table_.emplace_back(type_id, offset);
				return table_.size() - 1;
			} else {
				int lookup_index = head_;
				Lookup& lookup = table_[lookup_index];
				head_ = lookup.offset_;
				lookup.type_id_ = type_id;
				lookup.offset_ = offset;
				return lookup_index;
			}			
		}

		void DeleteLookup(LookupIndex lookup_index) {
			assert(lookup_index >= 0 && lookup_index < table_.size() && "Invalid lookup index.");
			Lookup& lookup = table_[lookup_index];
			lookup.type_id_ = Identifier::kInvalidId;
			lookup.offset_ = head_;
			head_ = lookup_index;
		}

		Lookup& operator[](LookupIndex index) {
			return table_[index];
		}

	private:
		std::vector<Lookup> table_;
		LookupIndex head_;
	};

	friend bool operator==(const NodeHandler& lhs, const NodeHandler& rhs) {
		return lhs.GetOffset() == rhs.GetOffset();
	}

	static const Offset kInvalidOffset = -1;
	static const LookupIndex kInvalidIndex = -1;

	NodeHandler root_;

	void* buffer_;
	size_t size_;
	size_t capacity_;

	LookupTable table_;

#pragma region Debug

	void PrintTree(NodeHandler handler, std::string indent, bool last)
	{

		std::cout << indent << "+- " << handler->key 
			<< "(" 
			<< "Parent:" << (handler.GetParent().IsValid() ? handler.GetParent()->key : 0) << "|"
			<< "Left:" << (handler.GetLeft().IsValid() ? handler.GetLeft()->key : 0) << "|"
			<< "Rigth:" << (handler.GetRight().IsValid() ? handler.GetRight()->key : 0)
			<< ")\n";
		indent += last ? "   " : "|  ";

		if (handler.GetLeft().IsValid())
		{
			PrintTree(handler.GetLeft(), indent, false);
		}
		if (handler.GetRight().IsValid())
		{
			PrintTree(handler.GetRight(), indent, true);
		}
	}

#pragma endregion Debug

	NodeHandler GetNode(TypeId key) {
		NodeHandler result = root_;

		while (result.IsValid()) {
			if (result.GetKey() > key) {
				result = result.GetLeft();
				continue;
			}

			if (result.GetKey() < key) {
				result = result.GetRight();
				continue;
			}

			return result;
		}

		return result;
	}

	NodeHandler CreateNewNode(TypeId key, size_t component_size) {
		size_t available = capacity_ - size_;
		size_t desired = sizeof(Node) + component_size;

		if (available < desired) {			
			void* temp = buffer_;
			capacity_ = size_ + desired; // TODO: amortization
			buffer_ = malloc(capacity_);
			memmove(buffer_, temp, size_);

			if (temp != nullptr) {
				free(temp);
			}		
		}
		
		Offset offset = size_;
		void* address = reinterpret_cast<char*>(buffer_) + offset;
		LookupIndex lookup_index = table_.CreateLookup(key, offset);
		new (address) Node(key, component_size, lookup_index);
		size_ += desired;
		return { this, offset };
	}

	NodeHandler GetOrCreateNode(TypeId key, size_t component_size) {
		NodeHandler result{ this, kInvalidOffset };
		root_ = GetOrCreateNode(root_, key, component_size, result);
		return result;
	}

	NodeHandler Remove(NodeHandler handler, TypeId key, NodeHandler& removed) {
		if (!handler.IsValid()) {
			return handler;
		}

		if (key < handler.GetKey()) {
			NodeHandler left = handler.GetLeft();
			handler.SetLeft(Remove(left, key, removed));
		} else if (key > handler.GetKey()) {
			NodeHandler right = handler.GetRight();
			handler.SetRight(Remove(right, key, removed));
		} else {
			NodeHandler parent = handler.GetParent();
			NodeHandler left = handler.GetLeft();
			NodeHandler right = handler.GetRight();

			removed = handler;
			table_.DeleteLookup(handler->lookup_index);
			handler.FreeComponent();
			
			if (!right.IsValid()) {
				return left;
			}

			NodeHandler min(this, kInvalidOffset);
			NodeHandler subroot = RemoveMinFromSubtree(right, min);
			min.SetRight(subroot);
			min.SetLeft(left);
			return Balance(min);
		}

		return Balance(handler);
	}

	NodeHandler RemoveMinFromSubtree(NodeHandler handler, NodeHandler& min) {
		NodeHandler left = handler.GetLeft();

		if (!left.IsValid()) {
			min = handler;
			return handler.GetRight();
		}

		handler.SetLeft(RemoveMinFromSubtree(left, min));
		return Balance(handler);
	}

	NodeHandler GetOrCreateNode(
		NodeHandler handler, 
		TypeId key, 
		size_t component_size,
		NodeHandler& result) 
	{
		if (!handler.IsValid()) {
			NodeHandler node = CreateNewNode(key, component_size);		
			result = node;
			return node;
		}

		if (key < handler.GetKey()) {
			NodeHandler left = handler.GetLeft();
			handler.SetLeft(GetOrCreateNode(left, key, component_size, result));
		} else if (key > handler.GetKey()) {
			NodeHandler right = handler.GetRight();
			handler.SetRight(GetOrCreateNode(right, key, component_size, result));
		} else {
			result = handler;
			handler.FreeComponent();
		}
		
		return Balance(handler);
	}

	NodeHandler Balance(NodeHandler handler) {
		handler.FixHeight();
		int balance_factor = handler.GetBalanceFactor();

		if (balance_factor == 2) {
			NodeHandler right = handler.GetRight();

			if (right.GetBalanceFactor() < 0) {
				handler.SetRight(RotateRight(right));
			}

			return RotateLeft(handler);
		}
		if (balance_factor == -2) {
			NodeHandler left = handler.GetLeft();

			if (left.GetBalanceFactor() > 0) {
				handler.SetLeft(RotateLeft(left));
			}

			return RotateRight(handler);
		}

		return handler;
	}

	NodeHandler RotateRight(NodeHandler handler) {
		NodeHandler left = handler.GetLeft();
		handler.SetLeft(left.GetRight());
		left.SetRight(handler);
		left.ResetParent();
		handler.FixHeight();
		left.FixHeight();
		return left;
	}

	NodeHandler RotateLeft(NodeHandler handler) {
		NodeHandler right = handler.GetRight();
		handler.SetRight(right.GetLeft());
		right.SetLeft(handler);
		right.ResetParent();
		handler.FixHeight();
		right.FixHeight();
		return right;
	}

	void Compress(NodeHandler removed) {
		NodeHandler next = GetNextNode(removed);
		Offset removed_offset = removed.GetOffset();
		size_t removed_size = GetBlockSize(removed);
		size_t move_size = size_ - removed.GetOffset() - removed_size;
		memmove(removed.GetAddress(), next.GetAddress(), move_size);
		size_ -= removed_size;

		if (root_.IsValid()) {
			for (NodeHandler temp = GetFirstNode(); !IsEnd(temp); temp = GetNextNode(temp)) {
				Node* node = temp.GetNode();

				if (temp.GetLeft().IsValid() && node->left_offset > removed_offset) {
					node->left_offset -= removed_size;
				}
				if (temp.GetRight().IsValid() && node->right_offset > removed_offset) {
					node->right_offset -= removed_size;
				}
				if (temp.GetParent().IsValid() && node->parent_offset > removed_offset) {
					node->parent_offset -= removed_size;
				}
			}

			Offset root_offset = root_.GetOffset();

			if (root_offset > removed_offset) {
				root_ = { this, root_offset - static_cast<Offset>(removed_size) };
			}
		}

		FixTable(removed_offset, removed_size);
	}

	void FixTable(size_t removed_offset, size_t removed_size) {
		for (size_t i = 0; i < table_.GetSize(); ++i) {
			Lookup& lookup = table_[i];

			if (lookup.IsValid() && lookup.offset_ > removed_offset) {
				lookup.offset_ -= removed_size;
			}
		}
	}

	size_t GetBlockSize(NodeHandler handler) {
		return sizeof(Node) + handler.GetSize();
	}

	NodeHandler GetFirstNode() {
		return { this, 0 };
	}

	NodeHandler GetNextNode(NodeHandler handler) {
		Offset next_offset = handler.GetOffset() + GetBlockSize(handler);
		return { this, next_offset };
	}

	bool IsEnd(NodeHandler handler) {
		return handler.GetOffset() >= size_;
	}
};

} // namespace aoe