# What is this game?

Dog is a Swiss board game similar to the Canadian [Tock](https://en.wikipedia.org/wiki/Tock).

## Rules

The rules can be found [here](http://www.dogspiel.info/images/pdfs/regeln/rules.pdf) or [here](https://www.connexxion24.com/downloads/anleitungen-braendi-dog/Spielregeln-braendi-dog-englisch.pdf). The latter version by the Brändi foundation also includes the so-called _Canadian rule_, which allows a seven to be split not only among the pieces of the player that plays it but also among the pieces of their teammate. This library implements the Canadian rule, but it is optional.


# What is this library for?

Implementations of the game (mainly of Dog, without the Canadian rule) exist, but they are either closed-source [[1](http://www.brettspielwelt.de/Hilfe/Anleitungen/Dog/), [2](https://dogapp.ch/)] or incomplete [[3](https://github.com/innerjoin/dog2go)]. This library is meant to be a complete, correct implementation of the game, which can be used to implement a website for people to play on or to develop an AI in a future project.


# Usage

To see how to use libdog, check out the [usage demo](demo/usage.cpp).


# Run demo

| :warning: GCC 10 required |
|---------------------------|
Tested on Fedora 33 with GCC 10.3.1 20210422 (Red Hat 10.3.1-1).

First, clone the repository.
```
$ git clone https://github.com/leopoldsedev/libdog.git
```

Then, navigate into the project directory and compile the project (cmake is used to build the project, but a Makefile is provided for ease-of-use).
```
$ cd libdog
$ make build
```

If compilation was successful, the demos can be run.
```
$ make run_demo_usage
$ make run_demo_random
```

The tests can be run as well (requires [gtest and gmock](https://github.com/google/googletest)).
```
$ make test
```


# Notation

To give the game some formality and for development/testing purposes, a game notation to describe board states and to specify player actions has been developed.


## Board state

The following is an ASCII representation of the board.
```
0    0 0 0 0  0 . . . . .               3
     3 2 1 0  1 . \     .
              2 .  . 0  .
              3 .  . 1  .               3
                .  . 2  .               3
              .    . 3    .             3
            .               .           3
          .                   .
. . . . .                       . . . . .
.                                     / .
.  . . . .                     . . . .  .
. /                                     .
. . . . .                       . . . . .
          .                   .
1           .               .
1             .      .    .
1               .    .  .
1               .    .  .
                .    .  .
                .     \ .
1               . . . . .    2 2 2 2    2
```
The number in each corner marks which part of the board (and consequently which finish) belongs to which player. The indices of kennel (0-3), path (0-63), and finish (0-3) are indicated as well. The remaining numbers each represent a player's piece.

The notation to describe a board state has the following format.
```
[pieces of player 0]|[pieces of player 1]|[pieces of player 2]|[pieces of player 3]
```

For each player, the positions of their pieces are denoted as a sequence of `P[index]` and `F[index]`. `P` means the piece is currently on the path on position `[index]`, and `F` means the piece is on the corresponding index in the finish. If a piece is not contained in the sequence it means it is in the kennel.
For example, `|||` would denote the initial state of a game (all pieces in their respective kennel) and `F0F1F2F3||F0F1F2F3|` would denote a concluded game where player 0 and player 2 won.

For each piece specified in the sequence, a star (`*`) may be added to denote that the piece is currently blocking other pieces. In a game of Dog, a piece can only be blocking when it is on the starting position (`P0`, `P16`, `P32`, `P48`), but the notation allows a piece to be marked as blocking on any position.

As an example, `P0*F1|P14P16|F0F3|P36P48*` denotes the following position.
```
0    0 0 . .  0*0 . . . .               3
     3 2 1 0  1 . \     .
              2 .  . 0  .
              3 .  0 1  .               3
                .  . 2  .               3
              .    . 3    .             .
            .               .           .
          .                   .         *
. . . . .                       . . . . 3
.                                     / .
1  . . . .                     . . . .  .
. /                                     .
1 . . . .                       . . . . .
          .                   .
.           .               .
.             .      2    .
1               .    .  3
1               .    .  .
                .    2  .
                .     \ .
1               . . . . .    . . 2 2    2
```

In this ASCII representation, if a piece is currently blocking other pieces, there is a star next to the piece.


## Cards

Dog is played with two 55 card decks (standard 52 card deck + 3 jokers). Suits are irrelevant for the gameplay, which means it is sufficient to specify the rank of a card. This library uses the standard card rank shorthands to denote certain cards, except that in addition, a joker is denoted as `X`. See the table below for the correspondences.

 Card      | Ace   | Two   | Three | Four  | Five  | Six   | Seven | Eight | Nine  | Ten   | Jack  | Queen | King  | Joker |
----------:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
 Shorthand | `A`   | `2`   | `3`   | `4`   | `5`   | `6`   | `7`   | `8`   | `9`   | `T`   | `J`   | `Q`   | `K`   | `X`   |

## Piece specification

Most actions affect specific pieces on the board. To make specifying a piece easier, a piece order or _rank_ from 0 to 3 for each player is defined. The rank is determined by the distance of the piece to the furthest slot in the player's finish (`F3`). The piece that is the closest to that slot has rank 0, the next closest has rank 1, and the one that is furthest away has rank 3.
As an example, consider the following board state.

```
0    0 . . .  0*0 . . . 0               3
     3 2 1 0  1 . \     .
              2 .  . 0  .
              3 .  . 1  .               3
                .  . 2  .               3
              .    0 3    .             3
            .               .           3
          .                   .
. . . . .                       . . . . .
.                                     / .
.  . . . .                     . . . .  .
. /                                     .
. . . . .                       . . . . .
          .                   .
1           .               .
1             .      .    .
1               .    .  .
1               .    .  .
                .    .  .
                .     \ .
1               . . . . .    2 2 2 2    2
```

In this case, the piece on `F3` has rank 0, the piece on `P60` has rank 1, the piece `P0` has rank 2 and the piece in the kennel has rank 3. Note that if the piece on `P0` was not blocking, it could enter right away and would thus have rank 1 instead of the piece on `P60`.

With this convention, a piece can be unambiguously specified by a pair of player ID and rank.


## Actions

There are six types of actions in the game: `give`, `discard`, `start`, `move`, `swap`, and `move multiple`. For each type of action, there is a corresponding notation format.
Like in the algebraic notation of chess, the notation does not include the player playing the action.


### Give
**Cards allowed:** all

At the start of each round, all players exchange a single card with their teammate. This is done using a give action. It is notated as
```
G[card]
```
**Example:** `G2` means give a `2` to the teammate.


### Discard
**Cards allowed:** all

If a player cannot legally play any of their cards, they must play a `discard` action, discarding a card without effect. Similar to the `give` action, it is notated as
```
D[card]
```
**Example:** `D2` means discard a `2`.


### Start

**Cards allowed:** `A`, `K`

**Description:** Moves a piece from the kennel to the starting position of the player playing the action.

**Notation:**
```
[card]#
```

**Example:** `A#` means move a piece out of the kennel.


### Move

**Cards allowed:** `A`, `2`, `3`, `4`, `5`, `6`, `8`, `9`, `T`, `Q`, `K`

**Description:** Moves a piece a certain number of steps forwards or backwards.

**Notation:**
```
[card][piece rank]
```
The piece is specified only by its rank and the player ID is implicitly given by the player who is playing the action.

**Example:** `50` means move the piece with rank 0 5 steps forwards.

Some cards that allow alternative actions, namely `A` (moving forwards 1 step instead of 11) and `4` (moving 4 steps backwards instead of forwards).
To activate the alternative action, an `'` is added after the card.

**Example:** `4'0` means move the piece with rank 0 4 steps backwards.

By default, a moving piece will always enter the player's finish if possible. If the finish is blocked, it will continue on the path. However, if the finish is not blocked and the player wishes to move past the finish anyway, this is denoted by adding a `-` after the piece. The notation allows this addition even if it would have no effect (e.g. because the finish is blocked anyway, or the piece is not within range to enter the finish).

**Example:** `80-` means move the piece with rank 0 8 steps forwards, explicitly avoiding the finish.


### Swap

**Cards allowed:** `J`

**Description:** Switches the position of two pieces.

**Notation:**
```
[card][piece rank 1][player ID][piece rank 2]
```
The first piece is specified only by its rank and the player ID is implicitly given by the player who is playing the action.  The second piece is specified by a player ID and rank.

**Example:** `J120` means swap the piece with rank 0 with the piece with rank 0 of player 2.


### Move multiple
**Cards allowed:** `7`

**Description:** Moves a set of pieces a certain number of steps forward.

**Notation:**
```
[card][move list]
```
The move list has the format:
```
[piece rank 1][step count 1][piece rank 2][step count 2]...
```
The piece is specified only by its rank and the player ID is implicitly given by the player who is playing the action. The step counts have the add up to the card value.

**Example:** `7022213` means to move the piece with rank 0 2 steps, the piece with rank 2 2 steps, and the piece with rank 1 3 steps (in the specified order).

With the Canadian rule, it is also possible to move pieces of the teammate. To specify a piece of the teammate, a `'` is added after the rank. In that case, the player ID is implicitly given by the teammate of the player who is playing the action.

**Example:** `7022'213` means to move the piece with rank 0 of the player playing the action 2 steps, the piece with rank 2 of the teammate 2 steps, and the piece with rank 1 of the player playing the action 3 steps.

Lastly, as for the move actions, it is possible to explicitly avoid the finish by adding a `-` after the step count.

**Example:** `702-2'5-` means to move the piece with rank 0 of the player playing the action 2 steps, and the piece with rank 1 of the teammate 5 steps, explicitly avoiding the finish in both times.


### Joker

**Cards allowed:** `X`

**Description:** Can be used to play any other action.

**Notation:**
```
[card][action]
```
The action can be any **other** action as described previously.

**Example:** `XJ120` means use the joker to play a swap.


# Limitations

This library has the following limitations.

* Querying legal moves can be rather slow depending on the game state. This is because there may be a lot of possibilities of split a `7` between pieces. So far the focus of the implementation has been correctness and there is much headroom for optimization. As a result, a discard action could also turn out to be a bit slow since to determine if discarding is legal in a given position, the list of legal moves has to be queried.
* The list of possible moves currently does not include moves with a `7` that avoid the finish. This technically makes this implementation incomplete. However, this detail was omitted because it would slow down generating the list of possible actions even further and it is a very rare action anyway. Once the library is faster this detail will be fixed.
* Only the 4 player variant of dog is implemented (2, 3, 5, and 6 player variants exist).
