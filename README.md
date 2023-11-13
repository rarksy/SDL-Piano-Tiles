# SDL Piano Tiles

# About
A simple piano tiles game made using SDL2 in C++
Made in roughly 2 days
Made as a simple introduction to 2D game development without the use of a game engine

# Contents
a very barebones main menu, with key press handling for changing different game settings including max rows and max tiles
title updates during game to show both the current score aswell as the current tile scrolling speed respectively
highscores are present but do not persist between application lifetimes

# Notes
not the cleanest code i can write, i intend to do a final update to the project to fix some of the codes styling aswell as a few minor bugs such as,

# Bugs
inconsistent Tile lineups by roughly a pixel or 2 on SOME tiles (not all)
clicking on an already clicked tile counts as a fail (this is more just a lacking feature as its only an extra if statement to add this)
