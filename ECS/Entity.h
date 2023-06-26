#pragma once

#include <new>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>

#include "IReflectable.h"

namespace aoe {

// maybe trivial identifier is better solution

class Entity {
public:

	Entity()
		: root_offset_(kInvalidOffset)
		, buffer_(nullptr)
		, size_(0)
		, capacity_(0)
	{

	}

	~Entity() {
		// TODO: go throw all components and call destructor manually;
		// free buffer;
	}

	template<typename TComponent>
	TComponent* Add() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		Node* node = GetNode(key, sizeof(TComponent));
		IReflectable* component = SetComponent<TComponent>(node);
		// TODO: return handler
		return static_cast<TComponent*>(component);
	}

	template<typename TComponent>
	void Remove() {

	}

	void DebugPrint() {
		if (root_offset_ != kInvalidOffset) {
			PrintTree(GetNode(root_offset_), "", true);
		}		
	}

private:
	using Offset = int32_t;
	
	struct Node {
		TypeId key;
		size_t size;
		unsigned char height;

		Offset parent_offset;
		Offset left_offset;
		Offset right_offset;

		Node()
			: Node(Identifier::kInvalid, 0)
		{}

		Node(TypeId key, Offset size)
			: key(key)
			, size(size)
			, height(0)
			, parent_offset(kInvalidOffset)
			, left_offset(kInvalidOffset)
			, right_offset(kInvalidOffset)
		{}
	};
	
	static const Offset kInvalidOffset = -1;
	static const Offset kZeroOffset = 0;

	Offset root_offset_;

	void* buffer_;
	size_t size_;
	size_t capacity_;

	static IReflectable* GetComponent(Node* node) {
		char* temp = reinterpret_cast<char*>(node);
		temp += sizeof(Node);
		return reinterpret_cast<IReflectable*>(temp);
	}

	template<typename TComponent>
	static TComponent* GetComponent(Node* node) {
		return static_cast<TComponent*>(GetComponent(node));
	}

	template<typename TComponent>
	static IReflectable* SetComponent(Node* node) {
		char* temp = reinterpret_cast<char*>(node);
		temp += sizeof(Node);
		IReflectable* component = new (temp) TComponent;
		return component;
	}

	void PrintTree(Node* tree, std::string indent, bool last)
	{
		std::cout << indent << "+- " << tree->key << "\n";
		indent += last ? "   " : "|  ";

		if (tree->left_offset != kInvalidOffset) 
		{
			PrintTree(GetNode(tree->left_offset), indent, false);
		}
		if (tree->right_offset != kInvalidOffset)
		{
			PrintTree(GetNode(tree->right_offset), indent, true);
		}
	}

	Node* GetNode(Offset offset) {
		if (offset == kInvalidOffset) {
			return nullptr;
		}

		char* temp = reinterpret_cast<char*>(buffer_) + offset;
		return reinterpret_cast<Node*>(temp);
	}

	Node* GetNode(TypeId key, size_t component_size) {
		if (root_offset_ == kInvalidOffset) {
			Offset root_offset = CreateNewNode(key, component_size);
			root_offset_ = root_offset;
			return GetNode(root_offset);
		}

		Offset offset = root_offset_;
		Node* node = GetNode(offset);

		while (true) {
			if (key < node->key) {
				if (node->left_offset == kInvalidOffset) {
					Offset left_offset = CreateNewNode(key, component_size);
					Node* new_node = GetNode(left_offset);
					node = GetNode(offset); // Update current node on buffer reallocation
					new_node->parent_offset = offset;
					node->left_offset = left_offset;
					Balance(left_offset);
					return new_node;
				}

				offset = node->left_offset;
				node = GetNode(offset);
				continue;
			}
			if (key > node->key) {
				if (node->right_offset == kInvalidOffset) {
					Offset right_offset = CreateNewNode(key, component_size);
					Node* new_node = GetNode(right_offset);
					node = GetNode(offset); // Update current node on buffer reallocation
					new_node->parent_offset = offset;
					node->right_offset = right_offset;
					Balance(right_offset);
					return new_node;
				}

				offset = node->right_offset;
				node = GetNode(offset);
				continue;
			}

			// key == node->key
			IReflectable* component = GetComponent(node);
			component->~IReflectable();
			return node;
		}
	}

	Offset CreateNewNode(TypeId key, size_t component_size) {
		size_t available = capacity_ - size_;
		size_t desired = sizeof(Node) + component_size;

		if (available < desired) {			
			void* temp = buffer_;
			capacity_ = size_ + desired; // TODO: amortization
			buffer_ = malloc(capacity_);
			memmove(buffer_, temp, size_);
			free(temp);			
		}
		
		Offset offset = size_;
		void* address = reinterpret_cast<char*>(buffer_) + offset;
		new (address) Node(key, component_size);
		size_ += desired;
		return offset;
	}

	void Balance(Offset offset) {
		while (offset != kInvalidOffset) {
			Node* temp = GetNode(offset);
			Node* parent = GetNode(temp->parent_offset);
			FixHeight(temp);
			int balance_factor = BalanceFactor(temp);

			if (balance_factor == 2) {
				if (BalanceFactor(GetNode(temp->right_offset)) < 0) {
					temp->right_offset = RotateRight(temp->right_offset);
				}

				if (parent == nullptr) {
					root_offset_ = RotateLeft(offset);
				} else if (parent->left_offset == offset){
					parent->left_offset = RotateLeft(offset);
				} else {
					parent->right_offset = RotateLeft(offset);
				}
			} else if (balance_factor == -2) {
				if (BalanceFactor(GetNode(temp->left_offset)) > 0) {
					temp->left_offset = RotateLeft(temp->left_offset);
				}

				if (parent == nullptr) {
					root_offset_ = RotateRight(offset);
				} else if (parent->left_offset == offset) {
					parent->left_offset = RotateRight(offset);
				} else {
					parent->right_offset = RotateRight(offset);
				}
			}

			offset = temp->parent_offset;
		}
	}

	int BalanceFactor(Node* node) {
		return Height(GetNode(node->right_offset)) - Height(GetNode(node->left_offset));
	}

	unsigned char Height(Node* node) {
		return node != nullptr ? node->height : 0;
	}

	Offset RotateRight(Offset offset) {
		Node* node = GetNode(offset);
		Node* left = GetNode(node->left_offset);
		Node* left_right = GetNode(left->right_offset);

		Offset temp = node->left_offset;
		left->parent_offset = node->parent_offset;
		node->left_offset = left->right_offset;

		if (left_right != nullptr) {
			left_right->parent_offset = offset;
		}
		
		left->right_offset = offset;
		node->parent_offset = temp;

		FixHeight(node);
		FixHeight(left);
		return temp;
	}

	Offset RotateLeft(Offset offset) {
		Node* node = GetNode(offset);
		Node* right = GetNode(node->right_offset);
		Node* right_left = GetNode(right->left_offset);

		Offset temp = node->right_offset;
		right->parent_offset = node->parent_offset;
		node->right_offset = right->left_offset;

		if (right_left != nullptr) {
			right_left->parent_offset = offset;
		}
		
		right->left_offset = offset;
		node->parent_offset = temp;

		FixHeight(node);
		FixHeight(right);
		return temp;
	}

	void FixHeight(Node* node) {
		unsigned char left_height = Height(GetNode(node->left_offset));
		unsigned char right_height = Height(GetNode(node->right_offset));
		node->height = left_height > right_height ? left_height : right_height;
		node->height += 1;
	}
};

} // namespace aoe