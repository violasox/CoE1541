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

int hashCode(int key)
{
  return key % SIZE;
}

struct DataItem *search(int key)
{
  //get the hash
  int hashIndex = hashCode(key);

  //move in array until an empty
  while(hashArray[hashIndex] != NULL)
  {

      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex];

      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= SIZE;
   }

   return NULL;
}

void insert(int key,int data)
{

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = data;
   item->key = key;

   //get the hash
   int hashIndex = hashCode(key);

   hashArray[hashIndex] = item;
}

struct DataItem* delete(struct DataItem* item)
{
   int key = item->key;

   //get the hash
   int hashIndex = hashCode(key);

   //move in array until an empty
   while(hashArray[hashIndex] != NULL)
   {

      if(hashArray[hashIndex]->key == key)
      {
         struct DataItem* temp = hashArray[hashIndex];

         //assign a dummy item at deleted position
         hashArray[hashIndex] = dummyItem;
         return temp;
      }

      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= SIZE;
   }

   return NULL;
}

void display(void)
{
   int i = 0;

   for(i = 0; i<SIZE; i++)
   {

      if(hashArray[i] != NULL)
         printf(" (%d,%d)",hashArray[i]->key,hashArray[i]->data);
      else
         printf(" ~~ ");
   }

   printf("\n");
}

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
