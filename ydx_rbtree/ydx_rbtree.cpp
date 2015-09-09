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
    rbtree_node_t  **root, *uncle/*������*/, *sentinel;

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
	//ѭ��Ϊ����һ�����ݣ�(������2��),ֱ��node���ݵ�root����node�ĸ�Ϊ��ɫ,��Ȼֻ��3����֮��Żᷢ��
	while(node != *root && rbt_is_red(node->parent))
	{
		//��һ���ж����󷽻����ҷ�����
		//���,�����Ϊ�游������
		if(node->parent == node->parent->parent->left)
		{
			uncle = node->parent->parent->right;
			//s1.�츸������.���������ڣ��游��죬��������游���ĸ����
			if(rbt_is_red(uncle))
			{
				rbt_black(node->parent);
				rbt_black(uncle);
				rbt_red(node->parent->parent);
				node = node->parent->parent;
			}
			//s2����
			else 
			{
				//�ж�����x����x���ڲ�������Ҫ��2����ת����xֻ��Ҫ��һ����ת
				//s2.1��x����LR 2����ת
				if(node == node->parent->right)
				{
					node = node->parent;
					rbtree_left_rotate(node);//��ת��nodeλ���м䣬������Ϊ������
				}
				rbt_black(node->parent);
				rbt_is_red(node->parent->parent);
				rbtree_right_rotate(node->parent->parent);//����ԭ�游����node��֮��nodeλ�ڶ���
			}

		}
		//�Ҳ壬����Ϊ���ľ�����������ͬ����ת�����෴
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
				//node��ʱΪ���node�ĸ��������󵽵ײ㣬ԭnode���м��
				//��ԭnode��ڣ�ԭ�游�������һ������
				rbt_black(node->parent);
				rbt_is_red(node->parent->parent);	
				rbtree_left_rotate(node->parent->parent);
			}
			
		}
	}
	//���ڵ�ʼ��Ϊ��ɫ
	rbt_black(*root);
}


//s1.���ҵ����Ӧ�ò����λ��,s2��ɽ�������ݼ��뵽����
void RBtree::rbtree_insert_value(rbtree_node_t * node)
{
	rbtree_node_t *ins_ptr = rbtree_.root;
	rbtree_node_t *sentinel = rbtree_.sentinel;
	rbtree_node_t **p;
	//���������
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
	//����Ԫ��
	//p��ʱָ��ins_ptr��������left����right,*p = node�����޸ĸ�����left����rightָ��
	//ָ��ĵ�ַ
	*p = node;
	/////////////////////
	node->parent = ins_ptr;
	node->left = sentinel;
	node->right = sentinel;
	rbt_red(node);
}

void RBtree::rbtree_delete(rbtree_node_t * node)
{
	/*�������ɾ���������ͨ��������ͬ
	1.��ɾ�����û��������ֱ�ӽ������Ķ�Ӧָ��ָ��sentinel��Ȼ��ɾ��
	2.��ɾ�������һ���������������Ķ�Ӧָ��ָ��Ψһ����������Ȼ��ɾ��
	3.��ɾ�������2������ʱ���ҵ�(delete node)����������㣬������������С��㣬����ɾ�����
	��λ�ã�Ȼ��ɾ��delete node*/
	/*
	1�� ɾ��������������ɾ���ıض���ֻ��һ����ɫ���ӻ�û�к��ӵĽ�㡣
	2�� ���������ɾ������һ����ɫ��㣬��ô���ض���һ��Ҷ�ӽ�㡣
	*/
	unsigned char is_red;
	rbtree_node_t **root, *sentinel, *subst, *node_sub, *w;

	root = &rbtree_.root;
	sentinel = rbtree_.sentinel;

	//s1 ������Ϊ��
	if(node->left == sentinel)
	{
		//��ʱnode_sub������sentinel
		node_sub = node->right;
		subst = node;
	}
	//s2 ������Ϊ��
	else if(node->right == sentinel)
	{
		node_sub = node->left;
		subst = node;
	}
	//s3 ����Ϊ��
	else
	{
		//����������С��㣬�ý��϶�����һ������
		subst = rbtree_min(node->right);
		if(subst->left != sentinel)
		{
			node_sub = subst->left
		}
		else
		{
			//��ʱnode_sub������sentinel
			node_sub = subst->right
		}
	}

	//����ɾ������Ǹ���ֱ��ɾ�������ı��µĸ����
	if(subst == *root)
	{
		*root = node_sub;
		rbt_black(node_sub);
		//ɾ�����
		node->left = NULL;
		node->right = NULL;
		node->parent = NULL;
		node->key = 0;

		return;
	}

	is_red = rbt_is_red(subst);

	//����ɾ������Ǹ�������
	if(subst == subst->parent->left)
	{
		subst->parent->left = node_sub;
	}
	//����ɾ������Ǹ�������
	else
	{
		subst->parent->right = node_sub;
	}

	//��substû�����ƶ�����node���������һ�������յ�ʱ��(subst�����Ǹ���Ϊ��������Ѿ�����)
	if(subst == node)
	{
		node_sub->parent = subst->parent;
	}
	//subst�ƶ�����subst���滻node�Ľ��
	else
	{
		if(subst->parent == node)
		{
			//��������ʱnode_sub->parent��ָ��subst��parent(node) subst node_sub�����ڵ�����
			node_sub->parent = subst;
		}
		else
		{
			//��������ʱnode_sub->parent��ָ��subst��node��subst����ȴ���1
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
	//ɾ�����
	node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->key = 0;
	//��ɫ���ֱ��ɾ��
    if (is_red)
    {
        return;
    }	
	
	/*ɾ������Ҫ�ָ������������*/
	    /* a delete fixup */
		//�������ʱ��4�п��ܣ����г�����s1234����
    while (node_sub != *root && rbt_is_black(node_sub))
    {
    	//�ж�node_sub�����󷽻����ҷ�
    	//node_sub����
    	if(node_sub == node_sub->parent->left)
    	{
			w = node_sub->parent->right;
			//s1.wΪ��ɫ��Ϳ��w��Ϳ�츸��㣬��������Ϊ����3���֮һ
			if(rbt_is_red(w))
			{
				rbt_black(w);
				rbt_red(node_sub->parent);     //wΪ�죬�������Ǻ�ɫ
				rbtree_left_rotate(node_sub->parent);
				w = node_sub->parent->right; //����wΪ��ת���node_sub���ֵ�
			}
			//s2.wΪ��ɫ����2�����Ӿ�Ϊ��ɫ��Ϳ��w����������������ϲ�����һ����ָ�򸸽��
			if (rbt_is_black(w->left) && rbt_is_black(w->right))
			{
				rbt_red(w);
				node_sub = node->parent;
			}
			else
			{
				//s3.w���к���Ϊ��ɫ����Ϊ��ɫ��Ϳ�����ӣ�Ϳ��w������һ�Σ�ת��Ϊs4
                if (rbt_is_black(w->right))
                {
                    rbt_black(w->left);
                    rbt_red(w);
                    rbtree_right_rotate(w);
                    w = node_sub->parent->right; //�ƶ�w����ǰnode_sub���ֵܽ��
                }
				//s4.w���Һ��Ӻ�ɫ������������Խ��������̣�ǰ��3��������ջ�ת��Ϊ�������
                rbt_copy_color(w, node_sub->parent);
                rbt_black(node_sub->parent);
                rbt_black(w->right);
                rbtree_left_rotate(node_sub->parent);
                node_sub = *root;				
			}
		}
		//��node_sub�ұ�,����Ϊ��������ľ���
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
	//���node����������x_right��������
	
	rbtree_node_t *x_right;
	rbtree_node_t **root = &rbtree_.root;
	rbtree_node_t *sentinel = &sentinel_;

	x_right = node->right;
	node->right = x_right->left;

	//sentinel��㲻�����������У�ֻ��ÿ��Ҷ�ӽ��������Ӿ�ָ��sentinel
	if(x_right->left != sentinel)
	{
		x_right->left->parent = node;
	}

	//���ڵ���ڸ���㣬�����ΪNULL		
	x_right->parent = node->parent;
	//case1 node Ϊ��
	if(node == *root)
	{
		*root = x_right;
	}
	//case2 node Ϊ������
	else if(node == node->parent->left)
	{
		node->parent->left = x_right;
	}
	//case3 node Ϊ������
	else
	{
		node->parent->right = x_right;
	}
	//����޸�node�����ӵ�λ�ù�ϵ
	x_right->left = node;
	node->parent = x_right;
	
}

void RBtree::rbtree_right_rotate(rbtree_node_t * node)
{
	//���node����������x_left��������
	
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
