#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "tables.h"
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"
#include "expr.h"

typedef struct BTreeNode {
	int* keys;
	struct BTreeNode** childPointers;
	int noOfFilledKeys;
	bool isLeafNode;
	RID* recordId;
	int dfsIndex;
} BTreeNode;

typedef struct BTreeDetails {
	BTreeNode* parent;
	int maxNoOfKeys;
	int numOfNodes;
	int noOfEntries;
	char *idxId;
	DataType type;
} BTreeDetails;

BM_BufferPool *bm;
BM_PageHandle *h;

typedef struct scanData
{
	BTreeNode* node;
	int index;
} scanData;

BTreeDetails* indexDetails;
scanData* data;

RC initIndexManager (void *mgmtData)
{
	printf("Begin initIndexManager \n");
	return RC_OK;
}

RC shutdownIndexManager ()
{
	printf("Begin shutdownIndexManager \n");
	return RC_OK;
}

BTreeNode* createNode(bool isLeaf)
{
	printf("Begin createNode \n");
	int n = indexDetails->maxNoOfKeys + 1;
	BTreeNode* node = (BTreeNode*) malloc (sizeof(BTreeNode));
	node->keys = (int *) malloc (n * sizeof (int));
	node->noOfFilledKeys = 0;
	node->isLeafNode = isLeaf;
	node->recordId = (RID*) malloc (n * sizeof (RID));
	node->childPointers = malloc ((n + 1) * sizeof (BTreeNode*));
	indexDetails->numOfNodes++;
	return node;
}

BTreeNode* goToLeftLeaf(BTreeNode* rootNode)
{
	printf("Begin goToLeftLeaf \n");
	if (rootNode->isLeafNode == true)
	{
		return rootNode;
	}
	
	return goToLeftLeaf(rootNode->childPointers[0]);
}

BTreeNode* getParentNode(BTreeNode* rootNode, BTreeNode* curNode)
{
	printf("Begin getParentNode \n");
	if (curNode == rootNode)
	{
		BTreeNode* newRoot = createNode(false);
		newRoot->childPointers[0] = curNode;
		indexDetails->parent = newRoot;
		return newRoot;
	}
	
	int i = 0;
	while (i < rootNode->noOfFilledKeys && curNode->keys[0] >= rootNode->keys[i])
	{
		i++;
	}
	
	if (rootNode->childPointers[i] == curNode)
	{
        return rootNode;
	}
	
	return getParentNode(rootNode->childPointers[i], curNode);
}

BTreeNode* splitNode(BTreeNode* node)
{
	printf("Begin splitNode \n");
	BTreeNode* newNode;
	if (node->isLeafNode)
	{
		int i, j;
		newNode = createNode(true);
		for (i = (indexDetails->maxNoOfKeys / 2) + 1, j=0; i <= indexDetails->maxNoOfKeys; i++, j++)
		{
			newNode->keys[j] = node->keys[i];
			newNode->recordId[j] = node->recordId[i];
			node->keys[i] = 0;
		}
		newNode->childPointers[0] = node->childPointers[0];
		node->childPointers[0] = newNode;
		newNode->noOfFilledKeys = j;
		node->noOfFilledKeys = (indexDetails->maxNoOfKeys / 2) + 1;
	}
	else 
	{
		int i, j;
		newNode = createNode(false);
		for (i = (indexDetails->maxNoOfKeys / 2), j=0; i <= indexDetails->maxNoOfKeys; i++, j++)
		{
			newNode->keys[j] = node->keys[i];
			newNode->childPointers[j] = node->childPointers[i+1];
			node->keys[i] = 0;
			node->childPointers[i+1]=NULL;
		}
		newNode->noOfFilledKeys = j;
		node->noOfFilledKeys = (indexDetails->maxNoOfKeys / 2);
	}
	return newNode;
}

RC createBtree (char *idxId, DataType keyType, int n)
{
	printf("Begin createBtree \n");
	
	indexDetails = (BTreeDetails*) malloc (sizeof(BTreeDetails));
	if(indexDetails==NULL)											
	{
		return RC_FILE_NOT_FOUND;								
	}
	indexDetails->parent = NULL;
	indexDetails->maxNoOfKeys = 0;
	indexDetails->numOfNodes = 0;
	indexDetails->noOfEntries = 0;
	
	indexDetails->maxNoOfKeys = n;
	indexDetails->type = keyType;
	indexDetails->parent = createNode(true);
	
	bm = MAKE_POOL();
	h = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));
	createPageFile(idxId);
	return RC_OK;
}

RC openBtree (BTreeHandle **tree, char *idxId)
{
	printf("Begin openBtree \n");
	initBufferPool(bm, idxId, 3, RS_FIFO, NULL);
	if ((rc = pinPage(bm, h, 0)) != RC_OK)
		return rc;
	return RC_OK;
}

RC closeBtree (BTreeHandle *tree)
{
	printf("Begin closeBtree \n");
	if((rc = unpinPage(bm,h)) != RC_OK)
		return rc;
	forcePage(bm, h)
	shutdownBufferPool(bm)
	return RC_OK;
}

RC deleteBtree (char *idxId)
{
	printf("Begin deleteBtree \n");
	destroyPageFile(idxId)
	free(bm);
	free(h);
	free(indexDetails->parent);
	free(indexDetails);
	return RC_OK;
}

RC getNumNodes (BTreeHandle *tree, int *result)
{
	printf("Begin getNumNodes \n");
	*result = indexDetails->numOfNodes;
	return RC_OK;
}

RC getNumEntries (BTreeHandle *tree, int *result)
{
	printf("Begin getNumEntries \n");
	*result = indexDetails->noOfEntries;
	return RC_OK;
}

RC getKeyType (BTreeHandle *tree, DataType *result)
{
	printf("Begin getKeyType \n");
	*result = indexDetails->type;
	return RC_OK;
}

RC findKey (BTreeHandle *tree, Value *key, RID *result)
{
	printf("Begin findKey \n");
	int valueToSearch = (*key).v.intV;
	BTreeNode* rootNode = indexDetails->parent;
	return recursiveSearch (rootNode, result, valueToSearch);
}

RC recursiveSearch(BTreeNode* node, RID* result, int valueToSearch)
{
	printf("Begin recursiveSearch \n");
    int i = 0;
    int j = 0;
	
	while (i < node->noOfFilledKeys && valueToSearch >= node->keys[i])
	{
		i++;
	}
	
	if (node->keys[i-1] == valueToSearch && node->isLeafNode == true)
	{
		*result = node->recordId[i-1];
		printf("First Return in recursiveSearch\n");
        return RC_OK;
	}
	
	if (node->isLeafNode == true || node->childPointers[i] == NULL)
	{
		printf("Second Return in recursiveSearch \n");
		return RC_IM_KEY_NOT_FOUND;
	}

	node = node->childPointers[i];	
	return recursiveSearch(node, result, valueToSearch);	
}

RC recursiveSearchNode(BTreeNode* node, int valueToSearch, BTreeNode** pointerToNode)
{
	printf("Begin recursiveSearch Node \n");
    int i = 0;
    int j = 0;
	
	while (i < node->noOfFilledKeys && valueToSearch >= node->keys[i])
	{
		i++;
	}
	
	if (node->keys[i-1] == valueToSearch && node->isLeafNode == true)
	{
		printf("First Return in recursiveSearch\n");
		*pointerToNode = node;
        return RC_OK;
	}
	
	if (node->isLeafNode == true || node->childPointers[i] == NULL)
	{
		printf("Second Return in recursiveSearch \n");
		*pointerToNode = node;
		return RC_IM_KEY_NOT_FOUND;
	}
	
	return recursiveSearchNode(node->childPointers[i], valueToSearch, pointerToNode);	
}

RC deleteFirstKeyFromNode(BTreeNode* node, int value)
{
	printf("Begin deleteFirstKeyFromNode");
	int i = 1;
	while (i < node->noOfFilledKeys)
	{
		node->keys[i - 1] = node->keys[i];
		i++;
	}
	node->noOfFilledKeys--;
}

RC insertKey (BTreeHandle *tree, Value *key, RID rid)
{
	int valueToInsert = (*key).v.intV;
	int i ;
	printf("Begin insertKey %d \n", valueToInsert);
	BTreeNode* node = indexDetails->parent;
	BTreeNode** pointerToNode = (BTreeNode**)malloc(sizeof(BTreeNode*));
	if (recursiveSearchNode(node, valueToInsert, pointerToNode) == RC_OK)
		return RC_IM_KEY_ALREADY_EXISTS ;
	else 
	{	
		node = *pointerToNode;
		indexDetails->noOfEntries++;
		node->keys[node->noOfFilledKeys] = valueToInsert;
		node->recordId[node->noOfFilledKeys] = rid;
		printf("Existing keys in node");
		for (i = 0; i <= node->noOfFilledKeys; i++)
			printf("%d", node->keys[i]);
		sort(node);
				
		if (node->noOfFilledKeys < indexDetails->maxNoOfKeys)
		{
			node->noOfFilledKeys++;				
			printf ("inserted successfully %d", node->noOfFilledKeys);
			for (i = 0; i < node->noOfFilledKeys; i++)
				printf("%d", node->keys[i]);
			if (node->isLeafNode)
				printf("I am leaf");
			return RC_OK;
		}
		else
		{
			printf ("inserted successfully after split %d", node->noOfFilledKeys);
			BTreeNode* newNode = (BTreeNode*) splitNode(node);			
			return recursiveInsert(node, newNode, newNode->keys[0]);
		}
	}
}

RC recursiveInsert(BTreeNode* oldNode, BTreeNode* newNode, int valueToInsert)
{
	printf("Begin recursiveInsert \n");
	BTreeNode* rootNode = indexDetails->parent;
	BTreeNode* parentNode = (BTreeNode*) getParentNode(rootNode, oldNode);
	parentNode->keys[parentNode->noOfFilledKeys] = valueToInsert;
	parentNode->childPointers[parentNode->noOfFilledKeys + 1] = newNode;
	sort(parentNode);
	if (parentNode->noOfFilledKeys < indexDetails->maxNoOfKeys)
	{
		parentNode->noOfFilledKeys++;
		return RC_OK;
	}
	else
	{
		BTreeNode* splitnode = splitNode(parentNode);
		int val = splitnode->keys[0];
		deleteFirstKeyFromNode(splitnode, val);
		return recursiveInsert(parentNode, splitnode, val);
	}
}

RC sort(BTreeNode* node)
{
	printf("Begin sort \n");
    int i, j, temp;
	if (node->noOfFilledKeys > 0)
	{
		int elementIndex = node->noOfFilledKeys;
		temp = node->keys[elementIndex];
		BTreeNode* tempChild = node->childPointers[elementIndex + 1];
		RID tempRID = node->recordId[elementIndex];
	
		i = node->noOfFilledKeys - 1; 
		while (node->keys[i] > temp && i >= 0)
		{
			node->keys[i + 1] = node->keys[i];
			node->childPointers[i + 2] = node->childPointers[i + 1];
			node->recordId[i + 1] = node->recordId [i]; 
			i--;
		}
	
		node->keys[i + 1] = temp;
		node->childPointers[i + 2] = tempChild;
		node->recordId[i + 1] = tempRID; 
	}
	return RC_OK;
}

RC deleteKey (BTreeHandle *tree, Value *key)
{
	printf("Begin deleteKey \n");
	int valueToDelete = (*key).v.intV;
	int i ;
	BTreeNode* node = indexDetails->parent;
	BTreeNode** pointerToNode = (BTreeNode**)malloc(sizeof(BTreeNode*));
	if (recursiveSearchNode(node, valueToDelete, pointerToNode) != RC_OK)
		return RC_IM_KEY_NOT_FOUND ;
	BTreeNode* nodeWithValue = *pointerToNode;
	recursiveDelete(nodeWithValue, valueToDelete)
	return RC_OK;
}

RC recursiveDelete(BTreeNode* node, int valueToDelete)
{
	int i;
	for (i = 0; i < node->noOfFilledKeys; i++)
	{
		if (node->keys[i] == valueToDelete)
		{
			break;
		}
	}
	while (i < node->noOfFilledKeys - 1)
	{
		node->keys[i] = node->keys[i+1];
		node->recordId[i] = node->recordId[i+1];
		i++;
	}
	
	node->keys[node->noOfFilledKeys-1] = 0;
	node->noOfFilledKeys--;
}

RC openTreeScan (BTreeHandle *tree, BT_ScanHandle **handle)
{
	printf("Begin openTreeScan \n");
	BTreeNode* rootNode = indexDetails->parent;
	BTreeNode* node = (BTreeNode*)goToLeftLeaf(rootNode);
	data = (scanData*) malloc (sizeof(scanData));
	data->node = node;
	data->index = 0;
}

RC nextEntry (BT_ScanHandle *handle, RID *result)
{
	printf("Begin nextEntry \n");
	int i;
	BTreeNode* node = data->node;
	if (node == NULL)
		return RC_IM_NO_MORE_ENTRIES;
		
	*result = node->recordId[data->index];
	
	BTreeNode* newn = node;
	while(newn != NULL)
	{
		printf("Keys in fillednodes %d", newn->noOfFilledKeys);
		for (i=0; i < newn->noOfFilledKeys; i++)
		{
			printf("%d", newn->keys[i]);
		}
		newn = newn->childPointers[0];
	}
	
	if (data->index == node->noOfFilledKeys - 1)
	{
		data->node = node->childPointers[0];
		data->index = 0;
	}
	else 
	{
		data->index = data->index+1;
		printf("%d\n",data->index );
	}
	return RC_OK;
}

RC closeTreeScan (BT_ScanHandle *handle)
{
	printf("Begin closeTreeScan \n");
	free(data);
	return RC_OK;
}

char *printTree (BTreeHandle *tree)
{
	printf("Begin printTree \n");
	return NULL;
}