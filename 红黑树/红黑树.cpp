#include<iostream>
using namespace std;


template<typename T>
class RBTree
{
public:
	RBTree() :root_(nullptr) {}
	void insert(const T& val)
	{
		if (root_ == nullptr)
		{
			root_ = new Node(val);
			return;
		}
		Node* parent = nullptr;
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ > val)
			{
				parent = cur;
				cur = cur->left_;
			}
			else if (cur->data_ < val)
			{
				parent = cur;
				cur = cur->right_;
			}
			else
			{
				return;
			}
		}
		Node* node = new Node(val, parent, nullptr, nullptr, RED);
		if (parent->data_ > val)
		{
			parent->left_ = node;
		}
		else
		{
			parent->right_ = node;
		}
		if (RED == color(parent))
		{
			fixAfterInsert(node);
		}

	}
	void remove(const T& val)
	{
		if (root_ == nullptr)
		{
			return;
		}
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ > val)
			{
				cur = cur->left_;
			}
			else if (cur->data_ < val)
			{
				cur = cur->right_;
			}
			else
			{
				break;
			}
		}
		if (cur == nullptr)
		{
			return;
		}
		if (cur->left_ != nullptr && cur->right_ != nullptr)
		{
			Node* pre = cur->left_;
			while (pre->right_ != nullptr)
			{
				pre = pre->right_;
			}
			cur->data_ = pre->data_;
			cur = pre;
		}
		Node* child = cur->left_;
		if (child == nullptr)
		{
			child = cur->right_;
		}
		if (child != nullptr)
		{
			child->parent_ = cur->parent_;
			if (cur->parent_ == nullptr)
			{
				root_ = child;
			}
			else
			{
				if (cur->parent_->left_ == cur)
				{
					cur->parent_->left_ = child;
				}
				else
				{
					cur->parent_->right_ = child;
				}
			}
			Color c = color(cur);
			delete cur;
			if (c == BLACK)
			{
				fixAfterRemove(child);
			}
		}
		else
		{
			if (cur->parent_ == nullptr)
			{
				delete cur;
				root_ = nullptr;
				return;
			}
			else
			{
				if (color(cur) == BLACK)
				{
					fixAfterRemove(cur);
				}
				if (cur->parent_->left_ == cur)
				{
					cur->parent_->left_ = nullptr;
				}
				else
				{
					cur->parent_->right_ = nullptr;
				}
				delete cur;
			}
		}
	}
private:
	enum Color
	{
		BLACK,
		RED
	};
	struct Node
	{
		Node(T data = T(), Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr, Color color = BLACK)
			:data_(data), left_(left), right_(right), parent_(parent), color_(color) {
		}

		T data_;
		Node* left_;
		Node* right_;
		Node* parent_;
		Color color_;
	};
	Node* root_;
	Color color(Node* node)
	{
		return node == nullptr ? BLACK : node->color_;
	}
	void setColor(Node* node, Color color)
	{
		node->color_ = color;
	}
	Node* left(Node* node)
	{
		return node->left_;
	}
	Node* right(Node* node)
	{
		return node->right_;
	}
	Node* parent(Node* node)
	{
		return node->parent_;
	}
	void leftRotate(Node* node)
	{
		Node* child = node->right_;
		child->parent_ = node->parent_;
		if (node->parent_ == nullptr)
		{
			root_ = child;
		}
		else
		{
			if (node->parent_->left_ == node)
			{
				node->parent_->left_ = child;
			}
			else
			{
				node->parent_->right_ = child;
			}

		}
		node->right_ = child->left_;
		if (child->left_ != nullptr)
		{
			child->left_->parent_ = node;
		}
		child->left_ = node;
		node->parent_ = child;
	}
	void rightRotate(Node* node)
	{
		Node* child = node->left_;
		child->parent_ = node->parent_;
		if (node->parent_ == nullptr)
		{
			root_ = child;
		}
		else
		{
			if (node->parent_->left_ == node)
			{
				node->parent_->left_ = child;
			}
			else
			{
				node->parent_->right_ = child;
			}

		}
		node->left_ = child->right_;
		if (child->right_ != nullptr)
		{
			child->right_->parent_ = node;
		}
		child->right_ = node;
		node->parent_ = child;
	}
	void fixAfterInsert(Node* node)
	{
		//如果当前红色节点的父节点也是红色，继续调整
		while (color(parent(node)) == RED)
		{
			if (left(parent(parent(node))) == parent(node))
			{
				//插入的节点在左子树中
				Node* uncle = right(parent(parent(node)));
				if (RED == color(uncle))
				{
					setColor(parent(node), BLACK);
					setColor(uncle, BLACK);
					setColor(parent(parent(node)), RED);
					node = parent(parent(node));
				}
				else
				{
					if (right(parent(node)) == node)
					{
						node = parent(node);
						leftRotate(node);
					}
					setColor(parent(node), BLACK);
					setColor(parent(parent(node)), RED);
					rightRotate(parent(parent(node)));
					break;
				}
			}
			else
			{
				Node* uncle = left(parent(parent(node)));
				if (RED == color(uncle))
				{
					setColor(parent(node), BLACK);
					setColor(uncle, BLACK);
					setColor(parent(parent(node)), RED);
					node = parent(parent(node));
				}
				else
				{
					if (left(parent(node)) == node)
					{
						node = parent(node);
						rightRotate(node);
					}
					setColor(parent(node), BLACK);
					setColor(parent(parent(node)), RED);
					leftRotate(parent(parent(node)));
					break;
				}
			}
			setColor(root_, BLACK);
		}
	}
	void fixAfterRemove(Node* node)
	{
		while (color(node) == BLACK)
		{
			if (left(parent(node)) == node)
			{
				//删除的黑色节点在左子树
				Node* brother = right(parent(node));
				if (color(brother) == RED)
				{
					setColor(parent(node), RED);
					setColor(brother, BLACK);
					leftRotate(parent(node));
					brother = right(parent(node));
				}
				if (color(left(brother)) == BLACK && color(right(brother)) == BLACK)
				{
					setColor(brother, RED);
					node = parent(node);
				}
				else
				{
					if (color(right(brother)) != RED)
					{
						setColor(brother, RED);
						setColor(left(brother), BLACK);
						rightRotate(brother);
						brother = right(parent(node));
					}
					setColor(brother, color(parent(node)));
					setColor(parent(node), BLACK);
					setColor(right(brother), BLACK);
					leftRotate(parent(node));
					break;
				}
			}
			else
			{
				//删除的黑色节点在右子树
				Node* brother = left(parent(node));
				if (color(brother) == RED)
				{
					setColor(parent(node), RED);
					setColor(brother, BLACK);
					rightRotate(parent(node));
					brother = left(parent(node));
				}
				if (color(left(brother)) == BLACK && color(right(brother)) == BLACK)
				{
					setColor(brother, RED);
					node = parent(node);
				}
				else
				{
					if (color(left(brother)) != RED)
					{
						setColor(brother, RED);
						setColor(left(brother), BLACK);
						leftRotate(brother);
						brother = left(parent(node));
					}
					setColor(brother, color(parent(node)));
					setColor(parent(node), BLACK);
					setColor(left(brother), BLACK);
					rightRotate(parent(node));
					break;
				}
			}
		}
	}
};







int main()
{
	RBTree<int> rb;
	for (int i = 1; i <= 10; ++i)
	{
		rb.insert(i);
	}
	rb.remove(9);

	return 0;
}