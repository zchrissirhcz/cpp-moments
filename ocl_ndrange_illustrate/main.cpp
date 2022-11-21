#include "../common/opencl_helper.hpp"
#include "../common/opencv_helper.hpp"

const char* kernel_str = CL_KERNEL(
    __kernel void test(__global const int* data)
    {
        int work_dim = get_work_dim();

        char symbols[3] = {'x', 'y', 'z'};

        int idx;
        {
            int j = get_global_id(0);
            int i = get_global_id(1);
            int width = get_global_size(0);
            int height = get_global_size(1);
            idx = i * width + j;
        }
        if (data[idx] == 255)
        {
            printf("global id:");
            for (int k = 0; k < 3; k++)
            {
                int global_id = get_global_id(k);
                char ch = symbols[k];
                printf(" g%c=%d", ch, global_id);
            }
            printf("\n");

            printf("global size:");
            for (int k = 0; k < work_dim; k++)
            {
                char ch = symbols[k];
                int global_size = get_global_size(k);
                printf(" G%c=%d", ch, global_size);
            }
            printf("\n");

            printf("local id:");
            for (int k = 0; k < work_dim; k++)
            {
                int local_id = get_local_id(k);
                char ch = symbols[k];
                printf(" s%c=%d", ch, local_id);
            }
            printf("\n");
            
            printf("local size(group size)");
            for (int k = 0; k < work_dim; k++)
            {
                char ch = symbols[k];
                int local_size = get_local_size(k);
                printf(" S%c=%d", ch, local_size);
            }
            printf("\n");

            printf("group id:");
            for (int k = 0; k < work_dim; k++)
            {
                char ch = symbols[k];
                int group_id = get_group_id(k);
                printf(" w%c=%d", ch, group_id);
            }
            printf("\n");

            printf("num groups:");
            for (int k = 0; k < work_dim; k++)
            {
                char ch = symbols[k];
                int num_groups = get_num_groups(k);
                printf(" W%c=%d", ch, num_groups);
            }
            printf("\n");
        }
    }
);

// 用于验证 NDRange 里的 local ID, group size, group ID, global ID 等相关概念，用计算结果进行验证
// 对照 ndrange_index_illustration.jpg 比对
int main()
{
    // step1~step6: create kernel
    cl_int ret;
    cl_platform_id platform = get_opencl_platform();
    cl_device_id device = get_opencl_device(platform);
    cl_context context = get_opencl_context(platform, device);
    cl_command_queue command_queue = get_opencl_command_queue(context, device, true);
    cl_program program = get_opencl_program_from_string(context, device, kernel_str);
    cl_kernel kernel = clCreateKernel(program, "test", &ret);
    CL_CHECK(ret);

    // step7: create memory objects
    const int width = 16;
    const int height = 12;
    cv::Size size;
    size.width = width;
    size.height = height;
    cv::Mat mat(size, CV_32SC1);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            mat.ptr<int>(i, j)[0] = 0;
        }
    }
    mat.ptr<int>(7, 10)[0] = 255;

    cl_mem_flags flags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
    const int bufsize = sizeof(cl_int) * height * width;
    cl_mem cl_a = clCreateBuffer(context, flags, bufsize, mat.data, NULL);


    // step8: set kernel arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), &cl_a);
    CL_CHECK(ret);

    // step9~step10: set work group size, run kernel
    size_t global_work_size[] = { (size_t)width, (size_t)height };
    size_t local_work_size[] = { 4, 4 };
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    CL_CHECK(ret);

    // step11: get result
    // skip

    // step12: release all resources
    release_opencl_objects(kernel, program, command_queue, context);

    return 0;
}