#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<CL/cl.h>

uint64_t gpu_execute(
    const char *shader_src,
    const uint64_t shader_len,
    const uint64_t tree_grid_size
) {
    uint64_t epoch_count = 0;

    cl_platform_id platform;
    cl_int errcode;

    errcode = clGetPlatformIDs(
        1,
        &platform,
        NULL
    );
    if (errcode != CL_SUCCESS) {
        printf("An error occured while getting the opencl platform.");
        exit(1);
    }
    
    cl_device_id gpu;

    errcode = clGetDeviceIDs(
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &gpu,
        NULL
    );
    if (errcode != CL_SUCCESS) {
        printf("An error occured while getting platform gpu.");
        exit(1);
    }

    cl_context context = clCreateContext(
        NULL,
        1,
        &gpu,
        NULL,
        NULL,
        &errcode
    );
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating opencl context.");
        clReleaseDevice(gpu);
        exit(1);
    }

    cl_program shader;
    clCreateProgramWithSource(context, 1, &shader_src, &shader_len, &errcode);
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating the shader.");
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    errcode = clBuildProgram(
        shader,
        1,
        &gpu,
        NULL,
        NULL,
        NULL
    );
    if (errcode != CL_SUCCESS) {
        printf("An error occured while compiling/linking the shader.");
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    cl_kernel shader_kernel;
    clCreateKernel(shader, "solver", &errcode);
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating the shader kernel.");
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    cl_command_queue queue;
    clCreateCommandQueue(context, gpu, NULL, &errcode);
    if (errcode != CL_SUCCESS) {
        printf("An error occured while creating the gpu command queue.");
        clReleaseKernel(shader_kernel);
        clReleaseProgram(shader);
        clReleaseContext(context);
        clReleaseDevice(gpu);
        exit(1);
    }

    uint64_t global_work_size[2] = {tree_grid_size, tree_grid_size};
    uint64_t local_work_size[2] = {tree_grid_size, 1};
    uint64_t offset[2] = {0, 0};
    errcode = clEnqueueNDRangeKernel(
        queue,
        shader_kernel,
        2,
        offset,
        global_work_size,
        local_work_size,
        NULL,
        NULL,
        NULL
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

    clReleaseCommandQueue(queue);
    clReleaseKernel(shader_kernel);
    clReleaseProgram(shader);
    clReleaseContext(context);
    clReleaseDevice(gpu);

    return epoch_count;
}