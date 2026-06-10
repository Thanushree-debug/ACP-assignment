/*
 * 2D Graphics Editor using * and _
 * Supports: Circle, Rectangle, Line, Triangle
 * Operations: Add, Delete, Modify objects
 * Canvas stored as 2D character array
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 30
#define COLS 80
#define MAX_OBJECTS 50

/*  Canvas */
char canvas[ROWS][COLS];

void init_canvas() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = ' ';
}

void display_canvas() {
    /* Top border */
    printf("+");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");

    for (int r = 0; r < ROWS; r++) {
        printf("|");
        for (int c = 0; c < COLS; c++)
            printf("%c", canvas[r][c]);
        printf("|\n");
    }

    /* Bottom border */
    printf("+");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
}

/* Safe pixel setter */
void set_pixel(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

/* 
   Object Types
    */
typedef enum { SHAPE_NONE, SHAPE_CIRCLE, SHAPE_RECT, SHAPE_LINE, SHAPE_TRIANGLE } ShapeType;

typedef struct {
    ShapeType type;
    /* Generic params: meaning depends on shape type */
    int x1, y1;   /* start / center */
    int x2, y2;   /* end / corner / apex */
    int x3, y3;   /* third point for triangle */
    int radius;   /* for circle */
    char fill;    /* '*' or '_' */
    int active;
} Object;

Object objects[MAX_OBJECTS];
int obj_count = 0;

/* 
   Drawing Primitives
    */

/* Bresenham line */
void draw_line_pixels(int r1, int c1, int r2, int c2, char ch) {
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    int sr = (r1 < r2) ? 1 : -1;
    int sc = (c1 < c2) ? 1 : -1;
    int err = dr - dc;

    while (1) {
        set_pixel(r1, c1, ch);
        if (r1 == r2 && c1 == c2) break;
        int e2 = 2 * err;
        if (e2 > -dc) { err -= dc; r1 += sr; }
        if (e2 <  dr) { err += dr; c1 += sc; }
    }
}

/* Mid-point circle */
void draw_circle_pixels(int cr, int cc, int radius, char ch) {
    int x = 0, y = radius;
    int d = 1 - radius;

    while (x <= y) {
        set_pixel(cr + x, cc + y, ch);
        set_pixel(cr + x, cc - y, ch);
        set_pixel(cr - x, cc + y, ch);
        set_pixel(cr - x, cc - y, ch);
        set_pixel(cr + y, cc + x, ch);
        set_pixel(cr + y, cc - x, ch);
        set_pixel(cr - y, cc + x, ch);
        set_pixel(cr - y, cc - x, ch);
        if (d < 0)
            d += 2 * x + 3;
        else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void draw_rect_pixels(int r1, int c1, int r2, int c2, char ch) {
    for (int c = c1; c <= c2; c++) { set_pixel(r1, c, ch); set_pixel(r2, c, ch); }
    for (int r = r1; r <= r2; r++) { set_pixel(r, c1, ch); set_pixel(r, c2, ch); }
}

void draw_triangle_pixels(int r1, int c1, int r2, int c2, int r3, int c3, char ch) {
    draw_line_pixels(r1, c1, r2, c2, ch);
    draw_line_pixels(r2, c2, r3, c3, ch);
    draw_line_pixels(r3, c3, r1, c1, ch);
}

/* 
   Render all objects onto a fresh canvas
    */
void render_all() {
    init_canvas();
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        Object *o = &objects[i];
        switch (o->type) {
            case SHAPE_CIRCLE:
                draw_circle_pixels(o->y1, o->x1, o->radius, o->fill);
                break;
            case SHAPE_RECT:
                draw_rect_pixels(o->y1, o->x1, o->y2, o->x2, o->fill);
                break;
            case SHAPE_LINE:
                draw_line_pixels(o->y1, o->x1, o->y2, o->x2, o->fill);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle_pixels(o->y1, o->x1, o->y2, o->x2, o->y3, o->x3, o->fill);
                break;
            default: break;
        }
    }
}

/* 
   Add Objects
    */
void add_circle() {
    if (obj_count >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_CIRCLE;
    o.active = 1;
    o.fill   = '*';
    printf("  Center col (x): "); scanf("%d", &o.x1);
    printf("  Center row (y): "); scanf("%d", &o.y1);
    printf("  Radius        : "); scanf("%d", &o.radius);
    objects[obj_count++] = o;
    printf("Circle added as object #%d.\n", obj_count - 1);
}

void add_rectangle() {
    if (obj_count >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_RECT;
    o.active = 1;
    o.fill   = '*';
    printf("  Top-left  col (x1): "); scanf("%d", &o.x1);
    printf("  Top-left  row (y1): "); scanf("%d", &o.y1);
    printf("  Bot-right col (x2): "); scanf("%d", &o.x2);
    printf("  Bot-right row (y2): "); scanf("%d", &o.y2);
    objects[obj_count++] = o;
    printf("Rectangle added as object #%d.\n", obj_count - 1);
}

void add_line() {
    if (obj_count >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_LINE;
    o.active = 1;
    o.fill   = '_';
    printf("  Start col (x1): "); scanf("%d", &o.x1);
    printf("  Start row (y1): "); scanf("%d", &o.y1);
    printf("  End   col (x2): "); scanf("%d", &o.x2);
    printf("  End   row (y2): "); scanf("%d", &o.y2);
    objects[obj_count++] = o;
    printf("Line added as object #%d.\n", obj_count - 1);
}

void add_triangle() {
    if (obj_count >= MAX_OBJECTS) { printf("Max objects reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_TRIANGLE;
    o.active = 1;
    o.fill   = '*';
    printf("  Vertex 1 col (x1): "); scanf("%d", &o.x1);
    printf("  Vertex 1 row (y1): "); scanf("%d", &o.y1);
    printf("  Vertex 2 col (x2): "); scanf("%d", &o.x2);
    printf("  Vertex 2 row (y2): "); scanf("%d", &o.y2);
    printf("  Vertex 3 col (x3): "); scanf("%d", &o.x3);
    printf("  Vertex 3 row (y3): "); scanf("%d", &o.y3);
    objects[obj_count++] = o;
    printf("Triangle added as object #%d.\n", obj_count - 1);
}

/* 
   List / Delete / Modify
    */
const char *shape_name(ShapeType t) {
    switch (t) {
        case SHAPE_CIRCLE:   return "Circle";
        case SHAPE_RECT:     return "Rectangle";
        case SHAPE_LINE:     return "Line";
        case SHAPE_TRIANGLE: return "Triangle";
        default:             return "Unknown";
    }
}

void list_objects() {
    int any = 0;
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        any = 1;
        Object *o = &objects[i];
        printf("  [%d] %s  fill='%c'", i, shape_name(o->type), o->fill);
        switch (o->type) {
            case SHAPE_CIRCLE:
                printf("  center=(%d,%d) radius=%d", o->x1, o->y1, o->radius);
                break;
            case SHAPE_RECT:
                printf("  (%d,%d)->(%d,%d)", o->x1, o->y1, o->x2, o->y2);
                break;
            case SHAPE_LINE:
                printf("  (%d,%d)->(%d,%d)", o->x1, o->y1, o->x2, o->y2);
                break;
            case SHAPE_TRIANGLE:
                printf("  (%d,%d) (%d,%d) (%d,%d)", o->x1, o->y1, o->x2, o->y2, o->x3, o->y3);
                break;
            default: break;
        }
        printf("\n");
    }
    if (!any) printf("  (no objects)\n");
}

void delete_object() {
    list_objects();
    int id;
    printf("Enter object number to delete (-1 to cancel): ");
    scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid id.\n"); return;
    }
    objects[id].active = 0;
    printf("Object #%d deleted.\n", id);
}

void modify_object() {
    list_objects();
    int id;
    printf("Enter object number to modify (-1 to cancel): ");
    scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid id.\n"); return;
    }
    Object *o = &objects[id];
    printf("Current fill char '%c'. New fill char (* or _): ", o->fill);
    char ch[4];
    scanf("%s", ch);
    if (ch[0] == '*' || ch[0] == '_') o->fill = ch[0];

    switch (o->type) {
        case SHAPE_CIRCLE:
            printf("New center col (x) [%d]: ", o->x1); scanf("%d", &o->x1);
            printf("New center row (y) [%d]: ", o->y1); scanf("%d", &o->y1);
            printf("New radius        [%d]: ", o->radius); scanf("%d", &o->radius);
            break;
        case SHAPE_RECT:
            printf("New top-left  col (x1) [%d]: ", o->x1); scanf("%d", &o->x1);
            printf("New top-left  row (y1) [%d]: ", o->y1); scanf("%d", &o->y1);
            printf("New bot-right col (x2) [%d]: ", o->x2); scanf("%d", &o->x2);
            printf("New bot-right row (y2) [%d]: ", o->y2); scanf("%d", &o->y2);
            break;
        case SHAPE_LINE:
            printf("New start col (x1) [%d]: ", o->x1); scanf("%d", &o->x1);
            printf("New start row (y1) [%d]: ", o->y1); scanf("%d", &o->y1);
            printf("New end   col (x2) [%d]: ", o->x2); scanf("%d", &o->x2);
            printf("New end   row (y2) [%d]: ", o->y2); scanf("%d", &o->y2);
            break;
        case SHAPE_TRIANGLE:
            printf("New v1 col [%d]: ", o->x1); scanf("%d", &o->x1);
            printf("New v1 row [%d]: ", o->y1); scanf("%d", &o->y1);
            printf("New v2 col [%d]: ", o->x2); scanf("%d", &o->x2);
            printf("New v2 row [%d]: ", o->y2); scanf("%d", &o->y2);
            printf("New v3 col [%d]: ", o->x3); scanf("%d", &o->x3);
            printf("New v3 row [%d]: ", o->y3); scanf("%d", &o->y3);
            break;
        default: break;
    }
    printf("Object #%d modified.\n", id);
}

/* 
   Demo: pre-load sample shapes
    */
void load_demo() {
    /* Circle: center (20,14), r=7 */
    objects[obj_count++] = (Object){SHAPE_CIRCLE, 20,14, 0,0, 0,0, 7, '*', 1};
    /* Rectangle: (35,5) -> (55,20) */
    objects[obj_count++] = (Object){SHAPE_RECT, 35,5, 55,20, 0,0, 0, '_', 1};
    /* Line: (5,25) -> (75,25) */
    objects[obj_count++] = (Object){SHAPE_LINE, 5,25, 75,25, 0,0, 0, '_', 1};
    /* Triangle: (40,2) (20,27) (60,27) */
    objects[obj_count++] = (Object){SHAPE_TRIANGLE, 40,2, 20,27, 60,27, 0, '*', 1};
    printf("Demo shapes loaded (objects 0-3).\n");
}

/* 
   Main Menu
    */
void print_menu() {
    printf("\n+------------------------------+\n");
    printf("|   2D Graphics Editor (C)     |\n");
    printf("+------------------------------+\n");
    printf("|  1. Display canvas           |\n");
    printf("|  2. Add circle               |\n");
    printf("|  3. Add rectangle            |\n");
    printf("|  4. Add line                 |\n");
    printf("|  5. Add triangle             |\n");
    printf("|  6. List objects             |\n");
    printf("|  7. Delete object            |\n");
    printf("|  8. Modify object            |\n");
    printf("|  9. Clear canvas             |\n");
    printf("|  0. Exit                     |\n");
    printf("+------------------------------+\n");
    printf("Choice: ");
}

int main() {
    init_canvas();
    load_demo();
    render_all();

    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: render_all(); display_canvas(); break;
            case 2: add_circle();    break;
            case 3: add_rectangle(); break;
            case 4: add_line();      break;
            case 5: add_triangle();  break;
            case 6: list_objects();  break;
            case 7: delete_object(); break;
            case 8: modify_object(); break;
            case 9:
                obj_count = 0;
                init_canvas();
                printf("Canvas cleared.\n");
                break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
