#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int page_number;
    int frame_number;
    int next_use;
    int last_access;
} PageTableEntry;

int fifo_policy(PageTableEntry *page_table, int *frames, int num_frames, int page_request, int request_time);
int optimal_policy(PageTableEntry *page_table, int *frames, int num_frames, int *page_requests, int num_requests, int current_request, int request_time);
int lru_policy(PageTableEntry *page_table, int *frames, int num_frames, int page_request, int request_time);

int main() {
    FILE *input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return 1;
    }

    int num_pages, num_frames, num_requests;
    fscanf(input_file, "%d %d %d", &num_pages, &num_frames, &num_requests);

    PageTableEntry *page_table = calloc(num_pages, sizeof(PageTableEntry));
    int *frames = calloc(num_frames, sizeof(int));
    int *page_requests = calloc(num_requests, sizeof(int));

    for (int i = 0; i < num_requests; i++) {
        fscanf(input_file, "%d", &page_requests[i]);
    }
    fclose(input_file);

    // Create output file
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        return 1;
    }

    // int page_faults = 0;
    int fifo_page_faults = 0;
    int optimal_page_faults = 0;
    int lru_page_faults = 0;

    // Process each page request                    
    for (int i = 0; i < num_requests; i++) {
        int page_request = page_requests[i];

        fifo_page_faults += fifo_policy(page_table, frames, num_frames, page_request, i);
        optimal_page_faults += optimal_policy(page_table, frames, num_frames, page_requests, num_requests, i, i);
        lru_page_faults += lru_policy(page_table, frames, num_frames, page_request, i);

                // Write the current state of the frames to the output file for each policy
        fprintf(output_file, "Request %d: ", i + 1);
        fprintf(output_file, "FIFO: ");
        for (int j = 0; j < num_frames; j++) {
            fprintf(output_file, "%d ", frames[j]);
        }
        fprintf(output_file, "Optimal: ");
        for (int j = 0; j < num_frames; j++) {
            fprintf(output_file, "%d ", frames[j]);
        }
        fprintf(output_file, "LRU: ");
        for (int j = 0; j < num_frames; j++) {
            fprintf(output_file, "%d ", frames[j]);
        }
        fprintf(output_file, "\n");
    }
    fprintf(output_file, "FIFO Page Faults: %d\n", fifo_page_faults);
    fprintf(output_file, "Optimal Page Faults: %d\n", optimal_page_faults);
    fprintf(output_file, "LRU Page Faults: %d\n", lru_page_faults);

        // Clean up and close the output file
        fclose(output_file);
        free(page_table);
        free(frames);
        free(page_requests);

        return 0;
    }




int fifo_policy(PageTableEntry *page_table, int *frames, int num_frames, int page_request, int request_time) {
    static int oldest_frame_index = 0;

    // Check if the requested page is already in a frame
    for (int i = 0; i < num_frames; i++) {
        if (page_table[page_request].frame_number == frames[i]) {
            // No page fault, the page is already in the frame
            return 0;
        }
    }

    // Page fault occurs
    int frame_to_replace = frames[oldest_frame_index];

    // Update the page table for the replaced page
    for (int i = 0; i < num_frames; i++) {
        if (page_table[i].frame_number == frame_to_replace) {
            page_table[i].frame_number = -1;
            break;
        }
    }

    // Update the page table for the requested page
    page_table[page_request].frame_number = frame_to_replace;

    // Update the frame
    frames[oldest_frame_index] = page_table[page_request].frame_number;

    // Update the oldest frame index for the next replacement
    oldest_frame_index = (oldest_frame_index + 1) % num_frames;

    // Page fault occurred
    return 1;
}



int optimal_policy(PageTableEntry *page_table, int *frames, int num_frames, int *page_requests, int num_requests, int current_request, int request_time) {
    
    int page_request = page_requests[current_request];

    // Check if the requested page is already in a frame
    for (int i = 0; i < num_frames; i++) {
        if (page_table[page_request].frame_number == frames[i]) {
        // No page fault, the page is already in the frame
        return 0;
        }
    }

    // Calculate the next-use times for all pages in the frames
    for (int i = 0; i < num_frames; i++) {
        int page_number = frames[i];
        int next_request = INT_MAX;

        for (int j = current_request + 1; j < num_requests; j++) {
            if (page_requests[j] == page_number) {
                next_request = j;
                break;
            }
        }

        page_table[page_number].next_use = next_request;
    }

    // Find the page with the greatest next-use number
    int max_next_use = -1;
    int frame_to_replace = -1;

    for (int i = 0; i < num_frames; i++) {
        int page_number = frames[i];

        if (page_table[page_number].next_use > max_next_use) {
            max_next_use = page_table[page_number].next_use;
            frame_to_replace = i;
        }
    }

    // Update the page table for the replaced page
    int replaced_page = frames[frame_to_replace];
    page_table[replaced_page].frame_number = -1;

    // Update the page table for the requested page
    page_table[page_request].frame_number = frame_to_replace;

    // Update the frame
    frames[frame_to_replace] = page_request;

    // Page fault occurred
    return 1;
}



int lru_policy(PageTableEntry *page_table, int *frames, int num_frames, int page_request, int request_time) {
    // Check if the requested page is already in a frame
    for (int i = 0; i < num_frames; i++) {
        if (page_table[page_request].frame_number == frames[i]) {
            // No page fault, the page is already in the frame
            // Update the last access time
            page_table[page_request].last_access = request_time;
            return 0;
        }
    }

    // Page fault occurs, find the page with the smallest time stamp
    int min_last_access = INT_MAX;
    int frame_to_replace = -1;

    for (int i = 0; i < num_frames; i++) {
        int page_number = frames[i];

        if (page_table[page_number].last_access < min_last_access) {
            min_last_access = page_table[page_number].last_access;
            frame_to_replace = i;
        }
    }

    // Update the page table for the replaced page
    int replaced_page = frames[frame_to_replace];
    page_table[replaced_page].frame_number = -1;

    // Update the page table for the requested page
    page_table[page_request].frame_number = frame_to_replace;
    page_table[page_request].last_access = request_time;

    // Update the frame
    frames[frame_to_replace] = page_request;

    // Page fault occurred
    return 1;
}

