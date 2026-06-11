/*
 * 
 *  2D Graphics Editor — ASCII Art using * and _
 *  Language : C
 *  Features : Circle, Rectangle, Line, Triangle
 *             Add / Delete / Modify / Display objects
 *  Display  : Each shape shown on its OWN canvas in a NEW window
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ── Canvas dimensions ── */
#define ROWS 30
#define COLS 80

/* ── Max number of objects ── */
#define MAX_OBJECTS 50

/* ── Temp file used to pass canvas to popup window ── */
#define TEMP_FILE "canvas_display.txt"

/* 
   SHAPE TYPES
    */
typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECT,
    SHAPE_LINE,
    SHAPE_TRIANGLE
} ShapeType;

/* 
   OBJECT STRUCTURE
    */
typedef struct {
    ShapeType type;
    int x1, y1;       /* start point / center / vertex1 */
    int x2, y2;       /* end point / corner / vertex2   */
    int x3, y3;       /* vertex3 (triangle only)         */
    int radius;       /* circle only                     */
    char fill;        /* '*' or '_'                      */
    int active;       /* 1 = exists, 0 = deleted         */
} Object;

/* ── Global object list ── */
Object objects[MAX_OBJECTS];
int obj_count = 0;

/* 
   CANVAS
    */
char canvas[ROWS][COLS];

/* Fill canvas with spaces */
void init_canvas() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = ' ';
}

/* Safe pixel write */
void set_pixel(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

/* 
   DRAWING ALGORITHMS
    */

/* Bresenham's line algorithm */
void draw_line(int r1, int c1, int r2, int c2, char ch) {
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

/* Mid-point circle algorithm */
void draw_circle(int cr, int cc, int radius, char ch) {
    int x = 0, y = radius, d = 1 - radius;
    while (x <= y) {
        set_pixel(cr+x, cc+y, ch); set_pixel(cr+x, cc-y, ch);
        set_pixel(cr-x, cc+y, ch); set_pixel(cr-x, cc-y, ch);
        set_pixel(cr+y, cc+x, ch); set_pixel(cr+y, cc-x, ch);
        set_pixel(cr-y, cc+x, ch); set_pixel(cr-y, cc-x, ch);
        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; y--; }
        x++;
    }
}

/* Rectangle — only border */
void draw_rect(int r1, int c1, int r2, int c2, char ch) {
    for (int c = c1; c <= c2; c++) { set_pixel(r1, c, ch); set_pixel(r2, c, ch); }
    for (int r = r1; r <= r2; r++) { set_pixel(r, c1, ch); set_pixel(r, c2, ch); }
}

/* Triangle — three lines */
void draw_triangle(int r1,int c1,int r2,int c2,int r3,int c3,char ch) {
    draw_line(r1,c1, r2,c2, ch);
    draw_line(r2,c2, r3,c3, ch);
    draw_line(r3,c3, r1,c1, ch);
}

/* Draw a single object onto the current canvas */
void render_object(Object *o) {
    switch (o->type) {
        case SHAPE_CIRCLE:
            draw_circle(o->y1, o->x1, o->radius, o->fill);
            break;
        case SHAPE_RECT:
            draw_rect(o->y1, o->x1, o->y2, o->x2, o->fill);
            break;
        case SHAPE_LINE:
            draw_line(o->y1, o->x1, o->y2, o->x2, o->fill);
            break;
        case SHAPE_TRIANGLE:
            draw_triangle(o->y1,o->x1, o->y2,o->x2, o->y3,o->x3, o->fill);
            break;
    }
}

/* 
   HELPER — shape name string
    */
const char *shape_name(ShapeType t) {
    switch (t) {
        case SHAPE_CIRCLE:   return "CIRCLE";
        case SHAPE_RECT:     return "RECTANGLE";
        case SHAPE_LINE:     return "LINE";
        case SHAPE_TRIANGLE: return "TRIANGLE";
        default:             return "UNKNOWN";
    }
}

/* 
   WRITE canvas + header to file  (used for popup window)
    */
void write_canvas_to_file(FILE *f, const char *header) {
    fprintf(f, "\n  %s\n", header);
    fprintf(f, "  +");
    for (int c = 0; c < COLS; c++) fprintf(f, "-");
    fprintf(f, "+\n");
    for (int r = 0; r < ROWS; r++) {
        fprintf(f, "  |");
        for (int c = 0; c < COLS; c++) fprintf(f, "%c", canvas[r][c]);
        fprintf(f, "|\n");
    }
    fprintf(f, "  +");
    for (int c = 0; c < COLS; c++) fprintf(f, "-");
    fprintf(f, "+\n\n");
}

/* 
   DISPLAY — each shape on its own canvas, opened in new window
   ============================================================ */
void display_all() {
    /* Count active objects */
    int any = 0;
    for (int i = 0; i < obj_count; i++)
        if (objects[i].active) { any = 1; break; }

    if (!any) {
        printf("  No shapes to display.\n");
        return;
    }

    /* Write all shapes (each on own canvas) to temp file */
    FILE *f = fopen(TEMP_FILE, "w");
    if (!f) { printf("  Error: cannot create temp file.\n"); return; }

    fprintf(f, "============================================================\n");
    fprintf(f, "  2D GRAPHICS EDITOR — SHAPE DISPLAY\n");
    fprintf(f, "============================================================\n");

    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;

        /* Fresh canvas for this shape only */
        init_canvas();
        render_object(&objects[i]);

        /* Build header string */
        char header[80];
        sprintf(header, "Shape [%d] : %s", i, shape_name(objects[i].type));

        write_canvas_to_file(f, header);
        fprintf(f, "------------------------------------------------------------\n");
    }

    fprintf(f, "\n  Press any key to close this window...\n");
    fclose(f);

    /* Open in a NEW cmd window */
    system("start cmd /c \"type canvas_display.txt & pause\"");

    printf("  >> Display window opened. Continue working in this window.\n");
}

/* 
   ADD FUNCTIONS
    */
void add_circle() {
    if (obj_count >= MAX_OBJECTS) { printf("  Object limit reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_CIRCLE;
    o.active = 1;
    o.fill   = '*';
    printf("  Enter center column x (0-%d): ", COLS-1); scanf("%d", &o.x1);
    printf("  Enter center row    y (0-%d): ", ROWS-1); scanf("%d", &o.y1);
    printf("  Enter radius             : "); scanf("%d", &o.radius);
    objects[obj_count++] = o;
    printf("  Circle added (ID = %d)\n", obj_count - 1);
}

void add_rectangle() {
    if (obj_count >= MAX_OBJECTS) { printf("  Object limit reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_RECT;
    o.active = 1;
    o.fill   = '*';
    printf("  Top-left  x1 (0-%d): ", COLS-1); scanf("%d", &o.x1);
    printf("  Top-left  y1 (0-%d): ", ROWS-1); scanf("%d", &o.y1);
    printf("  Bot-right x2 (0-%d): ", COLS-1); scanf("%d", &o.x2);
    printf("  Bot-right y2 (0-%d): ", ROWS-1); scanf("%d", &o.y2);
    /* auto-fix if user swaps corners */
    if (o.x1 > o.x2) { int t = o.x1; o.x1 = o.x2; o.x2 = t; }
    if (o.y1 > o.y2) { int t = o.y1; o.y1 = o.y2; o.y2 = t; }
    objects[obj_count++] = o;
    printf("  Rectangle added (ID = %d)\n", obj_count - 1);
}

void add_line() {
    if (obj_count >= MAX_OBJECTS) { printf("  Object limit reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_LINE;
    o.active = 1;
    o.fill   = '_';
    printf("  Start x1 (0-%d): ", COLS-1); scanf("%d", &o.x1);
    printf("  Start y1 (0-%d): ", ROWS-1); scanf("%d", &o.y1);
    printf("  End   x2 (0-%d): ", COLS-1); scanf("%d", &o.x2);
    printf("  End   y2 (0-%d): ", ROWS-1); scanf("%d", &o.y2);
    objects[obj_count++] = o;
    printf("  Line added (ID = %d)\n", obj_count - 1);
}

void add_triangle() {
    if (obj_count >= MAX_OBJECTS) { printf("  Object limit reached.\n"); return; }
    Object o = {0};
    o.type   = SHAPE_TRIANGLE;
    o.active = 1;
    o.fill   = '*';
    printf("  Vertex 1 x1 (0-%d): ", COLS-1); scanf("%d", &o.x1);
    printf("  Vertex 1 y1 (0-%d): ", ROWS-1); scanf("%d", &o.y1);
    printf("  Vertex 2 x2 (0-%d): ", COLS-1); scanf("%d", &o.x2);
    printf("  Vertex 2 y2 (0-%d): ", ROWS-1); scanf("%d", &o.y2);
    printf("  Vertex 3 x3 (0-%d): ", COLS-1); scanf("%d", &o.x3);
    printf("  Vertex 3 y3 (0-%d): ", ROWS-1); scanf("%d", &o.y3);
    objects[obj_count++] = o;
    printf("  Triangle added (ID = %d)\n", obj_count - 1);
}

/* 
   LIST OBJECTS
    */
void list_objects() {
    printf("\n  ---- Object List ----\n");
    int any = 0;
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        any = 1;
        Object *o = &objects[i];
        printf("  [%d] %-12s fill='%c'", i, shape_name(o->type), o->fill);
        if (o->type == SHAPE_CIRCLE)
            printf("  center=(%d,%d)  radius=%d", o->x1, o->y1, o->radius);
        else if (o->type == SHAPE_TRIANGLE)
            printf("  (%d,%d) (%d,%d) (%d,%d)", o->x1,o->y1, o->x2,o->y2, o->x3,o->y3);
        else
            printf("  (%d,%d) -> (%d,%d)", o->x1, o->y1, o->x2, o->y2);
        printf("\n");
    }
    if (!any) printf("  (no objects)\n");
    printf("  ---------------------\n");
}

/* 
   DELETE OBJECT
    */
void delete_object() {
    list_objects();
    int id;
    printf("  Enter ID to delete (-1 to cancel): ");
    scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("  Invalid ID.\n"); return;
    }
    objects[id].active = 0;
    printf("  Object [%d] deleted.\n", id);
}

/* 
   MODIFY OBJECT
    */
void modify_object() {
    list_objects();
    int id;
    printf("  Enter ID to modify (-1 to cancel): ");
    scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("  Invalid ID.\n"); return;
    }
    Object *o = &objects[id];
    char ch[4];
    printf("  Fill char (* or _) [current: %c]: ", o->fill);
    scanf("%s", ch);
    if (ch[0] == '*' || ch[0] == '_') o->fill = ch[0];

    printf("  Enter new coordinates:\n");
    switch (o->type) {
        case SHAPE_CIRCLE:
            printf("  Center x: "); scanf("%d", &o->x1);
            printf("  Center y: "); scanf("%d", &o->y1);
            printf("  Radius  : "); scanf("%d", &o->radius);
            break;
        case SHAPE_RECT:
            printf("  x1: "); scanf("%d", &o->x1);
            printf("  y1: "); scanf("%d", &o->y1);
            printf("  x2: "); scanf("%d", &o->x2);
            printf("  y2: "); scanf("%d", &o->y2);
            if (o->x1 > o->x2) { int t=o->x1; o->x1=o->x2; o->x2=t; }
            if (o->y1 > o->y2) { int t=o->y1; o->y1=o->y2; o->y2=t; }
            break;
        case SHAPE_LINE:
            printf("  x1: "); scanf("%d", &o->x1);
            printf("  y1: "); scanf("%d", &o->y1);
            printf("  x2: "); scanf("%d", &o->x2);
            printf("  y2: "); scanf("%d", &o->y2);
            break;
        case SHAPE_TRIANGLE:
            printf("  x1: "); scanf("%d", &o->x1);
            printf("  y1: "); scanf("%d", &o->y1);
            printf("  x2: "); scanf("%d", &o->x2);
            printf("  y2: "); scanf("%d", &o->y2);
            printf("  x3: "); scanf("%d", &o->x3);
            printf("  y3: "); scanf("%d", &o->y3);
            break;
    }
    printf("  Object [%d] modified.\n", id);
}

/* 
   MENU
    */
void print_menu() {
    printf("\n+--------------------------------+\n");
    printf("|    2D Graphics Editor (C)      |\n");
    printf("+--------------------------------+\n");
    printf("|  1. Display all shapes         |\n");
    printf("|  2. Add circle                 |\n");
    printf("|  3. Add rectangle              |\n");
    printf("|  4. Add line                   |\n");
    printf("|  5. Add triangle               |\n");
    printf("|  6. List objects               |\n");
    printf("|  7. Delete object              |\n");
    printf("|  8. Modify object              |\n");
    printf("|  9. Clear all                  |\n");
    printf("|  0. Exit                       |\n");
    printf("+--------------------------------+\n");
    printf("Choice: ");
}

/* 
   MAIN
    */
int main() {
    init_canvas();
    printf("  2D Graphics Editor started. Canvas is empty.\n");

    int choice;
    do {
        print_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1: display_all();    break;
            case 2: add_circle();     break;
            case 3: add_rectangle();  break;
            case 4: add_line();       break;
            case 5: add_triangle();   break;
            case 6: list_objects();   break;
            case 7: delete_object();  break;
            case 8: modify_object();  break;
            case 9:
                obj_count = 0;
                init_canvas();
                printf("  All objects cleared.\n");
                break;
            case 0:
                printf("  Exiting. Goodbye!\n");
                break;
            default:
                printf("  Invalid choice. Try again.\n");
        }
    } while (choice != 0);

    return 0;
}
