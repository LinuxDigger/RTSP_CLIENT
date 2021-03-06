/*
 * HashTable.cpp
 *
 *  Created on: Jun 8, 2018
 *      Author: jhb
 */




#include "HashTable.h"

HashTable::HashTable() {
}

HashTable::~HashTable() {
}

HashTable::Iterator::Iterator() {
}

HashTable::Iterator::~Iterator() {}

void* HashTable::RemoveNext() {
  Iterator* iter = Iterator::create(*this);
  char const* key;
  void* removedValue = iter->next(key);
  if (removedValue != 0) Remove(key);

  delete iter;
  return removedValue;
}

void* HashTable::getFirst() {
  Iterator* iter = Iterator::create(*this);
  char const* key;
  void* firstValue = iter->next(key);

  delete iter;
  return firstValue;
}

