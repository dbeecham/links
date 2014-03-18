#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

struct color { int r; int g; int b; };
struct color colors[10] = {
    {237, 0, 47},
    {255, 108, 0},
    {166, 0, 166},
    {72, 221, 0},
    {255, 199, 0},
    {40, 24, 209},
    {0, 157, 145},
    {207, 247, 0},
    {203, 0, 119},
    {80, 80, 80}
}; 

/*struct color colors[10] = {
    {10, 10, 10},
    {20, 20, 20},
    {30, 30, 30},
    {40, 40, 40},
    {50, 50, 50},
    {60, 60, 60},
    {70, 70, 70},
    {80, 80, 80},
    {90, 90, 90},
    {100, 100, 100}
};*/

struct config {
    double width;
    double height;
    double ball_radius;
    double line_width;
    double spacing;
    double margin;
    char *input;
    char *output;
    double num_ball_columns;
    double num_ball_rows;
    int num_balls;
} config = {1440, 900, 8, 6, 8, 10, "out.png", "in.csv", 0, 0, 0};

// Given an integer (in range [0, inf)), calculate which position
// in x the ball should have.
double pos_x(int i) {
    double ball_line_width = (2 * config.ball_radius * config.num_ball_columns) + (config.spacing * (config.num_ball_columns - 1));
    double first_ball_position = ((config.width - ball_line_width) / 2) + config.ball_radius;
    return (i % (int)config.num_ball_columns) * (config.ball_radius * 2 + config.spacing) + first_ball_position;
}

// Given an integer (in range [0, inf)), calculate which position
// in y the ball should have.
int pos_y(int i) {
    double ball_line_width = (2 * config.ball_radius * config.num_ball_rows) + (config.spacing * (config.num_ball_rows - 1));
    double first_ball_position = ((config.height - ball_line_width) / 2) + config.ball_radius;
    return floor((double)i / config.num_ball_columns) * (config.ball_radius * 2 + config.spacing) + first_ball_position;
}

void draw(cairo_t *cr) {
    FILE * input = fopen("digits", "r");
    if (NULL == input) {
        fprintf(stderr, "Could not open file!");
        exit(1);
    }

    int i = 0;
    int arr[config.num_balls];

    int in = fgetc(input);
    int x = 0;

    // Black background
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    while (EOF != in && i < config.num_balls) {
        if (in >= (int)'0' && in <= (int)'9') {

            x = in - (int)'0';

            // Add number to the array
            arr[i] = x;

            // draw circle
            cairo_set_source_rgb(cr, 
                    colors[x].r / 255.0, 
                    colors[x].g / 255.0, 
                    colors[x].b / 255.0);
            cairo_arc(cr, pos_x(i), pos_y(i), config.ball_radius, 0, 2 * M_PI);
            cairo_fill(cr);

            // check for links

            // Left?
            if (i - 1 > 0 && i % (int)config.num_ball_columns != 0 && x == arr[i-1]) {
                cairo_move_to(cr, pos_x(i), pos_y(i));
                cairo_line_to(cr, pos_x(i-1), pos_y(i-1));
                cairo_set_line_width(cr, config.line_width);
                cairo_stroke(cr);
            }


            // Above?
            if (i - config.num_ball_columns > 0 && x == arr[i - (int)config.num_ball_columns]) {
                cairo_move_to(cr, pos_x(i), pos_y(i));
                cairo_line_to(cr, pos_x(i-config.num_ball_columns), pos_y(i-config.num_ball_columns));
                cairo_set_line_width(cr, config.line_width);
                cairo_stroke(cr);
            }

            // Above-right?
            if (i - (config.num_ball_columns - 1) > 0 
                    && i % (int)config.num_ball_columns != (config.num_ball_columns - 1) 
                    && x == arr[i - (int)(config.num_ball_columns - 1)]) {
                cairo_move_to(cr, pos_x(i), pos_y(i));
                cairo_line_to(cr, pos_x(i-(config.num_ball_columns - 1)), pos_y(i-(config.num_ball_columns - 1)));
                cairo_set_line_width(cr, config.line_width);
                cairo_stroke(cr);
            }

            if (i - (config.num_ball_columns + 1) > 0 
                    && i % (int)config.num_ball_columns != 0 
                    && x == arr[i - (int)(config.num_ball_columns + 1)]) {
                cairo_move_to(cr, pos_x(i), pos_y(i));
                cairo_line_to(cr, pos_x(i-(config.num_ball_columns + 1)), pos_y(i-(config.num_ball_columns + 1)));
                cairo_set_line_width(cr, config.line_width);
                cairo_stroke(cr);
            }

            // Above-left?
            /*
            if (i - 11 > 0 && i % 10 != 0 && pi[i] == pi[i - 11]) {
                cairo_move_to(cr, pos_x(i), pos_y(i));
                cairo_line_to(cr, pos_x(i-11), pos_y(i-11));
                cairo_set_line_width(cr, line_width);
                cairo_stroke(cr);
            } */

            // Inclease current index
            i += 1;
        }

        in = getc(input);
    }
    fclose(input);
}

int main(int argc, char *argv[]) {


    static struct option long_options[] = {
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"margin", required_argument, 0, 'm'},
        {"radius", required_argument, 0, 'r'},
        {"line-width", required_argument, 0, 'l'},
        {"spacing", required_argument, 0, 's'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
    int c = 0;
    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "w:h:m:r:l:s:", long_options, &option_index);

        if (-1 == c) break;

        switch (c) {
            case 'w':
                config.width = atol(optarg);
                break;
            case 'h':
                config.height = atol(optarg);
                break;
            case 'm':
                config.margin = atol(optarg);
                break;
            case 'r':
                config.ball_radius = atol(optarg);
                break;
            case 'l':
                config.line_width = atol(optarg);
                break;
            case 's':
                config.spacing = atol(optarg);
                break;
            case 'o':
                config.output = optarg;
                break;
            case 'i':
                config.input = optarg;
                break;
            default:
                fprintf(stderr, "Something is up with your arguments!");
                exit(1);
        }
    }



    config.num_ball_columns = floor((config.width - 2*config.margin) / ((2 * config.ball_radius) + config.spacing));
    config.num_ball_rows = floor((config.height - 2*config.margin) / ((2 * config.ball_radius) + config.spacing));
    config.num_balls = config.num_ball_columns * config.num_ball_rows;
    printf("CONFIG:\n=======\nwidth: %f\nheight: %f\nmargin: %f\nball radius: %f\nline width: %f\nspacing: %f\n", 
            config.width, 
            config.height, 
            config.margin,
            config.ball_radius,
            config.line_width,
            config.spacing);

    printf("CALCULATED:\n===========\nballs: %i\n", config.num_balls);

    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, config.width, config.height);
    cairo_t *cr0 = cairo_create(surface);
    draw(cr0);
    cairo_surface_write_to_png(surface, "out.png");

    return 0;
}
