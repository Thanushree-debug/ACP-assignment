# 2D Graphics Editor in C

A menu-driven terminal-based 2D graphics editor using `*` and `_` ASCII characters.

## How to Compile

```bash
gcc graphics_editor.c -o graphics_editor -lm
```

## How to Run

```bash
.\graphics_editor.exe        # Windows
./graphics_editor            # Linux/Mac
```

## Menu Options

| Key | Action |
|-----|--------|
| 1 | Display all shapes (opens new window) |
| 2 | Add circle |
| 3 | Add rectangle |
| 4 | Add line |
| 5 | Add triangle |
| 6 | List all objects |
| 7 | Delete an object |
| 8 | Modify an object |
| 9 | Clear everything |
| 0 | Exit |

## Coordinate System

- x = column → 0 to 79 (left to right)
- y = row    → 0 to 29 (top to bottom)

## Display Behaviour

When you press **1 (Display)**:
- A **new cmd window** opens automatically
- Each shape is shown on its **own separate canvas**
- Shapes are NOT merged or overlapped
- Main terminal keeps full history of your inputs
- Close the popup window when done

## Example Coordinates

| Shape | Good values |
|-------|-------------|
| Circle | x=40, y=14, radius=8 |
| Rectangle | x1=10, y1=5, x2=60, y2=25 |
| Line | x1=0, y1=15, x2=79, y2=15 |
| Triangle | x1=40,y1=2 / x2=10,y2=27 / x3=70,y3=27 |

## Algorithms Used

- **Circle** — Mid-point circle algorithm
- **Line** — Bresenham's line algorithm  
- **Rectangle** — Border edge drawing
- **Triangle** — Three Bresenham lines

## AI Tool Used

Claude (claude.ai) — see `prompt.txt` for full interaction log.
