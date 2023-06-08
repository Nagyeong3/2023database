#include "btree.hpp"
#include <iostream> 

btree::btree(){
	root = new page(LEAF);
	height = 1;
};
struct ListNode{
    page* data;
    struct ListNode *next;
};
struct Stack{
    struct ListNode *top;
};
struct Stack *CreateStack(){
    struct Stack *stk;
    stk = (struct Stack *) malloc(sizeof(struct Stack));    
    stk->top = NULL;
    return stk;
}
void Push(struct Stack *stk, page* data){
    struct ListNode *temp;
    temp = (struct ListNode *) malloc(sizeof(struct ListNode));
    if(!temp)
        return;
    temp->data = data;
    temp->next = stk->top;
    stk->top = temp;
}
int IsEmptyStack(struct Stack *stk){
    return (stk->top==NULL);
}
page* Pop(struct Stack *stk){
    page* data;
    struct ListNode *temp;
    if(IsEmptyStack(stk))
        return 0;
    temp = stk->top;
    stk->top = stk->top->next;
    data = temp->data;
    free(temp);
    return data;
}

page* Top(struct Stack *stk){
    if(IsEmptyStack(stk))
        return 0;
    return stk->top->data;
}

void btree::insert(char *key, uint64_t val){
	// Please implement this function in project 2.
	Stack* stack=CreateStack();
	// page *stack[height];
	uint16_t stack_cnt=0;
	page* currentPage=root;
	page* new_page_leaf=new page(LEAF);
	page* new_page_internal=new page(INTERNAL);
	uint16_t key_len;//부모 키의 len만큼 
	uint64_t parent_addr;
	uint16_t record_size =  2 + strlen(key)+ 1 +sizeof(val);	// 레코드 사이즈 = 레코드 길이 담을 공간 + 키 길이 + '/0' + value size
	char* parent_key;
	while (currentPage->get_type()!=LEAF){
		printf("current type is not LEAF\n push the currentPage address(%d)\n",(uint64_t)currentPage);
		Push(stack,currentPage);
		stack_cnt++;
		currentPage = (page*)(currentPage->find(key));
	}
	//초기 currentPage가 LEAF node가 full인 경우 차곡차곡 위로 올라가며 full 확인해야 되므로
	if(currentPage->is_full(record_size)){
		while(currentPage->is_full(record_size)){
			if(currentPage->get_type()==LEAF){
				currentPage->defrag();
				new_page_leaf = currentPage->split(key,val,&parent_key);
				currentPage=Pop(stack);
				// key_len = strlen((const char*)*parent_key);
				key_len = strlen((char*)*parent_key);
				parent_key = (char*)malloc(key_len+1);
				key = parent_key;
				val = (uint64_t)new_page_leaf;
			}else if (currentPage->get_type()==INTERNAL&&currentPage!=root){
				new_page_internal = currentPage->split(key,val,&parent_key);
				currentPage=Pop(stack);
				key_len = strlen((char*)*parent_key);
				parent_key = (char*)malloc(key_len+1);
				key = parent_key;
				val = (uint64_t)new_page_internal;
			}else if(currentPage==root){
				height++;
				parent_key = (char*)malloc(key_len+1);
				new_page_internal = currentPage->split(key,val,&parent_key);
				key =parent_key;	//맨 상단 g
				val = (uint64_t)new_page_internal;//parent i의 주소
				page* new_root = new page(INTERNAL);
				new_root->set_leftmost_ptr(currentPage->get_leftmost_ptr());
				new_root->insert(key,val);
				root = new_root;
				currentPage=root;
				return;
				// top_root->insert(key,val);
				
			}
			
			// go back to the loop
		}
	}else{
		printf("currentpage full아님 바로 key:%s,val:%lu insert\n",key,val);
		currentPage->insert(key,val);
		return;
	}
	
	//parentkey 주소값 업데이트해야됨.
	
}
// key 를 인자로 받으면 LEAF 노드로 계속 타고 내려가서
// key 가 존재하는 지 찾고, 존재하면 value 를 반환하고 존재하지 않으면 0 을 반환합니다.
// 이는 수정된 page::find() 를 통해 쉽게 구현할 수 있을 것 같아요!
// 현재 페이지가 leaf 인지 아닌지 계속 확인하면서 find 를 호출하는 형태로 작성하면 될 것 같습니다.
uint64_t btree::lookup(char *key){
	// Please implement this function in project 2.
	uint64_t val = 0;
	page* currentPage=root;
	while(currentPage->get_type()!=LEAF){
		currentPage=(page*)(currentPage->find(key));
	}
	//find the value in leaf page
	val=currentPage->find(key);
	return val;
}
