#pragma once

#include <string>
#include <vector>

#include "glm.hpp"

/// <summary>
/// A node in the hiearchy, used for generating animations.
/// </summary>
struct Node
{
	/// <summary>
	/// The list of children, which may be empty but should not be null.
	/// </summary>
	std::vector<Node*> children;

	/// <summary>
	/// The name of the node. The name might be empty (length of zero) but all
	/// nodes which need to be referenced by either bones or animations are
	/// named. Multiple nodes may have the same name, except for nodes which
	/// are referenced by bones. Their names must be unique.
	///
	/// Cameras and lights reference a specific node by name - if there are
	/// multiple nodes with this name, they are assigned to each of them.
	/// 
	/// There are no limitations with regard to the characters contained in the
	/// name string as it is usually taken directly from the source file.
	/// 
	/// Implementations should be able to handle tokens such as whitespace,
	/// tabs, line feeds, quotation marks, ampersands etc.
	/// 
	/// Sometimes assimp introduces new nodes not present in the source file
	/// into the hierarchy (usually out of necessity because sometimes the
	/// source hierarchy format is simply not compatible). Their names are
	/// surrounded by &lt;&gt;, e.g. &lt;DummyRootNode&gt;.
	/// 
	/// </summary>
	const std::string name;

	/// <summary>
	/// The parent node, nullptr if this is the root node.
	/// </summary>
	const Node* parent;

	/// <summary>
	/// The transformation of this node, relative to the node's parent.
	/// </summary>
	const glm::mat4 node_transformation;

	/// <summary>
	/// Create a new node.
	/// </summary>
	/// <param name="name">The name of the node.</param>
	/// <param name="parent">This node's parent, if applicable.</param>
	/// <param name="node_transformation">The transformation matrix for this
	/// node, relative to the parent.</param>
	Node(std::string name, Node* parent, const glm::mat4& node_transformation);

	Node(const Node&) = delete;
	Node& operator=(const Node&) = delete;
	~Node();

	/// <summary>
	/// Add a child to this node. This node is now considered the owner of 
	/// the provided pointer.
	/// </summary>
	/// <param name="child">The child to add to this node.</param>
	void add_child(Node* child);
};