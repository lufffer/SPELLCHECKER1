#include "dictionary.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

int main(int argc, char *argv[]) {
  setlocale(LC_CTYPE, "");
  if (argc == 1) {
    printf("Please enter a dictionary and a file\n\n");
    printf("spell_check -h for help\n");
    return 1;
  } else if (argc == 2) {
    if (strcmp(argv[1], "-h") == 0) {
      puts("USAGE: spell_check dictionary.txt file.txt\n");
      puts("OPTIONS");
      puts("-h print this help");
      return 0;
    } else {
      printf("Please enter a file\n\n");
      printf("spell_check -h for help\n");
      return 1;
    }
  } else if (argc > 3) {
    puts("Too many arguments");
    printf("spell_check -h for help\n");
  }

  char *dictionary = argv[1];
  char *filename = argv[2];
  printf("File to be checked: %s\n", filename);

  Node *root;

  // Load dictionary
  if (!load(&root, dictionary)) {
    puts("Couldn't load dictionary");
    return 1;
  }

  /* // open text file */
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Couldn't open %s\n", filename);
    unload(root);
    return 1;
  }

  // get stadistics
  printf("\n=====================\nMISSPELLED WORDS\n=====================\n");
  int index = 0;
  int misspellings = 0;
  int words = 0;
  wchar_t word[LENGTH + 1];

  // iterate through each word in file
  for (wint_t c = fgetwc(file); c != WEOF; c = fgetwc(file)) {
    // Allow only alphabetical characters
    if (iswalpha(c)) {
      word[index] = c;
      index++;

      // ignore strings longer than LENGTH
      if (index > LENGTH) {
        while ((c = fgetwc(file)) != WEOF && iswalpha(c))
          ;
        index = 0;
      }
    } else if (iswdigit(c)) {
      while ((c = fgetwc(file)) != WEOF && iswalnum(c))
        ;
      index = 0;
    } else if (index > 0) {
      // get whole word
      word[index] = '\0';
      index = 0;
      words++;

      if (!check(&root, word)) {
        printf("%ls\n", word);
        misspellings++;
      }
    }
  }

  // cleanup
  fclose(file);
  unload(root);

  // print out statistics
  printf("\nDictionary word count: %d", size());
  printf("\nText word count:       %d", words);
  printf("\nMisspelled words:      %d\n", misspellings);
  return 0;
}
