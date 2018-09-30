#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#define SIZE 64

struct DataItem
{
  int data;
  int key;
};

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;

int hashCode(int key);

struct DataItem *search(int key);

void insert(int key,int data);

struct DataItem* delete(struct DataItem* item);

void display(void);

#endif

// NOTE: Put this stuff in five_stage.c after push to prevent merge conflicts
// struct DataItem* hashArray[TABLE_SIZE];
// dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
// dummyItem->data = -1;
// dummyItem->key = -1;
//
// struct DataItem {
//  int data;
//  int key;
// };
//
// https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
