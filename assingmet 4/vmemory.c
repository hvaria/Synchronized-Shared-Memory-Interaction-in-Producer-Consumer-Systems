/*
* CSC 139
* Spring 2023
* Fourth Assignment
* Varia, Himanshukumar
* Section #1
* OSs Tested on: Linux (ecs-pa-coding1.ecs.csus.edu)
* Compiled using: gcc -std=c99 -o vmemory vmemory.c
* Run command example: ./vmemory
* Result: when ther is an existing "input.txt" with the requiredformat the program
* writes into a file "output.txt" the scheduling results
* of the test algorithm specified in the input file data
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

void fifo_policy(FILE *output_file,int *page_requests, int num_pages, int num_frames, int num_requests);
void optimal_policy(FILE *output_file, int *page_requests, int num_pages, int num_frames, int num_requests);
void lru_policy(FILE *output_file, int *page_requests, int num_pages, int num_frames, int num_requests);

int main() {
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file) {
        perror("Error opening file");
        return 1;
    }

    int num_pages, num_frames, num_requests;
    fscanf(input_file, "%d %d %d", &num_pages, &num_frames, &num_requests);

    int *page_requests = (int *) malloc(num_requests * sizeof(int));
    for (int i = 0; i < num_requests; i++) {
        fscanf(input_file, "%d", &page_requests[i]);
    }
    fclose(input_file);

    FILE *output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Error opening output file");
        return 1;
    }

    fifo_policy( output_file, page_requests, num_pages, num_frames, num_requests);
    fprintf(output_file, "\n");
    optimal_policy(output_file, page_requests, num_pages, num_frames, num_requests);
    fprintf(output_file, "\n");
    lru_policy(output_file, page_requests, num_pages, num_frames, num_requests);


    fclose(output_file);
    free(page_requests);
    return 0;
}

// Implementation of the FIFO, Optimal, and LRU policy functions here

void fifo_policy(FILE *output_file, int *page_requests, int num_pages, int num_frames, int num_requests) {
    fprintf(output_file, "FIFO\n");


    // Allocate and initialize memory for frames and loaded
    int *frames = (int *) malloc(num_frames * sizeof(int));
    int *loaded = (int *) malloc(num_frames * sizeof(int));

    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
        loaded[i] = 0;
    }

    int page_faults = 0;
    int frame_idx = 0;

    // Iterate through the page requests
    for (int i = 0; i < num_requests; i++) {
        int page = page_requests[i];
        int pos = -1;

        // Check if the requested page is already in a frame
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                pos = j;
                break;
            }
        }

        // If the page is not in a frame (page fault)
        if (pos == -1) {

            // Unload the previous page from the selected frame, if any
            if (frames[frame_idx] != -1) {
                fprintf(output_file, "Page %d unloaded from Frame %d, ", frames[frame_idx], frame_idx);
            }

            // Load the requested page into the selected frame
            frames[frame_idx] = page;
            fprintf(output_file, "Page %d loaded into Frame %d\n", page, frame_idx);

            // Update the frame index to use the next frame in a circular manner
            frame_idx = (frame_idx + 1) % num_frames;
            page_faults++;
        } else {
            // If the page is already in a frame
            fprintf(output_file, "Page %d already in Frame %d\n", page, pos);
        }
    }

    fprintf(output_file, "\n%d page faults\n", page_faults);

    free(frames);
}



void optimal_policy(FILE *output_file, int *page_requests, int num_pages, int num_frames, int num_requests) {
    fprintf(output_file, "Optimal\n");


    // Allocate and initialize memory for frames
    int *frames = (int *) malloc(num_frames * sizeof(int));
    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
    }

    int page_faults = 0;

    // Iterate through the page requests
    for (int i = 0; i < num_requests; i++) {
        int page = page_requests[i];
        int pos = -1;

        // Check if the requested page is already in a frame
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                pos = j;
                break;
            }
        }

        // If the page is not in a frame (page fault)
        if (pos == -1) {
            int replace_idx = -1;
            int empty_frame = -1;

            // Check if there is an empty frame
            for (int j = 0; j < num_frames; j++) {
                if (frames[j] == -1) {
                    empty_frame = j;
                    break;
                }
            }

            // If an empty frame is found, use it
            if (empty_frame != -1) {
                replace_idx = empty_frame;
            } else {
                // If no empty frame is found, find the frame with the page that will be used the furthest in the future
                int furthest = -1;

                for (int j = 0; j < num_frames; j++) {
                    int last_used = num_requests;
                    for (int k = i + 1; k < num_requests; k++) {
                        if (page_requests[k] == frames[j]) {
                            last_used = k;
                            break;
                        }
                    }

                    if (last_used > furthest) {
                        furthest = last_used;
                        replace_idx = j;
                    }
                }
            }

            // Unload the previous page from the selected frame, if any
            if (frames[replace_idx] != -1) {
                fprintf(output_file, "Page %d unloaded from Frame %d, ", frames[replace_idx], replace_idx);
            }
            // Load the requested page into the selected frame
            frames[replace_idx] = page;
            fprintf(output_file, "Page %d loaded into Frame %d\n", page, replace_idx);
            page_faults++;
        } else {
            // If the page is already in a frame
            fprintf(output_file, "Page %d already in Frame %d\n", page, pos);
        }
    }

    fprintf(output_file, "\n%d page faults\n", page_faults);

    free(frames);
}



void lru_policy(FILE *output_file, int *page_requests, int num_pages, int num_frames, int num_requests) {
    fprintf(output_file, "LRU\n");

    // Allocate and initialize memory for frames and their last used timestamps
    int *frames = (int *) malloc(num_frames * sizeof(int));
    int *last_used = (int *) malloc(num_frames * sizeof(int));

    for (int i = 0; i < num_frames; i++) {
        frames[i] = -1;
        last_used[i] = -1;
    }

    int page_faults = 0;

    // Iterating through the page requests
    for (int i = 0; i < num_requests; i++) {
        int page = page_requests[i];
        int pos = -1;

        // Check if the requested page is already in a frame
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                pos = j;
                break;
            }
        }

        // If the page is not in a frame (page fault)
        if (pos == -1) {
            int replace_idx = -1;
            int empty_frame = -1;

            // Check if there is an empty frame
            for (int j = 0; j < num_frames; j++) {
                if (frames[j] == -1) {
                    empty_frame = j;
                    break;
                }
            }

            // If an empty frame is found, use it
            if (empty_frame != -1) {
                replace_idx = empty_frame;
            } else {

                // If no empty frame is found, find the least recently used frame
                int lru = INT_MAX;

                for (int j = 0; j < num_frames; j++) {
                    if (last_used[j] < lru) {
                        lru = last_used[j];
                        replace_idx = j;
                    }
                }
            }

            // Unload the previous page from the selected frame, if any
            if (frames[replace_idx] != -1) {
                fprintf(output_file, "Page %d unloaded from Frame %d, ", frames[replace_idx], replace_idx);
            }
            // Load the requested page into the selected frame
            frames[replace_idx] = page;
            fprintf(output_file, "Page %d loaded into Frame %d\n", page, replace_idx);
            page_faults++;
        } else {
            // If the page is already in a frame
            fprintf(output_file, "Page %d already in Frame %d\n", page, pos);
        }

        // Update the last_used timestamp for the current page
        for (int j = 0; j < num_frames; j++) {
            if (frames[j] == page) {
                last_used[j] = i;
            }
        }
    }

    // Print the total number of page faults
    fprintf(output_file, "\n%d page faults\n", page_faults);

    //Free allocated memory
    free(frames);
    free(last_used);
}
