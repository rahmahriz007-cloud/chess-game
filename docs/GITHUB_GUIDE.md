# Putting the project on GitHub (step by step)

## 1. One person creates the repository

1. Go to <https://github.com/new>
2. Name it `chess-game`, choose **Public**, and leave "Add a README" **unticked** (the project already has one).
3. Click **Create repository** and copy the URL, e.g. `https://github.com/YOUR-USERNAME/chess-game.git`

Then open a terminal inside the project folder:

```bash
git init
git branch -M main
git add .
git commit -m "Initial commit: complete C++ chess game"
git remote add origin https://github.com/YOUR-USERNAME/chess-game.git
git push -u origin main
```

After the push, replace `YOUR-USERNAME` in `README.md` (the build badge) with the real name.

## 2. Add the other two members

Repository → **Settings → Collaborators → Add people**.  Each member then runs:

```bash
git clone https://github.com/YOUR-USERNAME/chess-game.git
```

## 3. Show each member's real contribution (recommended)

Professors often look at the commit history.  A clean way to make it match the
report is for each member to commit *their own* part, from their own account:

| Member | Branch | Files they commit |
|---|---|---|
| Insia Fatima | `insia/piece-classes` | `include/Piece.h`, `include/King.h`, `Queen.h`, `Rook.h`, `Bishop.h`, `Knight.h`, `Pawn.h`, `include/PieceFactory.h`, `src/Piece.cpp`, `src/King.cpp` … `src/Pawn.cpp`, `src/PieceFactory.cpp` |
| Rahmah Rizwan | `rahmah/move-validation` | `src/BoardRules.cpp`, the `isValidMove()` / `attacksSquare()` / `canCastle()` parts of the piece `.cpp` files, `tests/test_chess.cpp` |
| Zainab Azram | `zainab/board-and-game` | `include/Board.h`, `include/Game.h`, `src/Board.cpp`, `src/Game.cpp`, `src/main.cpp`, `src/Demo.cpp` |

Example for one member:

```bash
git checkout -b zainab/board-and-game
git add include/Board.h include/Game.h src/Board.cpp src/Game.cpp src/main.cpp
git commit -m "Add Board management and Game logic"
git push -u origin zainab/board-and-game
```

Then open a **Pull Request** on GitHub and merge it.  Small, honest commits
(e.g. "Add castling rules", "Fix en passant bug") look much better than one
giant commit.

## 4. Before you present

* Make sure the green ✔ appears next to your last commit (the CI workflow builds and tests the game automatically).
* Add the report PDF in `docs/` (already included) and put the repository link on the report's cover page.
* Practise: `make demo`, then play a quick game with `moves e2`, a capture, `undo`, and a scholar's mate.
