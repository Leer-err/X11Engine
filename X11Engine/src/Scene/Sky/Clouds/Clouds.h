#pragma once

#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "Sampler.h"
#include "ShaderResource.h"
#include "Texture.h"

class Clouds {
    class CloudsBaker {
       public:
        CloudsBaker();

        void draw();

        Texture getCloudsTexture() const;

       private:
        Mesh screen_quad;
        GraphicsPipeline bake_pipeline;
        Texture clouds_texture;
        RenderTarget cloud_render_target;
        Buffer cloud_parameter_buffer;
    };

   public:
    Clouds();

    void draw();

   private:
    void updateCloudData();

    Mesh cloud_plane;
    GraphicsPipeline pipeline;
    Buffer camera_data_buffer;
    Buffer clouds_data_buffer;
    ShaderResource cloud_texture;

    CloudsBaker cloud_baker;

    float cloud_height;
    float cloud_plane_size;
};