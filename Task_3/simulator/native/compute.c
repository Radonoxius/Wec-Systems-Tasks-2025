#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <CL/cl.h>

//Device could be GPU, CPU, NPU, anything!
//Holds temporary values to be sent to Device...
uint8_t* TEMP;

//A T-Flip-Flop initialized to 0?
uint8_t TFF = 0;

//Resets done to 0
void reset_done(cl_command_queue queue, uint32_t* done) {
    clEnqueueSVMMap(
        queue,
        CL_TRUE,
        CL_MAP_WRITE,
        done,
        sizeof(uint32_t),
        0,
        nullptr,
        nullptr
    );

    *done = 0;

    clEnqueueSVMUnmap(queue, done, 0, nullptr, nullptr);
}

//Gives `true` if done = 0, basically the simulation ends
bool is_done(cl_command_queue queue, uint32_t* done) {
    bool result = false;

    clEnqueueSVMMap(
        queue,
        CL_TRUE,
        CL_MAP_READ,
        done,
        sizeof(uint32_t),
        0,
        nullptr,
        nullptr
    );

    if (*done == 0) {
        result = true;
    }
    
    clEnqueueSVMUnmap(queue, done, 0, nullptr, nullptr);
    return result;
}

//Gets the next `logical` tree grid
uint8_t* get_next_tree_grid(uint8_t* a, uint8_t* b) {
    if (TFF)
        return a;
    else
        return b;
}

//Gets the current `logical` tree grid
uint8_t* get_tree_grid(uint8_t* a, uint8_t* b) {
    if (TFF)
        return b;
    else
        return a;
}

//Gets random bytes on linux such that, p(1) = 0.25 & p(0) = 0.75
void get_randoms(
    cl_command_queue queue,
    uint8_t *buffer,
    const size_t len
) {
    FILE *fp = fopen("/dev/random", "rb");
    fread(TEMP, sizeof(uint8_t), len, fp);
    fclose(fp);

    for (size_t i = 0; i < len; i++)
        TEMP[i] = (TEMP[i] < 64) ? 1 : 0;

    clEnqueueSVMMemcpy(
        queue,
        CL_FALSE,
        buffer,
        TEMP,
        len,
        0,
        nullptr,
        nullptr
    );
}

//Initialises the tree grid to be simulated
void initialize(
    cl_command_queue queue,
    uint8_t *buffer,
    const size_t len,
    const uint32_t tree_grid_size,
    const uint8_t factor,
    const bool print_enabled
) {
    FILE *fp = fopen("/dev/random", "rb");
    fread(TEMP, sizeof(uint8_t), len, fp);
    fclose(fp);

    for (size_t i = 0; i < len; i++)
        TEMP[i] = (TEMP[i] <= factor) ? 1 : 0;

    for (size_t i = 0; i < tree_grid_size + 2; i++) {
        TEMP[i] = 3;
        TEMP[(tree_grid_size + 2) * (tree_grid_size + 1) + i] = 3;
    }

    for (size_t i = 1; i <= tree_grid_size; i++) {
        TEMP[i * (tree_grid_size + 2)] = 3;
        TEMP[i * (tree_grid_size + 2) + tree_grid_size + 1] = 3;
    }

    if (print_enabled) {
        printf("At start:\n");
        for (size_t j = 0; j < tree_grid_size; j++) {
            for (size_t i = 0; i < tree_grid_size; i++) {
                printf("%d", TEMP[(j + 1) * (tree_grid_size + 2) + i + 1]);
            }
            printf("\n");
        }
        printf("\n");
    }

    clEnqueueSVMMemcpy(
        queue,
        CL_TRUE,
        buffer,
        TEMP,
        len,
        0,
        nullptr,
        nullptr
    );
}

//Simulate it!
uint64_t simulate(
    const char *shader_src,
    const uint64_t shader_len,
    const uint32_t tree_grid_size,
    const uint8_t starting_factor,
    const bool print_enabled
) {
    uint64_t epoch_count = 0;

    //Device initialisation stuff...
    cl_platform_id platform;

    clGetPlatformIDs(
        1,
        &platform,
        nullptr
    );
    
    //Device initialisation stuff...
    cl_device_id device;

    clGetDeviceIDs(
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &device,
        nullptr
    );

    //Device initialisation stuff...
    cl_context context = clCreateContext(
        nullptr,
        1,
        &device,
        nullptr,
        nullptr,
        nullptr
    );

    //Create the shader program
    cl_program shader = clCreateProgramWithSource(
        context,
        1,
        &shader_src,
        &shader_len,
        nullptr
    );

    //Compile the shader program
    clBuildProgram(
        shader,
        1,
        &device,
        "-cl-mad-enable -cl-std=CL2.0 -Wall -Werror",
        nullptr,
        nullptr
    );

    cl_kernel shader_kernel = clCreateKernel(shader, "simulate", nullptr);

    cl_command_queue queue = clCreateCommandQueueWithProperties(
        context,
        device,
        nullptr,
        nullptr
    );

    uint64_t global_work_size[2] = { tree_grid_size, tree_grid_size };

    //Allocate Device memory and initialise it
    uint32_t* dimension = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_READ_ONLY,
        sizeof(uint32_t),
        0
    );
    clEnqueueSVMMap(
        queue,
        CL_TRUE,
        CL_MAP_WRITE,
        dimension,
        sizeof(uint32_t),
        0,
        nullptr,
        nullptr
    );
    *dimension = tree_grid_size;
    clEnqueueSVMUnmap(
        queue,
        dimension,
        0,
        nullptr,
        nullptr
    );

    uint64_t extended_grid_size =
        tree_grid_size * tree_grid_size + 4 * tree_grid_size + 4;

    TEMP = malloc(extended_grid_size);

    uint8_t* tree_grid = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_READ_WRITE,
        sizeof(uint8_t) * extended_grid_size,
        0
    );
    initialize(
        queue,
        tree_grid,
        extended_grid_size,
        tree_grid_size,
        starting_factor,
        print_enabled
    );

    uint8_t* randoms = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_READ_ONLY,
        sizeof(uint8_t) * tree_grid_size * tree_grid_size,
        0
    );
    uint8_t* next_tree_grid = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_READ_WRITE,
        sizeof(uint8_t) * extended_grid_size,
        0
    );
    for (size_t i = 0; i < extended_grid_size; i++)
        TEMP[i] = 3;
    clEnqueueSVMMemcpy(
        queue,
        CL_FALSE,
        next_tree_grid,
        TEMP,
        extended_grid_size,
        0,
        nullptr,
        nullptr
    );

    uint32_t* done = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_READ_WRITE,
        sizeof(uint32_t),
        0
    );

    clSetKernelArgSVMPointer(shader_kernel, 0, dimension);
    clSetKernelArgSVMPointer(shader_kernel, 2, randoms);
    clSetKernelArgSVMPointer(shader_kernel, 4, done);
    //Initialisation finished

    //Start the timer...
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    uint64_t t1 = (
        (uint64_t) time.tv_sec * (uint64_t) 1'000'000'000 +
        (uint64_t) time.tv_nsec
    );

    //Cuz the fun begins!
    do {
        //TFF magic
        clSetKernelArgSVMPointer(
            shader_kernel,
            1,
            get_tree_grid(tree_grid, next_tree_grid)
        );
        clSetKernelArgSVMPointer(
            shader_kernel,
            3,
            get_next_tree_grid(tree_grid, next_tree_grid)
        );

        reset_done(queue, done);
        get_randoms(queue, randoms, tree_grid_size * tree_grid_size);

        //Start computing on the Device!!!!
        clEnqueueNDRangeKernel(
            queue,
            shader_kernel,
            2,
            nullptr,
            global_work_size,
            nullptr,
            0,
            nullptr,
            nullptr
        );

        if (print_enabled) {
            clEnqueueSVMMap(
                queue,
                CL_TRUE,
                CL_MAP_READ,
                get_next_tree_grid(tree_grid, next_tree_grid),
                extended_grid_size,
                0,
                nullptr,
                nullptr
            );
            for (size_t j = 0; j < tree_grid_size; j++) {
                for (size_t i = 0; i < tree_grid_size; i++) {
                    printf(
                        "%d",
                        get_next_tree_grid(
                            tree_grid,
                            next_tree_grid
                        )[(j + 1) * (tree_grid_size + 2) + i + 1]);
                }
                printf("\n");
            }
            clEnqueueSVMUnmap(
                queue,
                get_next_tree_grid(tree_grid, next_tree_grid),
                0,
                nullptr,
                nullptr
            );
            printf("\n");
        }
        clFlush(queue);
        clFinish(queue);

        //Toggle,
        TFF = TFF ^ 1;
        //and increment!
        epoch_count += 1;
    } while (!is_done(queue, done));
    clFinish(queue);

    //Stop the timer! We're done!
    clock_gettime(CLOCK_MONOTONIC, &time);
    uint64_t t2 = (
        (uint64_t) time.tv_sec * (uint64_t) 1'000'000'000 +
        (uint64_t) time.tv_nsec
    );
    printf("Simulation finished in: %lums.\n", (t2 - t1) / 1'000'000);

    //Usual deallocation
    clSVMFree(context, done);
    clSVMFree(context, next_tree_grid);
    clSVMFree(context, randoms);
    clSVMFree(context, tree_grid);
    clSVMFree(context, dimension);

    free(TEMP);

    clReleaseCommandQueue(queue);
    clReleaseKernel(shader_kernel);
    clReleaseProgram(shader);
    clReleaseContext(context);
    clReleaseDevice(device);

    //Result!!!
    return epoch_count;
}