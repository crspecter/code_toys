#include <stdlib.h>
#include "ydx_rbtree.h"

using namespace ydx;

RBtree::RBtree()
{
	rbtree_init(&rbtree_, &sentinel_);
}

RBtree::~RBtree()
{

}

rbtree_node_t* RBtree::rbtree_min(rbtree_node_t * node)
{
	while(node->left != &sentinel_)
	{
		node = node->left;
	}
	return node;
}

void RBtree::rbtree_insert(rbtree_node_t *node)
{
    rbtree_node_t  **root, *uncle/*叔叔结点*/, *sentinel;

    /* a binary tree insert */

    root = &rbtree_.root;
    sentinel = rbtree_.sentinel;

    if (*root == sentinel)
    {
        node->parent = NULL;
        node->left = sentinel;
        node->right = sentinel;
        rbt_black(node);
        *root = node;
        return;
    }

	rbtree_insert_value(node);
	/* re-balance tree */
	//循环为向上一级回溯，(不超过2次),直到node回溯到root或者node的父为黑色,显然只有3层树之后才会发生
	while(node != *root && rbt_is_red(node->parent))
	{
		//第一步判断是左方还是右方插入
		//左插,父结点为祖父的左子
		if(node->parent == node->parent->parent->left)
		{
			uncle = node->parent->parent->right;
			//s1.红父，红叔.将父和叔变黑，祖父变红，继续检查祖父结点的父结点
			if(rbt_is_red(uncle))
			{
				rbt_black(node->parent);
				rbt_black(uncle);
				rbt_red(node->parent->parent);
				node = node->parent->parent;
			}
			//s2黑叔
			else 
			{
				//判断是内x，外x。内插的情况需要做2次旋转，外x只需要做一次旋转
				//s2.1内x，做LR 2次旋转
				if(node == node->parent->right)
				{
					node = node->parent;
					rbtree_left_rotate(node);//旋转后node位于中间，父结点变为其左子
				}
				rbt_black(node->parent);
				rbt_is_red(node->parent->parent);
				rbtree_right_rotate(node->parent->parent);//右旋原祖父结点和node，之后node位于顶层
			}

		}
		//右插，方法为左插的镜像处理，操作相同，旋转方向相反
		else
		{
			uncle = node->parent->parent->left;
			if(rbt_is_red(uncle))
			{
				rbt_black(node->parent);
				rbt_black(uncle);
				rbt_red(node->parent->parent);
				node = node->parent->parent;
			}	
			else
			{
				if(node == node->parent->left)
				{
					node = node->parent;
					rbtree_right_rotate(node);
				}
				//node此时为入参node的父，右旋后到底层，原node在中间层
				//将原node变黑，原祖父变红后进行一次左旋
				rbt_black(node->parent);
				rbt_is_red(node->parent->parent);	
				rbtree_left_rotate(node->parent->parent);
			}
			
		}
	}
	//根节点始终为黑色
	rbt_black(*root);
}


//s1.当找到结点应该插入的位置,s2完成将结点数据加入到树中
void RBtree::rbtree_insert_value(rbtree_node_t * node)
{
	rbtree_node_t *ins_ptr = rbtree_.root;
	rbtree_node_t *sentinel = rbtree_.sentinel;
	rbtree_node_t **p;
	//搜索插入点
	for(;;)
	{
		if((rbtree_key_int_t)(node->key - ins_ptr->key) < 0)
		{
			p = &ins_ptr->left;
		}
		else
		{
			p = &ins_ptr->right;
		}
		if( *p == sentinel)
			break;
		ins_ptr = *p;
	}
	//插入元素
	//p此时指向ins_ptr即父结点的left或者right,*p = node可以修改父结点的left或者right指针
	//指向的地址
	*p = node;
	/////////////////////
	node->parent = ins_ptr;
	node->left = sentinel;
	node->right = sentinel;
	rbt_red(node);
}

void RBtree::rbtree_delete(rbtree_node_t * node)
{
	/*红黑树的删除步骤和普通二叉树相同
	1.当删除结点没有子树，直接将父结点的对应指针指向sentinel，然后删除
	2.当删除结点有一个子树，将父结点的对应指针指向唯一的子树根，然后删除
	3.当删除结点有2个子树时，找到(delete node)左子树最大结点，或者右子树最小结点，代替删除结点
	的位置，然后删除delete node*/
	/*
	1、 删除操作中真正被删除的必定是只有一个红色孩子或没有孩子的结点。
	2、 如果真正的删除点是一个红色结点，那么它必定是一个叶子结点。
	*/
	unsigned char is_red;
	rbtree_node_t **root, *sentinel, *subst, *node_sub, *w;

	root = &rbtree_.root;
	sentinel = rbtree_.sentinel;

	//s1 左子树为空
	if(node->left == sentinel)
	{
		//此时node_sub可能是sentinel
		node_sub = node->right;
		subst = node;
	}
	//s2 右子树为空
	else if(node->right == sentinel)
	{
		node_sub = node->left;
		subst = node;
	}
	//s3 都不为空
	else
	{
		//找右子树最小结点，该结点肯定至多一个子树
		subst = rbtree_min(node->right);
		if(subst->left != sentinel)
		{
			node_sub = subst->left
		}
		else
		{
			//此时node_sub可能是sentinel
			node_sub = subst->right
		}
	}

	//当被删除结点是根，直接删除，并改变新的根结点
	if(subst == *root)
	{
		*root = node_sub;
		rbt_black(node_sub);
		//删除结点
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
		node->key = 0;

		return;
	}

	is_red = rbt_is_red(subst);

	//当被删除结点是父的左子
	if(subst == subst->parent->left)
	{
		subst->parent->left = node_sub;
	}
	//当被删除结点是父的右子
	else
	{
		subst->parent->right = node_sub;
	}

	//当subst没有做移动，即node结点至少有一个子树空的时候(subst不会是根，为根的情况已经处理)
	if(subst == node)
	{
		node_sub->parent = subst->parent;
	}
	//subst移动过，subst是替换node的结点
	else
	{
		if(subst->parent == node)
		{
			//调整结束时node_sub->parent会指向subst，parent(node) subst node_sub在相邻的三层
			node_sub->parent = subst;
		}
		else
		{
			//调整结束时node_sub->parent不指向subst，node到subst的深度大于1
			node_sub->parent = subst->parent;
		}
		subst->left = node->left;
		subst->right = node->right;
		subst->parent = node->parent;
		rbt_copy_color(subst, node);
        if (node == *root)
        {
            *root = subst;
        }		
		else
		{
            if (node == node->parent->left)
            {
                node->parent->left = subst;
            }
            else
            {
                node->parent->right = subst;
            }			
		}
		
        if (subst->left != sentinel)
        {
            subst->left->parent = subst;
        }

        if (subst->right != sentinel)
        {
            subst->right->parent = subst;
        }
	}
	//删除结点
	node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;
	//红色结点直接删除
    if (is_red)
    {
        return;
    }	
	
	/*删除后，需要恢复红黑树的性质*/
	    /* a delete fixup */
		//红黑树此时有4中可能，下列程序用s1234解释
    while (node_sub != *root && rbt_is_black(node_sub))
    {
    	//判断node_sub是在左方还是右方
    	//node_sub在左方
    	if(node_sub == node_sub->parent->left)
    	{
			w = node_sub->parent->right;
			//s1.w为红色，涂黑w，涂红父结点，左旋，变为其余3情况之一
			if(rbt_is_red(w))
			{
				rbt_black(w);
				rbt_red(node_sub->parent);     //w为红，父结点必是黑色
				rbtree_left_rotate(node_sub->parent);
				w = node_sub->parent->right; //调整w为旋转后的node_sub的兄弟
			}
			//s2.w为黑色，有2个孩子均为黑色，涂红w，将分析结点往树上层提升一级，指向父结点
			if (rbt_is_black(w->left) && rbt_is_black(w->right))
			{
				rbt_red(w);
				node_sub = node->parent;
			}
			else
			{
				//s3.w的有孩子为黑色，左为红色，涂黑左孩子，涂红w，右旋一次，转换为s4
                if (rbt_is_black(w->right))
                {
                    rbt_black(w->left);
                    rbt_red(w);
                    rbtree_right_rotate(w);
                    w = node_sub->parent->right; //移动w到当前node_sub的兄弟结点
                }
				//s4.w有右孩子红色，这种情况可以结束求解过程，前面3中情况最终会转化为这种情况
                rbt_copy_color(w, node_sub->parent);
                rbt_black(node_sub->parent);
                rbt_black(w->right);
                rbtree_left_rotate(node_sub->parent);
                node_sub = *root;				
			}
		}
		//在node_sub右边,操作为上面操作的镜像
		else
		{
			w = node_sub->parent->left;
            if (rbt_is_red(w))
            {
                rbt_black(w);
                rbt_red(node_sub->parent);
                rbtree_right_rotate(node_sub->parent);
                w = node_sub->parent->left;
            }
            if (rbt_is_black(w->left) && rbt_is_black(w->right))
            {
                rbt_red(w);
                node_sub = node_sub->parent;
            }
            else
            {
                if (rbt_is_black(w->left))
                {
                    rbt_black(w->right);
                    rbt_red(w);
                    rbtree_left_rotate(w);
                    w = node_sub->parent->left;
                }

                rbt_copy_color(w, node_sub->parent);
                rbt_black(node_sub->parent);
                rbt_black(w->left);
                rbtree_right_rotate(node_sub->parent);
                node_sub = *root;
            }
		}
    }
}

void RBtree::rbtree_left_rotate(rbtree_node_t * node)
{
	//完成node和它的右子x_right结点的左旋
	
	rbtree_node_t *x_right;
	rbtree_node_t **root = &rbtree_.root;
	rbtree_node_t *sentinel = &sentinel_;

	x_right = node->right;
	node->right = x_right->left;

	//sentinel结点不真正的在树中，只是每个叶子结点的左右子均指向sentinel
	if(x_right->left != sentinel)
	{
		x_right->left->parent = node;
	}

	//根节点存在父结点，父结点为NULL		
	x_right->parent = node->parent;
	//case1 node 为根
	if(node == *root)
	{
		*root = x_right;
	}
	//case2 node 为父左子
	else if(node == node->parent->left)
	{
		node->parent->left = x_right;
	}
	//case3 node 为父右子
	else
	{
		node->parent->right = x_right;
	}
	//最后修改node和右子的位置关系
	x_right->left = node;
	node->parent = x_right;
	
}

void RBtree::rbtree_right_rotate(rbtree_node_t * node)
{
	//完成node和它的左子x_left结点的右旋
	
	rbtree_node_t *x_left;
	rbtree_node_t **root = &rbtree_.root;
	rbtree_node_t *sentinel = &sentinel_;	

	x_left = node->left;
	node->left = x_left->right;
	if(x_left->right != sentinel)
	{
		x_left->right->parent = node;
	}
	
	x_left->parent = node->parent;

	if(node == *root)
	{
		*root = x_left;
	}
	else if(node == node->parent->left)
	{
		node->parent->left = x_left;
	}
	else
	{
		node->parent->right = x_left;
	}
	
	x_left->right = node;
	node->parent = x_left;
	
}
