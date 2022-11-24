#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>
#include <wchar.h>

#define LENGTH 100
#define LETTERS 35

typedef struct Node {
  bool isWord;
  struct Node *children[LETTERS];
} Node;

bool load(Node **root, const char *dictionary);

int size(void);

bool check(Node **root, const wchar_t *word);

void unload(Node *root);

#endif
