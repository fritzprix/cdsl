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
> All the data type in cdsl is used by inserting its type into the struct you define. this kind of approach is used to exploit inheritance mechanism in C. for C++, data type can be simply inheritted to the class 

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
        cdsl_slistPutTail(&lentry,(slistNode_t*) &people);
	}
    while(!cdsl_slistIsEmpty(&lentry)){
    	p = (struct person*)cdsl_slistRemoveHead(&lentry);
        printf("this is (name :%s %s / age : %d)\n",p->firstname,p->lastname,p->age);
    }
    return 0;
}

```
