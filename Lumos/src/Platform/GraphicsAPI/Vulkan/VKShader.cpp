#include "LM.h"
#include "VKShader.h"

#include "VKDevice.h"
#include "System/FileSystem.h"

namespace Lumos
{
	namespace graphics
	{
		ShaderType type = ShaderType::UNKNOWN;

		VKShader::VKShader(const std::string& shaderName, const std::string& filePath): m_StageCount(0), m_Name(shaderName), m_FilePath(filePath)
		{
			m_ShaderStages = NULL;

			m_Source = VFS::Get()->ReadTextFile(filePath + shaderName +  ".shader");

			Init();
		}

		VKShader::~VKShader()
		{
			Unload();
            delete[] m_ShaderStages;
			m_ShaderStages = nullptr;
		}

		VkShaderStageFlagBits ShaderTypeToVK(const ShaderType& shaderName)
		{
			switch (shaderName)
			{
			case ShaderType::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderType::GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
			case ShaderType::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderType::TESSELLATION_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case ShaderType::TESSELLATION_EVALUATION: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case ShaderType::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
			case ShaderType::UNKNOWN: return VK_SHADER_STAGE_VERTEX_BIT;
			default: return VK_SHADER_STAGE_VERTEX_BIT;
			}
		}

		bool VKShader::Init()
		{
			VkResult result;
			uint32_t currentShaderStage = 0;
			m_StageCount = 0;

			std::map<ShaderType, String>* files = new std::map<ShaderType, String>();
			PreProcess(m_Source, files);

			for (auto& source : *files)
			{
				m_ShaderTypes.push_back(source.first);
				m_StageCount++;
			}

			m_ShaderStages = new VkPipelineShaderStageCreateInfo[m_StageCount];

			for (uint32_t i = 0; i < m_StageCount; i++)
				m_ShaderStages[i] = {};

			for (auto& file : *files)
			{
				auto fileSize = FileSystem::GetFileSize(m_FilePath + file.second); //TODO: once process
				byte* source = FileSystem::ReadFile(m_FilePath + file.second);
				VkShaderModuleCreateInfo vertexShaderCI{};
				vertexShaderCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				vertexShaderCI.codeSize = fileSize;
				vertexShaderCI.pCode = reinterpret_cast<uint32_t*>(source);
				vertexShaderCI.pNext = VK_NULL_HANDLE;
				vertexShaderCI.flags = 0;

				m_ShaderStages[currentShaderStage].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				m_ShaderStages[currentShaderStage].stage = ShaderTypeToVK(file.first);
				m_ShaderStages[currentShaderStage].pName = "main";
				m_ShaderStages[currentShaderStage].pNext = VK_NULL_HANDLE;
				m_ShaderStages[currentShaderStage].flags = 0;

				result = vkCreateShaderModule(VKDevice::Instance()->GetDevice(), &vertexShaderCI, VK_NULL_HANDLE, &m_ShaderStages[currentShaderStage].module);
                
                delete source;

				if (result != VK_SUCCESS)
                {
                    delete files;
                    return false;
                }
				currentShaderStage++;
			}

            delete files;
			return true;
		}

		void VKShader::Unload() const
		{
			for (uint32_t i = 0; i < m_StageCount; i++)
				vkDestroyShaderModule(VKDevice::Instance()->GetDevice(), m_ShaderStages[i].module, VK_NULL_HANDLE);
		}

		VkPipelineShaderStageCreateInfo* VKShader::GetShaderStages() const
		{
			return m_ShaderStages;
		}

		uint32_t VKShader::GetStageCount() const
		{
			return m_StageCount;
		}

		void VKShader::PreProcess(const String& source, std::map<ShaderType, String>* sources)
		{
			type = ShaderType::UNKNOWN;
			std::vector<String> lines = GetLines(source);
			ReadShaderFile(lines, sources);
		}

		void VKShader::ReadShaderFile(std::vector<String> lines, std::map<ShaderType, String>* shaders)
		{
			for (uint i = 0; i < lines.size(); i++)
			{
				String str = String(lines[i]);
				str = StringReplace(str, '\t');

				if (StartsWith(str, "#shader"))
				{
					if (StringContains(str, "vertex"))
					{
						type = ShaderType::VERTEX;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "geometry"))
					{
						type = ShaderType::GEOMETRY;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "fragment"))
					{
						type = ShaderType::FRAGMENT;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "tess_cont"))
					{
						type = ShaderType::TESSELLATION_CONTROL;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "tess_eval"))
					{
						type = ShaderType::TESSELLATION_EVALUATION;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "compute"))
					{
						type = ShaderType::COMPUTE;
						std::map<ShaderType, String>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, String>(type, ""));
					}
					else if (StringContains(str, "end"))
					{
						type = ShaderType::UNKNOWN;
					}
				}
				else if (type != ShaderType::UNKNOWN)
				{
					shaders->at(type).append(lines[i]);
				}
			}
		}

	}
}