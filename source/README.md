## Header list
```
cdsl_slist.h : singly-linked list
cdsl_dlist.h : doubly-linked list
cdsl_bstree.h : vanila binary search tree
cdsl_nrbtree.h : red-black tree
cdsl_heap.h : binary heap
cdsl_spltree.h : splay tree
cdsl_hashtree.h : hash tree
```

## How-To
> for C project,  data types in cdsl can be used by inserting it into the struct you define. this kind of approach is often used to exploit inheritance mechanism in C. for C++, data type can be simply inheritted to the class. 

#### Singly-linked list (C example)
```C
struct person {
	slistNode_t lhead;
    int age;
    char firstname[20];
    char lastname[20]; 
};

slistEntry_t lentry;
static struct people[10];

int main(void) {
	int i;
    struct person* p;
	cdsl_slistEntryInit(&lentry);
    for(i = 0;i < 10;i++){
		people[i].age = rand();
        ...
		cdsl_slistNodeInit(&people[i].lhead);
		cdsl_slistPutTail(&lentry,&people[i].lhead);
        /*or you can simply use casting considering memory layout*/
        cdsl_slistPutTail(&lentry,(slistNode_t*) &people[i]);
	}
    while(!cdsl_slistIsEmpty(&lentry)){
    	p = (struct person*)cdsl_slistRemoveHead(&lentry);
        printf("this is (name :%s %s / age : %d)\n",p->firstname,p->lastname,p->age);
    }
    return 0;
}

```

#### red-black tree (C example)
```C
struct person {
	nrbtreeNode_t node;
    int age;
    char firstname[20];
    char lastname[20]; 
};

nrbtreeRoot_t root;
static struct people[10];

int main(void) {
	int i;
    struct person* p;
    cdsl_nrbtreeRootInit(&root);
    for(i = 0;i < 10;i++){
		people[i].age = rand();
        ...
		cdsl_nrbtreeNodeInit(&people[i].node);
		cdsl_nrbtreeInsert(&root, &people[i].node, FALSE);
        /*or you can simply use casting considering memory layout*/
//      cdsl_nrbtreeInsert(&root,(nrbtreeNode_t*) &people[i]);
	}
    while(!cdsl_nrbtreeIsEmpty(&lentry)){
    	p = (struct person*) cdsl_nrbtreeDeleteMax(&root);
        printf("this is (name :%s %s / age : %d)\n",p->firstname,p->lastname,p->age);
    }
    return 0;
}

```