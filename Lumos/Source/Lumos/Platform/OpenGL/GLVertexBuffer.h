#pragma once

#include "Graphics/RHI/VertexBuffer.h"
#include "GLDebug.h"

namespace Lumos
{
    namespace Graphics
    {
        class GLVertexBuffer : public VertexBuffer
        {
        private:
            uint32_t m_Handle {};
            BufferUsage m_Usage;
            uint32_t m_Size;
            bool m_Mapped = false;

        public:
            explicit GLVertexBuffer(BufferUsage usage);
            ~GLVertexBuffer();

            void Resize(uint32_t size) override;
            void SetData(uint32_t size, const void* data, bool addBarrier = false) override;
            void SetDataSub(uint32_t size, const void* data, uint32_t offset) override;

            void ReleasePointer() override;

            void Bind(CommandBuffer* commandBuffer, Pipeline* pipeline, uint8_t binding = 0) override;
            void Unbind() override;
            uint32_t GetSize() override { return m_Size; }

            static void MakeDefault();

        protected:
            static VertexBuffer* CreateFuncGL(const BufferUsage& usage);
            static VertexBuffer* CreateFuncWithDataGL(uint32_t size, const void* data, const BufferUsage& usage);

        protected:
            void* GetPointerInternal() override;
        };
    }
}
