#include <stdio.h>
#include <string.h>

#include "cpuid.h"

#define cpuid_test_and_show_feature(r, feature) \
    do if ((r) & bit_##feature) { \
        printf("%s ", #feature); \
    } while (0);

struct cpuid_version_info
{
    unsigned int stepping_id        : 4;
    unsigned int model              : 4;
    unsigned int family_id          : 4;
    unsigned int processor_type     : 2;
    unsigned int                    : 2;
    unsigned int extended_model_id  : 4;
    unsigned int extended_family_id : 8;
    unsigned int                    : 4;
};

static const char *const processor_type[] = {
    "Original OEM Processor",
    "Intel OverDrive® Processor",
    "Dual processor (not applicable to Intel486 processors)",
    "Intel reserved"
};

// 打印 CPUID 基本信息支持的最大输入值
static void show_maximum_input_value_for_basic_cpuid_info()
{
    unsigned int eax, ebx, ecx, edx;
    char buf[16];
    const unsigned int leaf = 0;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 拼接 CPUID 签名信息
    memcpy(buf + 0, &ebx, sizeof ebx);
    memcpy(buf + 4, &edx, sizeof edx);
    memcpy(buf + 8, &ecx, sizeof ecx);
    buf[12] = 0;

    // 基本 CPUID 信息支持的最大输入值
    printf("Maximum input value for basic CPUID information: 0x%X\n", eax);
}

// 打印 CPU 开发商 ID
static void show_cpu_vendor_id()
{
    unsigned int eax, ebx, ecx, edx;
    char buf[16];
    const unsigned int leaf = 0;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 拼接 CPUID 签名信息
    memcpy(buf + 0, &ebx, sizeof ebx);
    memcpy(buf + 4, &edx, sizeof edx);
    memcpy(buf + 8, &ecx, sizeof ecx);
    buf[12] = 0;

    // 打印 CPU 开发商 ID
    printf("CPU vendor ID: %s\n", buf);
}

// 打印 CPU 版本信息
static void show_cpu_version_info()
{
    unsigned int ebx, ecx, edx, family, model;
    const unsigned int leaf = 1;

    union {
        struct cpuid_version_info info;
        unsigned int              eax;
    } u;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &u.eax, &ebx, &ecx, &edx);

    // 计算 family
    family = u.info.family_id != 0x0F
           ? u.info.family_id
           : (u.info.extended_family_id + u.info.family_id);

    // 计算 model
    model = (u.info.model == 0x06 || u.info.model == 0x0F)
          ? ((u.info.extended_model_id << 4) + u.info.model)
          : u.info.model;

    printf("version_info.stepping_id: %u\n", u.info.stepping_id);
    printf("version_info.model: 0x%X\n", model);
    printf("version_info.family: 0x%X\n", family);
    printf("version_info.processor_type: 0x%X, %s\n", u.info.processor_type, processor_type[u.info.processor_type]);
}

// 打印 CPU 品牌信息
static void show_cpu_brand()
{
    unsigned int eax, ebx, ecx, edx, brand_index;
    const unsigned int leaf = 1;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 计算品牌 index
    brand_index = 0x0FFUL & ebx;

    // 打印品牌 index
    printf("CPU brand index: 0x%X\n", brand_index);

    // TODO: 打印品牌字符串
}

// 打印 CPU 缓存行大小
static void show_cpu_cacheline_size()
{
    // 从 Pentium 4 开始才有 cacheline 信息

    unsigned int eax, ebx, ecx, edx, cacheline_size;
    const unsigned int leaf = 1;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 计算缓存行大小
    cacheline_size = ((0x0FF00U & ebx) >> 8) * 8;

    // 打印缓存行大小
    printf("CPU cacheline_size: %u\n", cacheline_size);
}

// 打印 CPU Local APIC ID
static void show_cpu_local_apic_id()
{
    // 从 Pentium 4 开始才有 cacheline 信息

    // 不同的核上运行，结果不一样。使用以下命令测试：
    // taskset -c <core id> <cmd>
    // 输出的 Local APIC ID 即 core id

    unsigned int eax, ebx, ecx, edx, local_apic;
    const unsigned int leaf = 1;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 计算 Local APIC ID
    local_apic = (0xFF000000U & ebx) >> 24;

    // 打印 Local APIC ID
    printf("CPU Local APIC ID: %u\n", local_apic);
}

// 打印 CPU 特性
static void show_cpu_features()
{
    unsigned int eax, ebx, ecx, edx;
    const unsigned int leaf = 1;

    // 获取 CPUID 信息
    __get_cpuid(leaf, &eax, &ebx, &ecx, &edx);

    // 测试并输出 CPU 特性
    printf("CPU features: ");

    // features in ECX
    cpuid_test_and_show_feature(ecx, SSE3       );
    cpuid_test_and_show_feature(ecx, PCLMULQDQ  );
    cpuid_test_and_show_feature(ecx, DTES64     );
    cpuid_test_and_show_feature(ecx, MONITOR    );
    cpuid_test_and_show_feature(ecx, DSCPL      );
    cpuid_test_and_show_feature(ecx, VMX        );
    cpuid_test_and_show_feature(ecx, SMX        );
    cpuid_test_and_show_feature(ecx, EIST       );
    cpuid_test_and_show_feature(ecx, TM2        );
    cpuid_test_and_show_feature(ecx, SSSE3      );
    cpuid_test_and_show_feature(ecx, CNXTID     );
    cpuid_test_and_show_feature(ecx, SDBG       );
    cpuid_test_and_show_feature(ecx, FMA        );
    cpuid_test_and_show_feature(ecx, CMPXCHG16B );
    cpuid_test_and_show_feature(ecx, xTPR       );
    cpuid_test_and_show_feature(ecx, PDCM       );
    cpuid_test_and_show_feature(ecx, PCID       );
    cpuid_test_and_show_feature(ecx, DCA        );
    cpuid_test_and_show_feature(ecx, SSE4_1     );
    cpuid_test_and_show_feature(ecx, SSE4_2     );
    cpuid_test_and_show_feature(ecx, x2APIC     );
    cpuid_test_and_show_feature(ecx, MOVBE      );
    cpuid_test_and_show_feature(ecx, POPCNT     );
    cpuid_test_and_show_feature(ecx, TSCDeadline);
    cpuid_test_and_show_feature(ecx, AES        );
    cpuid_test_and_show_feature(ecx, XSAVE      );
    cpuid_test_and_show_feature(ecx, OSXSAVE    );
    cpuid_test_and_show_feature(ecx, AVX        );
    cpuid_test_and_show_feature(ecx, F16C       );
    cpuid_test_and_show_feature(ecx, RDRND);

    // features in EDX
    cpuid_test_and_show_feature(edx, FPU        );
    cpuid_test_and_show_feature(edx, VME        );
    cpuid_test_and_show_feature(edx, DE         );
    cpuid_test_and_show_feature(edx, PSE        );
    cpuid_test_and_show_feature(edx, TSC        );
    cpuid_test_and_show_feature(edx, MSR        );
    cpuid_test_and_show_feature(edx, PAE        );
    cpuid_test_and_show_feature(edx, MCE        );
    cpuid_test_and_show_feature(edx, CX8        );
    cpuid_test_and_show_feature(edx, APIC       );
    cpuid_test_and_show_feature(edx, SEP        );
    cpuid_test_and_show_feature(edx, MTRR       );
    cpuid_test_and_show_feature(edx, PGE        );
    cpuid_test_and_show_feature(edx, MCA        );
    cpuid_test_and_show_feature(edx, CMOV       );
    cpuid_test_and_show_feature(edx, PAT        );
    cpuid_test_and_show_feature(edx, PSE36      );
    cpuid_test_and_show_feature(edx, PSN        );
    cpuid_test_and_show_feature(edx, CLFSH      );
    cpuid_test_and_show_feature(edx, DS         );
    cpuid_test_and_show_feature(edx, ACPI       );
    cpuid_test_and_show_feature(edx, MMX        );
    cpuid_test_and_show_feature(edx, FXSR       );
    cpuid_test_and_show_feature(edx, SSE        );
    cpuid_test_and_show_feature(edx, SSE2       );
    cpuid_test_and_show_feature(edx, SS         );
    cpuid_test_and_show_feature(edx, HTT        );
    cpuid_test_and_show_feature(edx, TM         );
    cpuid_test_and_show_feature(edx, PBE);

    // 输出换行
    putc('\n', stdout);
}

int main()
{
    show_maximum_input_value_for_basic_cpuid_info();
    show_cpu_vendor_id();
    show_cpu_version_info();
    show_cpu_brand();
    show_cpu_cacheline_size();
    show_cpu_local_apic_id();
    show_cpu_features();
}