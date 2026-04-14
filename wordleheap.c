/**
 * Leanna Persaud
 * COP 3502C – Program 6: Wordle Heaps
 * Professor Rahaman
 * Date: 12/01/2025
 **/
/**MY NOTES: g=perfect, y= in the word but in incorrect spot, b(gray)= wrong */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define WORD_LEN 5
#define ALPHABET 26

// Table from instructions
// This tables tells use how common each letter is in the English language
// The letters that appear more often will get a higher score.

const int freq[26] = {
    8, 2, 3, 4, 13, 3, 2, 6, 7, 0, 1, 4, 2,
    7, 7, 2, 0, 6, 6, 9, 3, 1, 2, 0, 2, 0
};

// Structure definitions
// This keeps track of each word and its score
typedef struct {
    char word[WORD_LEN + 1];
    int score;
} Entry;

typedef struct {
    Entry *heapArray; // Keeps track of all the words in the heap
    int capacity;     // Helps us find the highest score (priortiy) 
    int size;
} heapStruct;

//  FUNCTION PROTOTYPES
int scoreWord(const char *word);
int compareEntries(Entry a, Entry b);
void swap(Entry *a, Entry *b);
void heapify(heapStruct *heap, int i);
void buildHeap(heapStruct *heap);
Entry extractMax(heapStruct *heap);
int isValidWord(const char *word, char guesses[][6], char feedbacks[][6], int m);
void freeHeap(heapStruct *heap);

// Main function
int main() {
    int n;                                 //Reads how many words are in the dictionary,
    if (scanf("%d", &n) != 1) return 1;    //Then creates space (malloc) for all the words

    char **dictionary = malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        dictionary[i] = malloc((WORD_LEN + 1) * sizeof(char));
        scanf("%s", dictionary[i]);         // Scan f string reads each word. 
    }

    int m;
    scanf("%d", &m);
// "M" is used to read how many guesses the user makes and the feedback for each guess.
    char guesses[m][6];
    char feedbacks[m][6];          // Each guess and its feedback are 5 letters long +1 for null terminator. 
    for (int i = 0; i < m; i++) {
        scanf("%s", guesses[i]);
        scanf("%s", feedbacks[i]);
    }

    // Filter candidates/players
    // Creates an array to hold valid candidate words.
    Entry *candidates = malloc(n * sizeof(Entry));
    int count = 0;  //<- How many words remain valid after filtering.

    //If no words remain valid after filtering, print "No candidates found."
    for (int i = 0; i < n; i++) {
        if (isValidWord(dictionary[i], guesses, feedbacks, m)) {
            strcpy(candidates[count].word, dictionary[i]);
            candidates[count].score = scoreWord(dictionary[i]);
            count++;
        }
    }

    if (count == 0) {
        printf("No candidates found.\n");
        for (int i = 0; i < n; i++) free(dictionary[i]);
        free(dictionary);
        free(candidates);
        return 0;
    }

    // Build the heap
    heapStruct heap;
    heap.heapArray = candidates;
    heap.size = count;
    heap.capacity = count;
    buildHeap(&heap);

    // Print the words in order from highest to lowest score. 
    while (heap.size > 0) {
        Entry top = extractMax(&heap);
        printf("%d %s\n", top.score, top.word);
    }

    // Cleanup and free memory
    for (int i = 0; i < n; i++) free(dictionary[i]);
    free(dictionary);
    freeHeap(&heap);

    return 0;
}

// Functio Definitions

// Calculate word score based on letter frequencies (how much a letter is used)
int scoreWord(const char *word) {
    int score = 0;
    for (int i = 0; i < WORD_LEN; i++)
        score += freq[word[i] - 'a'];
    return score;
}

// Compare entries for heap ordering
// Which word is bigger or has the higher score?
int compareEntries(Entry a, Entry b) {
    if (a.score != b.score)                           // If 2 words have the same score, use alphabetical order to determine the winner. 
        return a.score > b.score;
    return strcmp(a.word, b.word) < 0;
}

// Swap helper (swap 2 words in the heap)
void swap(Entry *a, Entry *b) {
    Entry temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify for max-heap
// Keep the heap in order by comparing parents and children.
void heapify(heapStruct *heap, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < heap->size && !compareEntries(heap->heapArray[largest], heap->heapArray[left]))
        largest = left;

    if (right < heap->size && !compareEntries(heap->heapArray[largest], heap->heapArray[right]))
        largest = right;

    if (largest != i) {
        swap(&heap->heapArray[i], &heap->heapArray[largest]);
        heapify(heap, largest);
    }
}

// Build heap from your list of words
void buildHeap(heapStruct *heap) {
    for (int i = heap->size / 2 - 1; i >= 0; i--)
        heapify(heap, i);
}

// Extract max
// Removes the highest priority word from the heap, and readjusts the heap.
Entry extractMax(heapStruct *heap) {
    Entry root = heap->heapArray[0];
    heap->heapArray[0] = heap->heapArray[heap->size - 1];
    heap->size--;
    heapify(heap, 0);
    return root;
}

// Validate word based on guesses and feedback
int isValidWord(const char *word, char guesses[][6], char feedbacks[][6], int m) {
    int green[WORD_LEN];                  // GREEN; correct letters in exact spots
    int yellow[WORD_LEN][ALPHABET] = {0}; // YELLOW; letters not allowed at these spots
    int gray[ALPHABET] = {0};             // GRAY; letters not in that word
    int minCount[ALPHABET] = {0};         // Minimum number of occurrences for letters

    for (int i = 0; i < WORD_LEN; i++) {
    green[i] = -1;  // set each position to -1 (unknown)
}

    // Process all feedbacks
    for (int g = 0; g < m; g++) {
        for (int i = 0; i < WORD_LEN; i++) {
            char c = guesses[g][i];
            int idx = c - 'a';

            if (feedbacks[g][i] == 'G') {
                green[i] = c;
            } else if (feedbacks[g][i] == 'Y') {
                yellow[i][idx] = 1;
                minCount[idx]++;
            } else if (feedbacks[g][i] == 'B') {
                gray[idx] = 1;
            }
        }
    }

    // Check word through all of the conditions
    int letterCount[ALPHABET] = {0};
    for (int i = 0; i < WORD_LEN; i++) {
        char c = word[i];
        int idx = c - 'a';
        letterCount[idx]++;

        if (green[i] != -1 && word[i] != green[i]) return 0;
        if (yellow[i][idx]) return 0;
    }
// Final checks 
    for (int i = 0; i < ALPHABET; i++) {
        if (minCount[i] > 0 && letterCount[i] < minCount[i])
            return 0;
        if (gray[i] && minCount[i] == 0 && letterCount[i] > 0)
            return 0;
    }

    return 1;  // If all checks pass, the word is valid!
}

// Free heap memory
void freeHeap(heapStruct *heap) {
    free(heap->heapArray);
}

//Done