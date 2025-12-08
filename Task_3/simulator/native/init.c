#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<CL/cl.h>

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

uint64_t simulate(
    const char *shader_src,
    const uint64_t shader_len,
    const uint32_t tree_grid_size
) {
    uint64_t epoch_count = 0;

    cl_platform_id platform;
    cl_int errcode;

    errcode = clGetPlatformIDs(
        1,
        &platform,
        nullptr
    );
    
    cl_device_id gpu;

    errcode = clGetDeviceIDs(
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
        &errcode
    );

    cl_program shader =
        clCreateProgramWithSource(context, 1, &shader_src, &shader_len, &errcode);
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating the shader.\n");
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    errcode = clBuildProgram(
        shader,
        1,
        &gpu,
        "-cl-mad-enable -cl-std=CL2.0 -Wall -Werror", //-Wall -Werror
        nullptr,
        nullptr
    );
    if (errcode != CL_SUCCESS) {
        printf("An error occured while compiling/linking the shader.\n\n");

        char* log;
        size_t log_len;
        clGetProgramBuildInfo(shader, gpu, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_len);
        log = (char*) malloc(log_len);
        clGetProgramBuildInfo(shader, gpu, CL_PROGRAM_BUILD_LOG, log_len, log, nullptr);
        printf("%s\n", log);

        free(log);
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    cl_kernel shader_kernel = clCreateKernel(shader, "simulate", &errcode);
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating the shader kernel.\n");
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    cl_command_queue queue = clCreateCommandQueueWithProperties(
        context,
        gpu,
        nullptr,
        &errcode
    );

    uint64_t global_work_size[2] = {tree_grid_size, tree_grid_size};
    uint64_t local_work_size[2] = {tree_grid_size, 1};
    uint64_t offset[2] = {0, 0};

    uint64_t total_grid_size =
        tree_grid_size * tree_grid_size + 4 * tree_grid_size + 4;

    uint32_t* dimension = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_WRITE_ONLY,
        sizeof(uint32_t),
        0
    );
    clEnqueueSVMMap(queue, CL_TRUE, CL_MAP_WRITE, dimension, sizeof(uint32_t), 0, nullptr, nullptr);
    *dimension = tree_grid_size;
    clEnqueueSVMUnmap(queue, dimension, 0, nullptr, nullptr);

    uint8_t* tree_grid = (uint8_t*) clSVMAlloc(
        context,
        CL_MEM_WRITE_ONLY,
        sizeof(uint8_t) * total_grid_size,
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
        sizeof(uint8_t) * total_grid_size,
        0
    );
    uint32_t* done = (uint32_t*) clSVMAlloc(
        context,
        CL_MEM_READ_WRITE,
        sizeof(uint32_t),
        0
    );
    reset_done(queue, done);

    clSetKernelArgSVMPointer(shader_kernel, 0, dimension);
    clSetKernelArgSVMPointer(shader_kernel, 1, tree_grid);
    clSetKernelArgSVMPointer(shader_kernel, 2, randoms);
    clSetKernelArgSVMPointer(shader_kernel, 3, next_tree_grid);
    clSetKernelArgSVMPointer(shader_kernel, 4, done);
    
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
        printf("An error occured while requesting kernel execution.");
        clReleaseCommandQueue(queue);
        clReleaseKernel(shader_kernel);
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    clSVMFree(context, done);
    clSVMFree(context, next_tree_grid);
    clSVMFree(context, randoms);
    clSVMFree(context, tree_grid);
    clSVMFree(context, dimension);

    clReleaseCommandQueue(queue);
    clReleaseKernel(shader_kernel);
    clReleaseProgram(shader);
    clReleaseContext(context);
    clReleaseDevice(gpu);

    return epoch_count;
}