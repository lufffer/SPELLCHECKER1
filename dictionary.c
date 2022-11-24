#include "dictionary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

int _size = 0;

static bool next(Node *ptr, int idx) {
  if (ptr->children[idx] == NULL) { // Node does not exist
    ptr->children[idx] = malloc(sizeof(Node));
    if (ptr->children[idx] == NULL) { // Error creating node
      return false;
    }
    ptr->children[idx]->isWord = false;
    for (int j = 0; j < LETTERS; ++j) {
      ptr->children[idx]->children[j] = NULL;
    }
  }
  return true;
}

static int fixIdx(int idx, int c) {
  if (c == (int)L'á') {
    idx = 26;
  } else if (c == (int)L'é') {
    idx = 27;
  } else if (c == (int)L'í') {
    idx = 28;
  } else if (c == (int)L'ó') {
    idx = 29;
  } else if (c == (int)L'ú') {
    idx = 30;
  } else if (c == (int)L'ñ') {
    idx = 31;
  } else if (c == (int)L'ü') {
    idx = 32;
    // This character: "‒́", is a super strange character because you see a
    // multibyte character but actually there are two separated characters
    // You have to treat them as a string and not as a char, so when I
    // print the int version of theese characters gives me the next output
    // '-' as 8210
    // '´' as 769
    // this is strange because '-'(score) ascii is 45 and '´'(acute) ascii
    // is 239 On internet I just found this:
    // https://spanish.stackexchange.com/questions/19312/cómo-se-llama-el-símbolo-́
  } else if (c == 8210) {
    idx = 33;
  } else if (c == 769) {
    idx = 34;
  }
  return idx;
}

bool load(Node **root, const char *dictionary) {
  // initialize trie
  *root = malloc(sizeof(Node));
  if (*root == NULL) {
    return false;
  }
  (*root)->isWord = false;
  for (int i = 0; i < LETTERS; ++i) {
    (*root)->children[i] = NULL;
  }

  // open dictionary file
  FILE *file = fopen(dictionary, "r");
  if (file == NULL) {
    unload(*root);
    return false;
  }

  // buffer for each word
  wchar_t word[LENGTH];
  int idx;
  // insert each word into trie
  while (fscanf(file, "%ls", word) != WEOF) {
    Node *ptr = *root;
    for (unsigned int i = 0, len = wcslen(word); i < len; ++i) {
      idx = (int)word[i] - (int)'a'; // Case a-z save in pos [0 - 25]
      if (idx > 25) {
        idx = fixIdx(idx, (int)word[i]);
      }
      // Create next node
      if (!next(ptr, idx)) {
        return false;
      }
      ptr = ptr->children[idx];
    }
    ptr->isWord = true;
    _size++;
  }
  // cleanup
  fclose(file);
  return true;
}

bool check(Node **root, const wchar_t *word) {
  Node *ptr = *root;
  int idx;
  for (unsigned int i = 0, len = wcslen(word); i < len; ++i) {
    idx = (int)towlower(word[i]) - (int)'a';
    if (idx > 25) {
      idx = fixIdx(idx, (int)word[i]);
    }
    ptr = ptr->children[idx];
    if (ptr == NULL) {
      return false;
    }
  }

  return ptr->isWord;
}

int size(void) { return _size; }

void unload(Node *root) {
  if (root == NULL) {
    return;
  }

  for (int i = 0; i < LETTERS; ++i) {
    if (root->children[i] != NULL) {
      unload(root->children[i]);
    }
  }

  free(root);
}
