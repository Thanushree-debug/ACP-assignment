# 2D Graphics Editor in C

A terminal-based 2D graphics editor using `*` and `_` characters, built entirely in C.

## Features

- **Canvas**: 30×80 character array displayed in the terminal
- **Shapes**: Circle, Rectangle, Line, Triangle
- **Operations**: Add, Delete, Modify objects
- **Fill characters**: `*` (default for most shapes) and `_` (default for lines/rectangles)

## Algorithms Used

| Shape | Algorithm |
|-------|-----------|
| Circle | Mid-point circle algorithm |
| Line | Bresenham's line algorithm |
| Rectangle | Border drawing with top/bottom/left/right edges |
| Triangle | Three Bresenham lines connecting three vertices |

## How to Compile & Run

```bash
gcc -o graphics_editor graphics_editor.c -lm
./graphics_editor
```

## Menu Options

```
1. Display canvas
2. Add circle
3. Add rectangle
4. Add line
5. Add triangle
6. List objects
7. Delete object
8. Modify object
9. Clear canvas
0. Exit
```

## Coordinate System

- **x** = column (left to right, 0–79)
- **y** = row (top to bottom, 0–29)

## Sample Output

On startup, four demo shapes are loaded automatically:
- A circle (center 20,14 radius 7) drawn with `*`
- A rectangle (35,5)→(55,20) drawn with `_`
- A horizontal line drawn with `_`
- A triangle with vertices at (40,2), (20,27), (60,27) drawn with `*`

## File Structure

```
graphics_editor.c   — complete single-file source
README.md           — this file
```
