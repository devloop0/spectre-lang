import <"std/syscall">

const int DIM = 8;

func int print_board(int* board) {
  using std::syscall::direct_write;
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      if (board[i*DIM + j] as bool)
        direct_write(1, "Q ", 2);
      else
        direct_write(1, ". ", 2);
    }
    direct_write(1, "\n", 1);
  }
  direct_write(1, "\n\n", 2);
}

func int conflict(int* board, int row, int col) {
  for (int i = 0; i < row; i++) {
    if (board[i*DIM + col] as bool)
      return 1;
    int j = row - i;
    if (0 < col - j + 1)
      if (board[i*DIM + col - j] as bool)
        return 1;
    if (col + j < 8)
      if (board[i*DIM + col + j] as bool)
        return 1;
  }
  return 0;
}


func int solve(int* board, int row) {
  if (row == 8) {
    print_board(board);
    return 0;
  }
  for (int i = 0; i < 8; i++) {
    if (!conflict(board, row, i) as bool) {
      board[row*DIM + i] = 1;
      solve(board, row + 1);
      board[row*DIM + i] = 0;
    }
  }
}

func int main() {
  int* board = stk int(8 * 8);
  for (int i = 0; i < 64; i++)
    board[i] = 0;
  solve(board, 0);
}
