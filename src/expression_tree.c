#include "../headers/ExpressionTree.h"

// static unsigned char precedenceLookUp[] = {
// 	[NODE_ERROR] = 255	/* error has highest precedence: if found an error, stop tree building immediately */
// };

ExpressionTree expressiontree_build_tree(Token *expr, size_t length)
{
	// loop through token stream expr and build a tree if the expression is valid
	ExpressionTree root = NULL;
	return root;
}
void expressiontree_print_to_file(FILE *fp, ExpressionTree root)
{
	return;
}
void expressiontree_destroy_tree(ExpressionTree *root)
{
	return;
}

