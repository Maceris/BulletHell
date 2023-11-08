#include "Models/Node.h"

Node::Node(std::string name, Node* parent, 
	const glm::mat4& node_transformation)
	: name{ name }
	, parent{ parent }
	, node_transformation{ node_transformation }
{}

Node::~Node()
{
	for (int i = 0; i < children.size(); ++i)
	{
		Node* child = children[i];
		if (child != nullptr)
		{
			delete child;
			children[i] = nullptr;
		}
	}
	children.clear();
}

void Node::add_child(Node* child)
{
	if (child == nullptr)
	{
		return;
	}
	children.push_back(child);
}