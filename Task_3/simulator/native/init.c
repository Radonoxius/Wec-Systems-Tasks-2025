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

void get_randoms(cl_command_queue queue, uint8_t *buffer, size_t len) {
    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, buffer, len, 0, nullptr, nullptr);

    FILE *fp = fopen("/dev/random", "rb");
    fread(TEMP, sizeof(uint8_t), len, fp);
    fclose(fp);

    for (size_t i = 0; i < len; i++)
        TEMP[i] = (TEMP[i] < 64) ? 1 : 0;

    for (size_t i = 0; i < len; i++)
        buffer[i] = TEMP[i];

    clEnqueueSVMUnmap(queue, buffer, 0, nullptr, nullptr);
}

//factor = 255 means all trees start off in alive state
//factor = 0 means all trees start off in burning state
void initialize(cl_command_queue queue, uint8_t *buffer, size_t len, uint8_t factor) {
    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, buffer, len, 0, nullptr, nullptr);

    FILE *fp = fopen("/dev/random", "rb");
    fread(TEMP, sizeof(uint8_t), len, fp);
    fclose(fp);

    for (size_t i = 0; i < len; i++)
        TEMP[i] = (TEMP[i] <= factor) ? 1 : 0;

    for (size_t i = 0; i < len; i++)
        buffer[i] = TEMP[i];

    clEnqueueSVMUnmap(queue, buffer, 0, nullptr, nullptr);
}

//factor = 255 means all trees start off in alive state
//factor = 0 means all trees start off in burning state
uint64_t simulate(
    const char *shader_src,
    const uint64_t shader_len,
    const uint32_t tree_grid_size,
    const uint8_t starting_factor
) {
    uint64_t epoch_count = 0;

    cl_platform_id platform;
    cl_int errcode;

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
    uint64_t local_work_size[2] = {tree_grid_size, 1};
    uint64_t offset[2] = {0, 0};

    uint32_t* dimension = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_WRITE_ONLY,
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
        CL_MEM_WRITE_ONLY,
        sizeof(uint8_t) * extended_grid_size,
        0
    );
    uint8_t* randoms = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_WRITE_ONLY,
        sizeof(uint8_t) * tree_grid_size * tree_grid_size,
        0
    );
    uint8_t* next_tree_grid = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_WRITE_ONLY,
        sizeof(uint8_t) * extended_grid_size,
        0
    );
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

        errcode = clEnqueueNDRangeKernel(
            queue,
            shader_kernel,
            2,
            offset,
            global_work_size,
            local_work_size,
            0,
            nullptr,
            nullptr
        );
        if (errcode != CL_SUCCESS) {
            printf("An error occured while kernel execution.\n");
            clReleaseCommandQueue(queue);
            clReleaseKernel(shader_kernel);
            clReleaseProgram(shader);
            clReleaseContext(context);
            clReleaseDevice(gpu);
            exit(1);
        }
        
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
        clFinish(queue);

        epoch_count += 1;
    } while (!is_done(queue, done));

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