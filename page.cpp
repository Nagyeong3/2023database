#include "page.hpp"
#include <iostream> 
#include <cstring> 
//dest = data를 저장할 메모리 공간
void put2byte(void *dest, uint16_t data){
	*(uint16_t*)dest = data;
}

uint16_t get2byte(void *dest){
	return *(uint16_t*)dest;
}

page::page(uint16_t type){
	hdr.set_num_data(0);
	hdr.set_data_region_off(PAGE_SIZE-1-sizeof(page*));
	hdr.set_offset_array((void*)((uint64_t)this+sizeof(slot_header)));
	hdr.set_page_type(type);
}

uint16_t page::get_type(){
	return hdr.get_page_type();
}

uint16_t page::get_record_size(void *record){
	uint16_t size = *(uint16_t *)record;
	return size;
}

char *page::get_key(void *record){
	char *key = (char *)((uint64_t)record+sizeof(uint16_t));
	return key;
}

uint64_t page::get_val(void *key){
	uint64_t val= *(uint64_t*)((uint64_t)key+(uint64_t)strlen((char*)key)+1);
	return val;
}

void page::set_leftmost_ptr(page *p){
	leftmost_ptr = p;
}

page *page::get_leftmost_ptr(){
	return leftmost_ptr;	
}

uint64_t page::find(char *key){
	// Please implement this function in project 1.
	uint64_t val = 0;
	return val;
}

bool page::insert(char *key,uint64_t val){
	int num_data = hdr.get_num_data();
	void *offset_array=hdr.get_offset_array();
	//printf("offset array %d \n",offset_array);
	uint16_t off=0;
	uint16_t record_size =  2 + strlen(key)+ 1 +sizeof(val);	// 레코드 사이즈 = 레코드 길이 담을 공간 + 키 길이 + '/0' + value size

	if(is_full(record_size)){
		printf("page is full\n");
		return false;
	}else{
		//printf("page 끝 지점: %d \n",hdr.get_data_region_off());
		printf("free space 시작지점: %d \n", data);
		// //1) 레코드사이즈 저장
		printf("key: %s value: %d record size: %d \n",key,val,record_size);
		//2)데이터가 들어갈 위치계산
		uint64_t data_dest = hdr.get_data_region_off()-record_size;
		printf("this 주소: %u\n", (uint64_t)this); 
		printf("dest addr : %d \n", data_dest);
		//레코드 사이즈 저장
		uint64_t address = (uint64_t)this + data_dest;
		memcpy((void*)address,&record_size,2);

		printf("data가 들어가는 주소: %u\n", (uint64_t)this + data_dest);
		printf("this+data_dest 주소에 들어간 Record size: %u\n", get2byte((void*)address));
		
		address = (uint64_t)this + data_dest+2;
		memcpy((void*)address,key,strlen(key)+1);
		printf("주소에 들어간 key: %s\n",(char*)(void*)address);

		address = (uint64_t)this+data_dest+2+strlen(key)+1;
		memcpy((void*)address,&val,8);
		printf("주소에 들어간 val: %u\n", get2byte((void*)address));
		hdr.set_data_region_off(data_dest);	//데이터 시작 영역 조정(방금 들어온 레코드만큼 앞당기기)
	

		//offset
		printf("offset\n");
		memcpy(offset_array,&data_dest,2);
		for(int i=0; i<num_data; i++){
			uint64_t curr_off=get2byte(data+i*2);
			address=(uint64_t)this+curr_off+2;
			printf("curr_off(%d)에 들어간 key = %s\n",curr_off, (char*)(void*)address);
			if((char*)(void*)address > key){
				//put2byte(data+i*2,key);
			}
		}
		printf("memcpy로 data insert => offset array %u \n",get2byte(offset_array));
		num_data++;
		hdr.set_num_data(num_data);
	
		
		hdr.set_offset_array((void*)((uint64_t)offset_array + 2));

		//printf("offset array address %d \n", hdr.get_offset_array());
		for(int i=0; i<num_data; i++){
			off= *(uint16_t *)((uint64_t)data+i*2);	
			printf(" %d |",off);
		}
		printf("\n");
		printf("offset array %u \n -----insert 끝------\n",get2byte(offset_array));

	}
		printf("현재 데이터 개수는? : %d\n", num_data);

	return true;
}

page* page::split(char *key, uint64_t val, char** parent_key){
	// Please implement this function in project 2.
	page *new_page;
	return new_page;
}

bool page::is_full(uint64_t inserted_record_size){
	int num_data = hdr.get_num_data();
	void *offset_array=hdr.get_offset_array();
	int header_size=sizeof(header_size);
	printf("------this is is_full func!-------\n");
	printf("size: %d get_data_region: %d \n",inserted_record_size, hdr.get_data_region_off());
	for(int i=0; i<num_data; i++){
		header_size= header_size+i*2;
	}	
	int freespace = hdr.get_data_region_off()-header_size-hdr.get_num_data()*2-inserted_record_size-sizeof(uint16_t);
	printf("less than 0 check: %d \n",freespace);
	if(freespace < 0){
		return true;
	}else{
		return false;
	}
}

void page::defrag(){
	page *new_page = new page(get_type());
	int num_data = hdr.get_num_data();
	void *offset_array=hdr.get_offset_array();
	void *stored_key=nullptr;
	uint16_t off=0;
	uint64_t stored_val=0;
	void *data_region=nullptr;

	for(int i=0; i<num_data/2; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		data_region = (void *)((uint64_t)this+(uint64_t)off);
		stored_key = get_key(data_region);
		stored_val= get_val((void *)stored_key);
		new_page->insert((char*)stored_key,stored_val);
	}	
	new_page->set_leftmost_ptr(get_leftmost_ptr());

	memcpy(this, new_page, sizeof(page));
	hdr.set_offset_array((void*)((uint64_t)this+sizeof(slot_header)));
	delete new_page;

}

void page::print(){
	uint32_t num_data = hdr.get_num_data();
	uint16_t off=0;
	uint16_t record_size= 0;
	void *offset_array=hdr.get_offset_array();
	void *stored_key=nullptr;
	uint64_t stored_val=0;

	printf("## slot header\n");
	printf("Number of data :%d\n",num_data);
	printf("offset_array : |");
	for(int i=0; i<num_data; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		printf(" %d |",off);
	}
	printf("\n");

	void *data_region=nullptr;
	for(int i=0; i<num_data; i++){
		off= *(uint16_t *)((uint64_t)offset_array+i*2);	
		data_region = (void *)((uint64_t)this+(uint64_t)off);
		record_size = get_record_size(data_region);
		stored_key = get_key(data_region);
		stored_val= get_val((void *)stored_key);
		printf("==========================================================\n");
		printf("| data_sz:%u | key: %s | val :%lu |\n",record_size,(char*)stored_key, stored_val,strlen((char*)stored_key));

	}
}




