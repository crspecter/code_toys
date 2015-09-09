#ifndef __YDX_RBTREE_H__
#define __YDX_RBTREE_H__

namespace ydx
{
typedef unsigned long rbtree_key_t;
typedef long		  rbtree_key_int_t;

#define rbt_red(node)               ((node)->color_ = 1)
#define rbt_black(node)             ((node)->color_ = 0)
#define rbt_is_red(node)            ((node)->color_)
#define rbt_is_black(node)          (!rbt_is_red(node))
#define rbt_copy_color(n1, n2)      (n1->color_ = n2->color_)


#define rbtree_sentinel_init(node)  rbt_black(node)

#define rbtree_init(tree, s)					\
    rbtree_sentinel_init(s);					\
    (tree)->root = s;							\
    (tree)->sentinel = s;


struct rbtree_node_t
{
	unsigned char 	color_;
	rbtree_node_t 	*left;
	rbtree_node_t 	*right;
	rbtree_node_t 	*parent;
	rbtree_key_t 	key;
};

struct rbtree_t
{
	rbtree_node_t 	*root;
	rbtree_node_t 	*sentinel;
};

class RBtree
{
public:
	RBtree();
	~RBtree();
public:
	void 	rbtree_insert(rbtree_node_t *node);
	void 	rbtree_delete(rbtree_node_t *node);	


private:
	void 	rbtree_left_rotate(rbtree_node_t *node); //node and node->right
	void 	rbtree_right_rotate(rbtree_node_t *node); //node and node->left
	void 				rbtree_insert_value(rbtree_node_t *node);
	rbtree_node_t*		rbtree_min(rbtree_node_t *node);		//return the min key node
	
private:
	rbtree_t 		rbtree_;
	rbtree_node_t 	sentinel_;
};

}


#endif 