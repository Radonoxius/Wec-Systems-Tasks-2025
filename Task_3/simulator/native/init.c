#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<CL/cl.h>

uint8_t* TEMP;

void reset_done(cl_command_queue queue, uint32_t* done) {
    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, done, sizeof(uint32_t), 0, nullptr, nullptr);
    *done = 0;
    clEnqueueSVMUnmap(queue, done, 0, nullptr, nullptr);
}

bool is_done(cl_command_queue queue, uint32_t* done) {
    bool result = false;

    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_READ, done, sizeof(uint32_t), 0, nullptr, nullptr);
    if (*done == 0) {
        result = true;
    }
    
    clEnqueueSVMUnmap(queue, done, 0, nullptr, nullptr);
    return result;
}

void get_randoms(cl_command_queue queue, uint8_t *buffer, const size_t len) {
    FILE *fp = fopen("/dev/random", "rb");
    fread(TEMP, sizeof(uint8_t), len, fp);
    fclose(fp);

    for (size_t i = 0; i < len; i++)
        TEMP[i] = (TEMP[i] < 64) ? 1 : 0;

    clEnqueueSVMMemcpy(queue, CL_FALSE, buffer, TEMP, len, 0, nullptr, nullptr);
}

void initialize(
    cl_command_queue queue,
    uint8_t *buffer,
    const size_t len,
    const uint32_t tree_grid_size,
    const uint8_t factor
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

    //printf("At start:\n");
    //for (size_t j = 0; j < tree_grid_size + 2; j++) {
    //    for (size_t i = 0; i < tree_grid_size + 2; i++) {
    //        printf("%d", TEMP[j * (tree_grid_size + 2) + i]);
    //    }
    //    printf("\n");
    //}
    //printf("\n");

    clEnqueueSVMMemcpy(queue, CL_TRUE, buffer, TEMP, len, 0, nullptr, nullptr);
}

uint64_t simulate(
    const char *shader_src,
    const uint64_t shader_len,
    const uint32_t tree_grid_size,
    const uint8_t starting_factor
) {
    uint64_t epoch_count = 0;

    cl_platform_id platform;

    clGetPlatformIDs(
        1,
        &platform,
        nullptr
    );
    
    cl_device_id gpu;

    clGetDeviceIDs(
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &gpu,
        nullptr
    );

    cl_context context = clCreateContext(
        nullptr,
        1,
        &gpu,
        nullptr,
        nullptr,
        nullptr
    );

    cl_program shader =
        clCreateProgramWithSource(context, 1, &shader_src, &shader_len, nullptr);

    clBuildProgram(
        shader,
        1,
        &gpu,
        "-cl-mad-enable -cl-std=CL2.0 -Wall -Werror",
        nullptr,
        nullptr
    );

    cl_kernel shader_kernel = clCreateKernel(shader, "simulate", nullptr);

    cl_command_queue queue = clCreateCommandQueueWithProperties(
        context,
        gpu,
        nullptr,
        nullptr
    );

    uint64_t global_work_size[2] = {tree_grid_size, tree_grid_size};

    uint32_t* dimension = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_READ_ONLY,
        sizeof(uint32_t),
        0
    );
    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, dimension, sizeof(uint32_t), 0, nullptr, nullptr);
    *dimension = tree_grid_size;
    clEnqueueSVMUnmap(queue, dimension, 0, nullptr, nullptr);

    uint64_t extended_grid_size =
        tree_grid_size * tree_grid_size + 4 * tree_grid_size + 4;

    TEMP = malloc(extended_grid_size);

    uint8_t* tree_grid = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_READ_ONLY,
        sizeof(uint8_t) * extended_grid_size,
        0
    );
    initialize(queue, tree_grid, extended_grid_size, tree_grid_size, starting_factor);

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
    clEnqueueSVMMemcpy(queue, CL_TRUE, next_tree_grid, TEMP, extended_grid_size, 0, nullptr, nullptr);

    uint32_t* done = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_READ_WRITE,
        sizeof(uint32_t),
        0
    );

    clSetKernelArgSVMPointer(shader_kernel, 0, dimension);
    clSetKernelArgSVMPointer(shader_kernel, 1, tree_grid);
    clSetKernelArgSVMPointer(shader_kernel, 2, randoms);
    clSetKernelArgSVMPointer(shader_kernel, 3, next_tree_grid);
    clSetKernelArgSVMPointer(shader_kernel, 4, done);

    do {
        reset_done(queue, done);
        get_randoms(queue, randoms, tree_grid_size * tree_grid_size);

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
        
        clEnqueueSVMMemcpy(
            queue,
            CL_FALSE,
            tree_grid,
            next_tree_grid,
            extended_grid_size,
            0,
            nullptr,
            nullptr
        );

        epoch_count += 1;
    } while (!is_done(queue, done));

    //printf("At the End:\n");
    //clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_READ, next_tree_grid, extended_grid_size, 0, nullptr, nullptr);
    //for (size_t j = 0; j < tree_grid_size + 2; j++) {
    //    for (size_t i = 0; i < tree_grid_size + 2; i++) {
    //        printf("%d", next_tree_grid[j * (tree_grid_size + 2) + i]);
    //    }
    //    printf("\n");
    //}
    //clEnqueueSVMUnmap(queue, next_tree_grid, 0, nullptr, nullptr);
    //printf("\n");

    clFinish(queue);

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
    clReleaseDevice(gpu);

    return epoch_count;
}