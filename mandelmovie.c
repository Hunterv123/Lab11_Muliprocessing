/* CPE-2600 Lab 11: Generate Mandelbrot movie frames with child processes. 
* Author: Hunter Vassar
* Section: 121
* Date: 11/23/2026
* Compile: make mandelmovie
*/

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

enum { FRAME_COUNT = 50 };

int main(int argc, char *argv[])
{
    int process_limit = 0;
    int option;

    while ((option = getopt(argc, argv, "p:h")) != -1) {
        switch (option) {
        case 'p': {
            char *end;
            long count = strtol(optarg, &end, 10);
            if (end == optarg || *end != '\0' || count < 1 || count > FRAME_COUNT) {
                fprintf(stderr, "-p must be an integer from 1 to %d\n", FRAME_COUNT);
                return EXIT_FAILURE;
            }
            process_limit = (int)count;
            break;
        }
        case 'h':
            printf("Usage: %s -p <processes> (1-%d)\n", argv[0], FRAME_COUNT);
            return EXIT_SUCCESS;
        default:
            return EXIT_FAILURE;
        }
    }

    if (process_limit == 0 || optind != argc) {
        fprintf(stderr, "Usage: %s -p <processes> (1-%d)\n", argv[0], FRAME_COUNT);
        return EXIT_FAILURE;
    }

    const double initial_scale = 4.0;
    const double final_scale = 0.0005;
    const double zoom_step = pow(final_scale / initial_scale, 1.0 / (FRAME_COUNT - 1));
    double scale = initial_scale;
    int next_frame = 0;
    int active_children = 0;
    int result = EXIT_SUCCESS;

    while (next_frame < FRAME_COUNT || active_children > 0) {
        while (result == EXIT_SUCCESS && next_frame < FRAME_COUNT &&
               active_children < process_limit) {
            char scale_arg[32];
            char filename[32];
            snprintf(scale_arg, sizeof(scale_arg), "%.12g", scale);
            snprintf(filename, sizeof(filename), "mandel%d.jpg", next_frame);

            pid_t child = fork();
            if (child == -1) {
                perror("fork");
                result = EXIT_FAILURE;
                break;
            }
            if (child == 0) {
                execl("./mandel", "mandel", "-x", "0.286932", "-y", "0.014287",
                      "-s", scale_arg, "-o", filename, (char *)NULL);
                perror("./mandel");
                _exit(EXIT_FAILURE);
            }

            active_children++;
            next_frame++;
            scale *= zoom_step;
        }

        if (active_children == 0)
            break;

        int status;
        pid_t finished;
        do {
            finished = wait(&status);
        } while (finished == -1 && errno == EINTR);

        if (finished == -1) {
            perror("wait");
            return EXIT_FAILURE;
        }
        active_children--;

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "mandel process %ld failed\n", (long)finished);
            result = EXIT_FAILURE;
        }
    }

    return result;
}
