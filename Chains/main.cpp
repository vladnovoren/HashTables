#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>


const unsigned long long kHashP = 257;
const unsigned long long kMaxLen = 100003;
const unsigned long long kTableSize = 1000000;
const unsigned long long kMod = 1000000007;



// Debug stuff
//--------------------------------------------------------------------------------
// Проверка на то, что память была выделена по указанному указателю
bool IsMemAlloced(const void *ptr, const char *file, const char *func, size_t line) {
  if (!ptr) {
    return false;
  } else
    return true;
}
//--------------------------------------------------------------------------------



// Common structures & typedefs
//--------------------------------------------------------------------------------
struct Var {
  const char *name;
  int val;
};
//--------------------------------------------------------------------------------



// List
//--------------------------------------------------------------------------------
typedef Var ListT;

struct ListNodeT {
  ListT val;
  ListNodeT *next;
};

struct List {
  ListNodeT *first;
};


// Выделение новой ноды листа на куче
ListNodeT *AllocListNode(ListT val) {
  ListNodeT *new_node = (ListNodeT *)malloc(sizeof(ListNodeT));
  if (!IsMemAlloced(new_node, __FILE__, __PRETTY_FUNCTION__, __LINE__))
    return nullptr;
  new_node->val = val;
  new_node->next = nullptr;
  return new_node;
}


// Освобождение памяти, выделенной под лист
void DestructList(List *list) {
  assert(list);

  ListNodeT *cur_node_p = list->first;
  ListNodeT *prev_node_p = cur_node_p;
  while (cur_node_p) {
    prev_node_p = cur_node_p;
    cur_node_p = cur_node_p->next;
    free((void *)prev_node_p->val.name);
    free(prev_node_p);
  }
}


// Индикатор равенства элементов листа (в данном случае проверяетя, что одинаковые имена)
bool CompListT(ListT first, ListT second) {
  assert(first.name);
  assert(second.name);

  size_t symb_num = 0;
  if (strcmp(first.name, second.name) == 0)
    return true;
  else
    return false;
}


// Поиск элемента в листе по имени
ListNodeT *FindNode(List *list, ListT elem) {
  assert(list);

  ListNodeT *cur_node_p = list->first;
  while (cur_node_p) {
    if (CompListT(cur_node_p->val, elem))
      return cur_node_p;
    else
      cur_node_p = cur_node_p->next;
  }

  return nullptr;
}


// Вставка в начало листа
ListNodeT *PushFront(List *list, ListT elem) {
  assert(list);
  assert(elem.name); 

  ListNodeT *new_node_p = AllocListNode(elem);
  if (new_node_p) {
    new_node_p->next = list->first;
    list->first = new_node_p;
    return new_node_p;
  } else
    return nullptr;
}
//--------------------------------------------------------------------------------



// Hash-table
//--------------------------------------------------------------------------------
typedef Var HT_ElemT;

struct HT {
  List *buckets;
  size_t size;
};


HT AllocateHT(size_t n_buckets) {
  HT new_table = {};
  
  new_table.buckets = (List *)calloc(n_buckets, sizeof(List));
  if (IsMemAlloced(new_table.buckets, __FILE__, __PRETTY_FUNCTION__, __LINE__))
    new_table.size = n_buckets;

  return new_table;
}


unsigned long long GetHash(const char *str) {
  assert(str);

  unsigned long long hash = 0;
  size_t symb_num = 0;
  while (str[symb_num]) {
    hash = hash * kHashP + (unsigned long long)(str[symb_num] - '0');
    symb_num++;
  }

  return hash % kTableSize;
}


unsigned long long GetHash(const HT_ElemT elem) {
  assert(elem.name);

  return GetHash(elem.name) % kTableSize;
}


HT_ElemT *FindInHT(HT *table, HT_ElemT elem) {
  assert(table);

  size_t hash = GetHash(elem);
  ListNodeT *found = FindNode(table->buckets + hash, elem);
  if (found)
    return &(found->val);
  else
    return nullptr;
}


HT_ElemT *InsertIntoHT_NoDup(HT *table, HT_ElemT new_elem) {
  assert(table);

  size_t hash = GetHash(new_elem);
  ListNodeT *found = FindNode(table->buckets + hash, new_elem);
  if (found)
    return &(found->val);
  else
    return &(PushFront(table->buckets + hash, new_elem)->val);
}


HT_ElemT *InsertIntoHT(HT *table, HT_ElemT new_elem) {
  assert(table);

  size_t hash = GetHash(new_elem);
  ListNodeT *p = PushFront(table->buckets + hash, new_elem);
  return p ? &(p->val) : nullptr;
}


HT_ElemT *UpdElemInHT(HT *table, HT_ElemT update) {
  assert(table);

  HT_ElemT *found = FindInHT(table, update);
  if (found)
    found->val += update.val;
  else {
    char *new_str = strdup(update.name);
    if (!IsMemAlloced(new_str, __FILE__, __PRETTY_FUNCTION__, __LINE__))
      return nullptr;

    update.name = new_str;
   
    found = InsertIntoHT(table, update);
  }

  return found;
}


void DestructHT(HT *table) {
  assert(table);

  for (size_t bucket_num = 0; bucket_num < table->size; bucket_num++)
    DestructList(table->buckets + bucket_num);

  free(table->buckets);
}
//--------------------------------------------------------------------------------



int main () {
  char *str = (char *)calloc(kMaxLen, sizeof(char));
  int num = 0;

  HT table = AllocateHT(kTableSize);
  if (!table.size)
    return 1;

  while (scanf("%s %d", str, &num) == 2) {
    HT_ElemT *elem = UpdElemInHT(&table, {str, num});
    if (!IsMemAlloced(elem, __FILE__, __PRETTY_FUNCTION__, __LINE__))
      return 1;
    printf("%d\n", elem->val);
  }

  DestructHT(&table);
  free(str);
  return 0;
}
