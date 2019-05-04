# QueensGA

## The problem
> Place N queens on an NxN chess board such that no queen threatens the other.
> Source: https://en.wikipedia.org/wiki/Eight_queens_puzzle

## Terms
* Gene - an arrangement of n queens on the board.
* Generation - a group of genes at a certain time.
* Rank of a gene - how close is the arrangement to a solution.
* Elite - top % of the highest ranked genes.
* Mating - two genes produce a new gene for the next generation.
* Mutation - "nature like" transforms an entire generation by a small change.

## Solution
Following Genetic Algorithm principles I rank genes according to their closeness to a solution which is based on the amount of threatening queens in the gene.
Next I choose the Elite to automatically continue to be in the next generation.
The rest of the generation is being mated and produce the next generation.
The mating process includes a selection algorithm which uses a Roullette Selection algorithm to choose with high probability stronger genes as parents to continue to the next generation. -A gene can be a parent of multiple children.
Every few generations I perform a mutation, thus assuring randomness and effectiveness.

## Can be described as:
1. Create generation.
2. Choose Elite.
3. Roullette selection mating.
4. Mutate (if needed).
5. Go to 2 with new generation. Stop when goal is reached.

## How to run:
Run Queens.exe 
