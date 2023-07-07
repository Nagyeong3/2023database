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
	// page *stack[height];
	Stack* stack=CreateStack();
	uint16_t stack_cnt=0;
	page* currentPage=root;
	page* new_page_leaf=new page(LEAF);
	page* new_page_internal=new page(INTERNAL);
	uint16_t key_len;//부모 키의 len만큼 
	uint64_t parent_addr;
	uint16_t record_size =  2 + strlen(key)+ 1 +sizeof(val);	// 레코드 사이즈 = 레코드 길이 담을 공간 + 키 길이 + '/0' + value size
	
	key_len = strlen((char*)key);
	char* parent_key = (char*)malloc(key_len+1);
	while (currentPage->get_type()!=LEAF){
		// printf("current type is not LEAF\n push the currentPage address(%d)\n",(uint64_t)currentPage);
		Push(stack,currentPage);
		stack_cnt++;
		// printf("자식 페이지 찾아가는 과정 stack push page is..?\n");
		// currentPage->print();
		currentPage = (page*)(currentPage->find(key));
		
		
	}
	// printf("currentpage is full? %d \n",currentPage->is_full(record_size));
	//초기 currentPage가 LEAF node가 full인 경우 차곡차곡 위로 올라가며 full 확인해야 되므로
	if(currentPage->is_full(record_size)){
		
		while(currentPage->is_full(record_size)){
			uint16_t page_type = currentPage->get_type();
			if(page_type==LEAF){
				printf("여긴 리프\n");
				currentPage->print();
				
				new_page_leaf = currentPage->split(key,val,&parent_key);
				printf("들어온 parent key %s\n split result\n",parent_key);
				new_page_leaf->print();
				// printf("new_page_leaf addrr????????&&&& %u\n",(uint64_t)new_page_leaf);
				// printf("split이후 올라갈 key:%s\n ",parent_key);
				// printf("%u  vs %u",(uint64_t)root, (uint64_t)currentPage);
				if((uint64_t)root==(uint64_t)currentPage){
					// printf("root이자 curr\n");
					page* new_root = new page(INTERNAL);
					new_root->set_leftmost_ptr(currentPage);
					strcpy(key,parent_key);

					val=(uint64_t)new_page_leaf;
					
					// new_root->insert(parent_key,val);
					// printf("new root print\n");
					// new_root->print();
					currentPage->defrag();
					printf("defrag결과1=================>>>>>>\n");
					

					

					currentPage->print();
					// printf("root get->leftmost connect @!!: %llu\n",(uint64_t)currentPage);
				
					root=new_root;	//root=new_root

					root->set_leftmost_ptr(currentPage);
					
					// printf("c의 left most ptr %llu",root->get_leftmost_ptr());
					// memcpy(currentPage, root, sizeof(page));
					currentPage=root;
					// printf("root addr : %llu, currpage addr %llu\n\n\n\n\n",root,currentPage);
					
				}else{
					// printf("**************************************%s \n",parent_key);
					strcpy(key,parent_key);
					// printf("key(%s)==parentkey(%s)",key,parent_key);
					
					currentPage->defrag();
					// printf("defrag결과 qrst??=================>>>>>>\n");
					// printf("x1x1currpage addr:%llu\n",(uint64_t)currentPage);
					// currentPage->print();
					
					// key_len = strlen((char*)*parent_key);
					// parent_key = (char*)malloc(key_len+1);
					
					val=(uint64_t)new_page_leaf;
					
					currentPage=Pop(stack);
					// key_len = strlen((const char*)*parent_key);
				
				}			
				// printf("================new_page_leaf 잠깐 지움================\n");
				// new_page_leaf->print();
				// if(){
				// 	printf("here .....");
				// }else{
				// 	printf("nope .....");
				// }
				
			}else if (page_type==INTERNAL&&((uint64_t)root!=(uint64_t)currentPage)){
				// printf("여긴 internal, not root\n");
				new_page_internal = currentPage->split(key,val,&parent_key);
				// page* temp;
				// temp=Pop(stack);
				// temp->set_leftmost_ptr(currentPage);
				currentPage->defrag();
				// printf("currentPage defrag 했음 \n");
				// currentPage->print();
				// currentPage=temp;

				// printf("currentPage에 parent page를 대입 했음 \n");
				// currentPage->print();

				key_len = strlen((char*)*parent_key);
				parent_key = (char*)malloc(key_len+1);
				strcpy(key,parent_key);
				// printf("key(%s)==parentkey(%s)",key,parent_key);
				val = (uint64_t)new_page_internal;
			}else if((uint64_t)root==(uint64_t)currentPage){
				// printf("여긴 root\n");
				height++;
				
				new_page_internal = currentPage->split(key,val,&parent_key);
				strcpy(key,parent_key);
				// printf("key(%s)==parentkey(%s)",key,parent_key);
				val = (uint64_t)new_page_internal;
				page* new_root = new page(INTERNAL);
				// new_root->set_leftmost_ptr(currentPage->get_leftmost_ptr());
				new_root->insert(key,val);
				
				// printf("동작확인 내가 원하는 new root mm \n");
				// new_root->print();
				new_root->set_leftmost_ptr(currentPage);
				currentPage->defrag();
				// page* test = new_root->get_leftmost_ptr();
				// printf("동작 확인 내가 원하는 new root m의 leftmost는 dgj \n");
				// test->print();
				root=new_root;
				
				currentPage=root;

				// memcpy(currentPage, root, sizeof(page));//currentPage=root;
				// printf("new root_left most ptr %llu, 레프트 모스트 따라간 자식 페이지 프린트는?\n",(u_int64_t)currentPage->get_leftmost_ptr());
				//  currentPage->print();
				// printf("new root?? %llu root?? %llu \n curr %llu",new_root,root,currentPage);
				// page* test2 = root->get_leftmost_ptr();
				// printf("동작 확인 내가 원하는 new root m의 leftmost는 dgj \n");
				// test2->print();
				return;
				// top_root->insert(key,val);
				
			}
			
			// go back to the loop
		}
		// printf("key ????? %s \n",key);
		// printf("curr add %llu, root add %llu",currentPage,root);
		currentPage->insert(key,val);
		printf("insert후 print\n");
		currentPage->print();
		return;
		
	}
	else{
	
		// printf("currentpage full아님 바로 key:%s,val:%lu insert\n",key,val);
		
		currentPage->insert(key,val);
			// currentPage->print();
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
	
	
	// for(int i=0;i<3;i++){
	// 	// currentPage->print();
	// 	// printf("current page addr : %llu\n",currentPage);
	// 	// currentPage=(page*)(currentPage->find(key));
	// 	// printf("curr :? %llu\n",(uint64_t)currentPage);

	// 	printf("this is lookup=====\n");
	// 	printf("내가 원하는 new root mm \n");
	// 	currentPage->print();
	// 	page* test23 = currentPage->get_leftmost_ptr();
	// 	printf("내가 원하는 new root m의 leftmost는 dgj \n");
	// 	test23->print();
	// 	currentPage=test23;
	// }
	
	while(currentPage->get_type()!=LEAF){
		// currentPage->print();
		currentPage=(page*)(currentPage->find(key));
	}
	//find the value in leaf page
	val=currentPage->find(key);
	return val;
}
