#ifndef KD_TREE_H
#define KD_TREE_H

#include <vector>

struct KDNode
{
	int vectorIndex;
	int dimension;
	KDNode* left;
	KDNode* right;
};

template<class T>
class KDTree
{
private:
	KDNode* root;

public:
	KDTree(const vector<T>& dataset);
	~KDTree();

	// return the index of the value most similar to the given argument
	int nearestNeighbor(const T&);
};

#endif