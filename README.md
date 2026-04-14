# Wordle-Heap-C
Wordle solver in C using heap data structure to rank valid words based on letter frequency and feedback constraints.
# Wordle Heap Solver (C)

This project implements a Wordle-style solver using a max heap data structure in C. It filters valid candidate words based on previous guesses and feedback, then ranks them using letter frequency scoring.

## Features

- Filters valid words based on Wordle feedback (G, Y, B)
- Scores words using English letter frequency
- Uses a max heap to prioritize best guesses
- Outputs words from highest to lowest score

## How It Works

1. Reads a dictionary of words
2. Processes previous guesses and feedback
3. Filters valid candidate words
4. Scores each word based on letter frequency
5. Builds a max heap
6. Outputs words ranked by score

## Compilation

```bash
gcc wordleheap.c -o wordleheap
