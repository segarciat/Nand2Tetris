# Chapter 9: High Level Language

The objective of this project is to create an interactive application in order to get acquainted with the Jack language. The point is to prepare for the construction of the Jack-on-Hack compiler as well as the Jack operating system (standard library).

## Context

_Jack_ is a simple object-oriented programming language that does not support inheritance. It comes with a standard library that allows a user to do simple math, output text to the screen, draw pixels, read keyboard inputs, and manage memory, among other things.

## Dodge

The provided sample Jack application was a game called `SquareGame` that allowed a user to move a square around the screen as well as grow and shrink it. Using the base code in this game, I developed a simple game called _Dodge_.

A player in the _Dodge_ is a square that can move left, right, and can jump. The objective is for the player to dodge an incoming mob who starts at the right end of the map and is moving continuously left. When the mob reaches the left end, its position is reset to the right. The game ends when the player and the mob collide.
