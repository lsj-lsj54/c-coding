#include<iostream>
#include<stack>
#include<queue>
using namespace std;

template<typename T, typename Compare = less<T>>
class BSTree
{
public:
	BSTree() : root_(nullptr) {}
	~BSTree() {}

	void n_insert(const T& val)
	{
		if (root_ == nullptr)
		{
			root_ = new Node(val);
			return;
		}
		Node* cur = root_;
		Node* par = nullptr;
		while (cur != nullptr)
		{
			if (!compare(cur->data_, val))
			{
				par = cur;
				cur = cur->left_;
			}
			else if (compare(cur->data_, val))
			{
				par = cur;
				cur = cur->right_;
			}
			else
			{
				//已经存在了
				return;
			}
		}
		if (!compare(par->data_, val))
		{
			par->left_ = new Node(val);
		}
		else if (compare(par->data_, val))
		{
			par->right_ = new Node(val);
		}
	}
	void insert(const T& val)
	{
		root_ = insert(root_, val);
	}
	void n_remove(const T& val)
	{
		if (root_ == nullptr)
		{
			return;
		}
		Node* parent = nullptr;
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ == val)
			{
				break;
			}
			else if (!compare(cur->data_, val))
			{
				parent = cur;
				cur = cur->left_;
			}
			else if (compare(cur->data_, val))
			{
				parent = cur;
				cur = cur->right_;
			}

		}
		//没有找到待删除节点 
		if (cur == nullptr)
		{
			return;
		}
		//情况3
		if (cur->left_ != nullptr && cur->right_ != nullptr)
		{
			parent = cur;
			Node* pre = cur->left_;
			while (pre->right_ != nullptr)
			{
				parent = pre;
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
		if (parent == nullptr)//根节点恰好是要删除的
		{
			root_ = child;
		}
		else
		{
			if (parent->left_ == cur)
			{
				parent->left_ = child;
			}
			else
			{
				parent->right_ = child;
			}
		}
		delete cur;
	}
	void remove(const T& val)
	{
		root_ = remove(root_, val);
	}
	bool non_query(const T& val)
	{
		Node* cur = root_;
		while (cur != nullptr)
		{
			if (cur->data_ == val)
			{
				return true;
			}
			else if (!compare(cur->data_, val))
			{
				cur = cur->left_;
			}
			else if (compare(cur->data_, val))
			{
				cur = cur->right_;
			}
		}
		return false;
	}
	bool query(const T& val)
	{
		return query(root_, val) != nullptr;
	}
	void preOrder()
	{
		cout << "递归，前序遍历:";
		preOrder(root_);
		cout << endl;
	}
	void n_preOrder()
	{
		cout << "非递归前序遍历:";
		if (root_ == nullptr)
		{
			return;
		}
		stack<Node*>s;
		s.push(root_);
		while (!s.empty())
		{
			Node* top = s.top();
			s.pop();
			cout << top->data_ << "  ";
			if (top->right_ != nullptr)
			{
				s.push(top->right_);
			}
			if (top->left_ != nullptr)
			{
				s.push(top->left_);
			}
		}
		cout << endl;
	}
	void inOrder()
	{
		cout << "递归，中序遍历:";
		inOrder(root_);
		cout << endl;
	}
	void n_inOrder()
	{
		cout << "非递归中序遍历:";
		stack<Node*>s;
		Node* cur = root_;
		while (cur != nullptr)
		{
			s.push(cur);
			cur = cur->left_;
		}
		while (!s.empty())
		{
			Node* top = s.top();
			s.pop();

			cout << top->data_ << "  ";

			cur = top->right_;
			while (cur != nullptr)
			{
				s.push(cur);
				cur = cur->left_;
			}
		}
		cout << endl;
	}
	void postOrder()
	{
		cout << "递归，后序遍历:";
		postOrder(root_);
		cout << endl;
	}
	void n_postOrder()
	{
		cout << "非递归后序遍历:";
		if (root_ == nullptr)
		{
			return;
		}
		stack<Node*>s1;
		stack<Node*>s2;
		s1.push(root_);
		while (!s1.empty())
		{
			Node* top = s1.top();
			s1.pop();

			s2.push(top);
			if (top->left_ != nullptr)
			{
				s1.push(top->left_);
			}
			if (top->right_ != nullptr)
			{
				s1.push(top->right_);
			}
		}
		while (!s2.empty())
		{
			cout << s2.top()->data_ << "  ";
			s2.pop();
		}
		cout << endl;
	}
	int high()
	{
		return high(root_);
	}
	int number()
	{
		return number(root_);
	}
	void levelOrder()
	{
		cout << "递归，层序遍历:";
		int h = high();
		for (int i = 0; i < h; ++i)
		{
			levelOrder(root_, i);
		}
		cout << endl;
	}
	void n_levelOrder()
	{
		cout << "非递归层序遍历:";
		queue<Node*> q;
		q.push(root_);

		while (!q.empty())
		{
			Node* t = q.front();
			q.pop();
			cout << t->data_ << "  ";
			if (t->left_ != nullptr)
			{
				q.push(t->left_);
			}
			if (t->right_ != nullptr)
			{
				q.push(t->right_);
			}
		}
		cout << endl;
	}
private:
	struct Node
	{
		Node(T data = T())
			:data_(data)
			, left_(nullptr)
			, right_(nullptr)
		{
		}

		T data_;
		Node* left_;
		Node* right_;
	};
	Node* root_;
	Compare compare;

	void preOrder(Node* cur)
	{
		if (cur == nullptr)
		{
			return;
		}
		cout << cur->data_ << "  ";
		preOrder(cur->left_);
		preOrder(cur->right_);
	}
	void inOrder(Node* cur)
	{
		if (cur == nullptr)
		{
			return;
		}
		inOrder(cur->left_);
		cout << cur->data_ << "  ";
		inOrder(cur->right_);

	}
	void postOrder(Node* cur)
	{
		if (cur == nullptr)
		{
			return;
		}
		postOrder(cur->left_);
		postOrder(cur->right_);
		cout << cur->data_ << "  ";
	}
	int high(Node* node)
	{
		if (node == nullptr)
		{
			return 0;
		}
		int left = high(node->left_);
		int right = high(node->right_);
		return left > right ? left + 1 : right + 1;
	}
	int number(Node* node)
	{
		if (node == nullptr)
		{
			return 0;
		}
		return number(node->left_) + number(node->right_) + 1;
	}
	void levelOrder(Node* cur, int i)
	{
		if (cur == nullptr)
		{
			return;
		}
		if (i == 0)
		{
			cout << cur->data_ << "  ";
			return;
		}
		levelOrder(cur->left_, i - 1);
		levelOrder(cur->right_, i - 1);
	}
	Node* insert(Node* node, const T& val)
	{
		if (node == nullptr)
		{
			return new Node(val);
		}
		if (node->data == val)
		{
			return node;
		}
		else if (compare(node->data_, val))
		{
			node->right_ = insert(node->right_, val);
		}
		else
		{
			node->left_ = insert(node->left_, val);
		}
		return node;
	}
	Node* remove(Node* node, const T& val)
	{
		if (node == nullptr)
		{
			return nullptr;
		}
		if (node->data_ == val)
		{
			if (node->left_ != nullptr && node->right_ != nullptr)
			{
				Node* pre = node->left_;
				while (pre->right_ != nullptr)
				{
					pre = pre->right_;
				}
				node->data_ = pre->data_;
				node->left_ = remove(node->left_, pre->data_);
			}
			else
			{
				if (node->left_ != nullptr)
				{
					Node* left = node->left_;
					delete node;
					return left;
				}
				else if (node->right_ != nullptr)
				{
					Node* right = node->right_;
					delete node;
					return right;
				}
				else
				{
					delete node;
					return nullptr;
				}
			}
		}

		else if (compare(node->data_, val))
		{
			node->right_ = remove(node->right_, val);
		}
		else
		{
			node->left_ = remove(node->left_, val);
		}
		return node;
	}
	Node* query(Node* node, const T& val)
	{
		if (node == nullptr)
		{
			return nullptr;
		}
		if (node->data_ == val)
		{
			return node;
		}
		else if (compare(node->data_, val))
		{
			return query(node->right_, val);
		}
		else
		{
			return query(node->left_, val);
		}
	}
};

int main()
{
	BSTree<int> t;
	t.n_insert(12);
	for (int i = 1; i <= 5; ++i)
	{
		t.n_insert(i);
	}
	for (int i = 20; i <= 25; ++i)
	{
		t.n_insert(i);
	}

	t.preOrder();
	t.n_preOrder();
	t.inOrder();
	t.n_inOrder();
	t.postOrder();
	t.n_postOrder();
	t.levelOrder();
	t.n_levelOrder();
	cout << "--------------------------------------------------------" << endl;
	BSTree<int> t1;
	t1.n_insert(12);
	for (int i = 1; i <= 5; ++i)
	{
		t1.n_insert(i);
	}
	for (int i = 20; i <= 25; ++i)
	{
		t1.n_insert(i);
	}
	t1.remove(24);
	t1.preOrder();
	t1.inOrder();
	t1.postOrder();
	t1.levelOrder();
	return 0;
}

