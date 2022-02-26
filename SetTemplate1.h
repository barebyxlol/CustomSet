#include <cstddef>
#include <algorithm>


template<class T>
class Set {
private:
    struct node {
        T key;
        size_t height;
        node *left;
        node *right;
        node *parent;
        bool is_end;

        node(T k, node *par) {
            key = k;
            left = nullptr;
            right = nullptr;
            parent = par;
            is_end = false;
            height = 1;
        }

        explicit node(node *par) {
            left = nullptr;
            right = nullptr;
            parent = par;
            is_end = true;
            height = 1;
        }

    };

    size_t GetHeight(node *v) const {
        if (v != nullptr) {
            return v->height;
        }
        return 0;
    }

    int GetBalance(node *v) const {
        if (v != nullptr) {
            return GetHeight(v->left) - GetHeight(v->right);
        }
        return 0;
    }

    void FixHeight(node *v) {
        if (v != nullptr) {
            v->height = std::max(GetHeight(v->left), GetHeight(v->right)) + 1;
        }
    }

    node *RightRotation(node *v) {
        node *q = v->left;
        v->left = q->right;
        if (v->left != nullptr) {
            v->left->parent = v;
        }
        q->right = v;
        q->parent = v->parent;
        v->parent = q;
        FixHeight(v);
        FixHeight(q);
        return q;
    }

    node *LeftRotation(node *v) {
        node *q = v->right;
        v->right = q->left;
        if (v->right != nullptr) {
            v->right->parent = v;
        }
        q->left = v;
        q->parent = v->parent;
        v->parent = q;
        FixHeight(v);
        FixHeight(q);
        return q;
    }

    node *FixBalance(node *v) {
        if (v == nullptr) {
            return nullptr;
        }
        FixHeight(v);
        if (GetBalance(v) == -2) {
            if (GetBalance(v->right) > 0) {
                v->right = RightRotation(v->right);
            }
            v = LeftRotation(v);
            return v;
        }
        if (GetBalance(v) == 2) {
            if (GetBalance(v->left) < 0) {
                v->left = LeftRotation(v->left);
            }
            v = RightRotation(v);
            return v;
        }
        return v;
    }

    node *Insert(node *v, const T &k, node *par) {
        if (v == nullptr) {
            return new node(k, par);
        }
        if (v->is_end || k < v->key) {
            v->left = Insert(v->left, k, v);
        } else {
            v->right = Insert(v->right, k, v);
        }
        v = FixBalance(v);
        return v;
    }

    node *FindMin(node *v) const {
        if (v->left == nullptr) {
            return v;
        } else {
            return FindMin(v->left);
        }
    }

    node *FindEnd(node *v) const {
        if (v->right == nullptr) {
            return v;
        } else {
            return FindEnd(v->right);
        }
    }

    node *EraseMin(node *v) {
        if (v->left == nullptr) {
            if (v->right != nullptr) {
                v->right->parent = v->parent;
            }
            return v->right;
        }
        v->left = EraseMin(v->left);
        v = FixBalance(v);
        return v;
    }

    node *Erase(node *v, const T &k) {
        if (v == nullptr) {
            return nullptr;
        }
        if (v->is_end || k < v->key) {
            v->left = Erase(v->left, k);
        } else if (v->key < k) {
            v->right = Erase(v->right, k);
        } else {
            node *l = v->left;
            node *r = v->right;
            if (r == nullptr) {
                if (l != nullptr) {
                    l->parent = v->parent;
                }
                delete v;
                return l;
            }
            node *minn = FindMin(r);
            minn->right = EraseMin(r);
            minn->parent = v->parent;
            delete v;
            minn->left = l;
            if (minn->left != nullptr) {
                minn->left->parent = minn;
            }
            if (minn->right != nullptr) {
                minn->right->parent = minn;
            }
            minn = FixBalance(minn);
            return minn;
        }
        v = FixBalance(v);
        return v;
    }

    node *Find(node *v, const T &k) const {
        if (v == nullptr) {
            return nullptr;
        }
        if (v->is_end) {
            if (v->left != nullptr && !(v->left->key < k) && !(k < v->left->key)) {
                return v->left;
            } else {
                return nullptr;
            }
        }
        if (k < v->key) {
            return Find(v->left, k);
        } else if (v->key < k) {
            return Find(v->right, k);
        }
        return v;

    }

    node *LowerBound(node *v, node *par, const T &k) const {
        if (v == nullptr) {
            return par;
        }
        if (v->is_end) {
            if (v->left != nullptr && !(v->left->key < k)) {
                return v->left;
            }
            return v;
        }
        if (k < v->key) {
            return LowerBound(v->left, v, k);
        } else if (v->key < k) {
            return LowerBound(v->right, v, k);
        }
        return v;
    }

    void DestroySet(node *v) {
        if (v == nullptr) {
            delete v;
            return;
        }
        DestroySet(v->left);
        DestroySet(v->right);
        delete v;
    }

    node *CopyNode(node *v, node *par) {
        if (v == nullptr) {
            return nullptr;
        }
        if (v->is_end) {
            node *n = new node(par);
            n->left = CopyNode(v->left, n);
            return n;
        }
        node *n = new node(v->key, par);
        n->left = CopyNode(v->left, n);
        n->right = CopyNode(v->right, n);
        return n;
    }

public:

    class iterator {
    public:
        iterator() = default;

        explicit iterator(node *v) : it(v) {}

        iterator(const iterator &iter) : it(iter.it) {}

        iterator &operator=(const iterator &iter) {
            if (this == &iter) {
                return *this;
            }
            it = iter.it;
            return *this;
        }

        bool operator==(const iterator &iter) const {
            return it == iter.it;
        }

        bool operator!=(const iterator &iter) const {
            return it != iter.it;
        }

        iterator &operator++() {
            if (it->is_end) {
                return *this;
            }
            if (it->right != nullptr) {
                it = it->right;
                while (it->left != nullptr) {
                    it = it->left;
                }
                return *this;
            }
            T t = it->key;
            while (!it->is_end && !(t < it->key) && it->parent != nullptr) {
                it = it->parent;
            }
            return *this;
        }

        iterator &operator--() {
            if (it->is_end) {
                if (it->left != nullptr) {
                    it = it->left;
                } else {
                    it = it->parent;
                }
                return *this;
            }
            if (it->left != nullptr) {
                it = it->left;
                while (it->right != nullptr) {
                    it = it->right;
                }
                return *this;
            }
            T t = it->key;
            if (it->parent == nullptr) {
                return *this;
            }
            if (it->parent->is_end) {
                if (it->parent->parent == nullptr) {
                    return *this;
                }
                it = it->parent->parent;
            }
            while (!(it->key < t) && it->parent != nullptr) {
                it = it->parent;
            }
            return *this;
        }

        iterator &operator++(int) {
            if (it->is_end) {
                return *this;
            }
            if (it->right != nullptr) {
                it = it->right;
                while (it->left != nullptr) {
                    it = it->left;
                }
                return *this;
            }
            T t = it->key;
            while (!(t < it->key) && it->parent != nullptr) {
                it = it->parent;
            }
            return *this;
        }

        iterator &operator--(int) {
            if (it->is_end) {
                if (it->left != nullptr) {
                    it = it->left;
                } else {
                    it = it->parent;
                }
                return *this;
            }
            if (it->left != nullptr) {
                it = it->left;
                while (it->right != nullptr) {
                    it = it->right;
                }
                return *this;
            }
            T t = it->key;
            if (it->parent == nullptr) {
                return *this;
            }
            if (it->parent->is_end) {
                if (it->parent->parent == nullptr) {
                    return *this;
                }
                it = it->parent->parent;
            }
            while (!(it->key < t) && it->parent != nullptr) {
                it = it->parent;
            }
            return *this;
        }

        T operator*() const {
            return it->key;
        }

        const T *operator->() const {
            return &(it->key);
        }

    private:
        const node *it;
    };

    Set() {
        root_ = new node(nullptr);
        size_ = 0;
        end_ = root_;
    }

    void insert(const T &k) {
        if (Find(root_, k) == nullptr) {
            ++size_;
            root_ = Insert(root_, k, nullptr);
        }
    }

    template<typename Iterator>
    Set(Iterator beginit, Iterator endit) {
        size_ = 0;
        root_ = new node(nullptr);
        end_ = root_;
        std::for_each(beginit, endit, [this](const T &k) { (*this).insert(k); });
    }

    Set(std::initializer_list<T> lst) {
        size_ = 0;
        root_ = new node(nullptr);
        end_ = root_;
        std::for_each(lst.begin(), lst.end(), [this](const T &k) { (*this).insert(k); });
    }

    Set(const Set &st) {
        size_ = st.size_;
        root_ = CopyNode(st.root_, nullptr);
        end_ = FindEnd(root_);
    }

    Set &operator=(const Set &st) {
        if (this == &st) {
            return *this;
        }
        DestroySet(root_);
        size_ = st.size_;
        root_ = CopyNode(st.root_, nullptr);
        end_ = FindEnd(root_);
        return *this;
    }

    ~Set() {
        DestroySet(root_);
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    iterator find(T k) const {
        node *v = Find(root_, k);
        if (v == nullptr) {
            return iterator(end_);
        }
        return iterator(v);
    }

    void erase(T k) {
        if (Find(root_, k) != nullptr) {
            --size_;
            root_ = Erase(root_, k);
        }
    }

    iterator begin() const {
        return iterator(FindMin(root_));
    }

    iterator end() const {
        return iterator(end_);
    }

    iterator lower_bound(const T &k) const {
        auto iter = iterator(LowerBound(root_, nullptr, k));
        auto e = iterator(end_);
        while (iter != e && *iter < k) {
            ++iter;
        }
        return iter;
    }

private:
    node *root_;
    size_t size_;
    node *end_;
};
