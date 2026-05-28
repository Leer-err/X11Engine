#include "Graphics.h"

#include <SDL3/SDL_vulkan.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "AppConfig.h"
#include "ExtensionFunctions.h"
#include "LoggerFactory.h"
#include "Queue.h"
#include "RenderEngine.h"
#include "RenderingBackend.h"
#include "Result.h"
#include "VkBootstrap.h"
#include "Window.h"

namespace Graphics {

enum class Error {
    InstanceCreationFailed,
    NoSuitableDevice,
    DeviceCreationFailed,
    QueueCreationFailed,
    AllocatorCreationFailed
};

static RenderEngine* render_engine = nullptr;

static Result<vkb::Instance, Error> createInstance(const Config::App& config) {
    vkb::InstanceBuilder builder;
    builder.set_app_name(config.getName().c_str()).require_api_version(1, 3, 0);

#ifndef NDEBUG
    builder.request_validation_layers().use_default_debug_messenger();
#endif

    auto inst_ret = builder.build();
    if (!inst_ret) return Error::InstanceCreationFailed;

    return inst_ret.value();
};

static Result<vkb::Device, Error> createDevice(const vkb::Instance& instance,
                                               VkSurfaceKHR surface) {
    vkb::PhysicalDeviceSelector selector{instance};
    auto phys_ret =
        selector.set_surface(surface)
            .set_minimum_version(1, 3)
            .require_dedicated_transfer_queue()
            .add_required_extension(VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME)
            .add_required_extension(VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME)
            .select();

    if (!phys_ret) {
        if (phys_ret.error() == vkb::PhysicalDeviceError::no_suitable_device)
            return Error::NoSuitableDevice;
        return Error::DeviceCreationFailed;
    }

    vkb::DeviceBuilder device_builder{phys_ret.value()};

    VkPhysicalDeviceVulkan12Features features12 = {};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.runtimeDescriptorArray = VK_TRUE;
    device_builder.add_pNext(&features12);

    VkPhysicalDeviceSynchronization2FeaturesKHR synchronization_2{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR};
    synchronization_2.synchronization2 = VK_TRUE;
    device_builder.add_pNext(&synchronization_2);

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{};
    dynamic_rendering_feature.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_rendering_feature.dynamicRendering = VK_TRUE;
    device_builder.add_pNext(&dynamic_rendering_feature);

    VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptor_buffer{
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT};
    descriptor_buffer.descriptorBuffer = VK_TRUE;
    device_builder.add_pNext(&descriptor_buffer);

    auto dev_ret = device_builder.build();
    if (!dev_ret) return Error::DeviceCreationFailed;

    return dev_ret.value();
};

static Result<Queue, Error> createQueue(const vkb::Device& device,
                                        const vkb::QueueType& queue_type) {
    auto graphics_queue_ret = device.get_queue(queue_type);
    if (!graphics_queue_ret) return Error::QueueCreationFailed;

    auto index = device.get_queue_index(queue_type).value();

    return Queue{graphics_queue_ret.value(), index};
}

static Result<VmaAllocator, Error> createAllocator(
    const vkb::Instance& instance, const vkb::Device& device) {
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorCreateInfo.physicalDevice = device.physical_device;
    allocatorCreateInfo.device = device.device;
    allocatorCreateInfo.instance = instance.instance;
    allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    VmaAllocator allocator = {};
    VkResult result = vmaCreateAllocator(&allocatorCreateInfo, &allocator);
    if (result != VK_SUCCESS) return Error::AllocatorCreationFailed;

    return allocator;
}

static void createOverlay(const vkb::Instance& instance,
                          const vkb::Device& device, Queue graphics_queue,
                          SDL_Window* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    float main_scale = 1;
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    ImGui_ImplSDL3_InitForVulkan(window);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.ApiVersion = VK_API_VERSION_1_3;
    init_info.Instance = instance;
    init_info.PhysicalDevice = device.physical_device;
    init_info.Device = device;
    init_info.QueueFamily = graphics_queue.index;
    init_info.Queue = graphics_queue.queue;
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = VK_NULL_HANDLE;
    init_info.MinImageCount = 3;
    init_info.ImageCount = 3;
    init_info.UseDynamicRendering = true;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.DescriptorPoolSize = 100;

    VkFormat swapchain_format = VK_FORMAT_R8G8B8A8_SRGB;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo
        .colorAttachmentCount = 1;
    init_info.PipelineInfoMain.PipelineRenderingCreateInfo
        .pColorAttachmentFormats = &swapchain_format;

    ImGui_ImplVulkan_Init(&init_info);
}

static void printError(const Error error) {
    auto logger = LoggerFactory::getLogger("Graphics");
    switch (error) {
        case Error::InstanceCreationFailed:
            logger.error("Vulkan instance creation failed");
            break;
        case Error::DeviceCreationFailed:
            logger.error("Vulkan device creation failed");
            break;
        case Error::QueueCreationFailed:
            logger.error("Failed to acquire queue from device");
            break;
        case Error::AllocatorCreationFailed:
            logger.error("Failed to create allocator");
            break;
        case Error::NoSuitableDevice:
            logger.error("Failed to find suitable device");
            break;
    }
}

bool init() {
    auto instance_result = createInstance(Config::App::get());
    if (instance_result.isError()) {
        printError(instance_result.getError());
        return false;
    }
    auto instance = instance_result.getResult();

    VkSurfaceKHR surface;
    auto handle = Window::get().getHandle();
    SDL_Vulkan_CreateSurface(handle, instance.instance, nullptr, &surface);
    auto device_result = createDevice(instance, surface);
    if (device_result.isError()) {
        printError(device_result.getError());
        return false;
    }
    auto device = device_result.getResult();

    auto graphics_queue_result = createQueue(device, vkb::QueueType::graphics);
    if (graphics_queue_result.isError()) {
        printError(graphics_queue_result.getError());
        return false;
    }
    auto graphics_queue = graphics_queue_result.getResult();

    auto presentation_queue_result =
        createQueue(device, vkb::QueueType::present);
    if (presentation_queue_result.isError()) {
        printError(presentation_queue_result.getError());
        return false;
    }
    auto presentation_queue = presentation_queue_result.getResult();

    auto allocator_result = createAllocator(instance, device);
    if (allocator_result.isError()) {
        printError(allocator_result.getError());
        return false;
    }
    auto allocator = allocator_result.getResult();

    loadExtensionFunctions(instance, device);

    createOverlay(instance, device, graphics_queue, handle);

    render_engine = new RenderEngine(instance, device, graphics_queue,
                                     presentation_queue, allocator, surface);

    return true;
}

IRenderEngine* getRenderEngine() { return render_engine; }

}  // namespace Graphics