# static_algs
motivation: using only the compiler, statically solve a sudoku puzzle.

# rough plan (updated: 2018apr17)
1. start by making a static sort alg
2. modify to support empty types
3. create derivations that check for latin squre conditions, start with 4x4
4. devise a way to explore the latin square state space in a DFS with backup
5. improve stack usage to support latin square size of 9x9
6. make into full blown sudoku puzzle solver
