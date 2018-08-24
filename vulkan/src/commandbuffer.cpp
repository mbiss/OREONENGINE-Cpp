#include "commandbuffer.h"
#include "util.h"

using namespace vk;

CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBufferLevel level)
{
	VkCommandBufferAllocateInfo commandbufferAllocateInfo = {};
	commandbufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandbufferAllocateInfo.commandPool = commandPool;
	commandbufferAllocateInfo.level = level;
	commandbufferAllocateInfo.commandBufferCount = 1;

	VkResult vkResult = vkAllocateCommandBuffers(device, &commandbufferAllocateInfo, &handle);

	if (vkResult != VK_SUCCESS) {
		std::cout << "Failed to allocate command buffer: "
			<< translateVkResult(vkResult) << std::endl;
	}
}

void vk::CommandBuffer::beginRecord(VkCommandBufferUsageFlags usageFlags)
{
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = usageFlags;

	VkResult vkResult = vkBeginCommandBuffer(handle, &commandBufferBeginInfo);

	if (vkResult != VK_SUCCESS) {
		std::cout << "Failed to begin record command buffer: "
			<< translateVkResult(vkResult) << std::endl;
	}
}

void vk::CommandBuffer::beginRecordSecondary(VkCommandBufferUsageFlags usageFlags,
	VkFramebuffer framebuffer, VkRenderPass renderPass, int subpass)
{
	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.framebuffer = framebuffer;
	inheritanceInfo.renderPass = renderPass;
	inheritanceInfo.subpass = subpass;
	inheritanceInfo.occlusionQueryEnable = VK_FALSE;
	inheritanceInfo.queryFlags = 0;
	inheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.flags = usageFlags;
	commandBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

	VkResult vkResult = vkBeginCommandBuffer(handle, &commandBufferBeginInfo);

	if (vkResult != VK_SUCCESS) {
		std::cout << "Failed to begin record secondary command buffer: "
			<< translateVkResult(vkResult) << std::endl;
	}
}

void vk::CommandBuffer::finishRecord()
{
	VkResult vkResult = vkEndCommandBuffer(handle);

	if (vkResult != VK_SUCCESS) {
		std::cout << "Failed to finish record command buffer: "
			<< translateVkResult(vkResult) << std::endl;
	}
}

void vk::CommandBuffer::beginRenderPassCmd(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	uint32_t width, uint32_t height, int colorAttachmentCount, int depthAttachment, VkSubpassContents subpassContentsFlag)
{
	VkClearValue* clearValues = new VkClearValue[colorAttachmentCount + depthAttachment];

	for (int i = 0; i < colorAttachmentCount; i++) {
		VkClearColorValue clearColorValue = { 0.0f, 0.0f, 0.0f, 1.0f };
		VkClearValue clearValue = {};
		clearValue.color = clearColorValue;
		clearValues[i] = clearValue;
	}
	if (depthAttachment == 1) {
		VkClearDepthStencilValue clearDepthStencilValue = { 1.0f };
		VkClearValue clearValue = {};
		clearValue.depthStencil = clearDepthStencilValue;
		clearValues[(colorAttachmentCount + depthAttachment) - 1] = clearValue;
	}

	this->beginRenderPassCmd(renderPass, frameBuffer, width, height, clearValues, subpassContentsFlag);
}

void vk::CommandBuffer::beginRenderPassCmd(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	uint32_t width, uint32_t height, int colorAttachmentCount, int depthAttachment, float clearR, float clearG, float clearB, float clearA,
	VkSubpassContents subpassContentsFlag)
{
	VkClearValue* clearValues = new VkClearValue[colorAttachmentCount + depthAttachment];

	for (int i = 0; i < colorAttachmentCount; i++) {
		VkClearColorValue clearColorValue = { clearR, clearG, clearB, clearA };
		VkClearValue clearValue = {};
		clearValue.color = clearColorValue;
		clearValues[i] = clearValue;
	}
	if (depthAttachment == 1) {
		VkClearDepthStencilValue clearDepthStencilValue = { 1.0f };
		VkClearValue clearValue = {};
		clearValue.depthStencil = clearDepthStencilValue;
		clearValues[(colorAttachmentCount + depthAttachment) - 1] = clearValue;
	}

	this->beginRenderPassCmd(renderPass, frameBuffer, width, height, clearValues, subpassContentsFlag);
}

void vk::CommandBuffer::endRenderPassCmd()
{
	vkCmdEndRenderPass(handle);
}

void vk::CommandBuffer::bindComputePipelineCmd(VkPipeline pipeline)
{
	vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void vk::CommandBuffer::bindGraphicsPipelineCmd(VkPipeline pipeline)
{
	vkCmdBindPipeline(handle, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void vk::CommandBuffer::pushConstantsCmd(VkPipelineLayout pipelineLayout,
	VkPipelineStageFlags stageFlags, const void* pData)
{
	vkCmdPushConstants(handle, pipelineLayout, stageFlags, 0, sizeof(pData), pData);
}

void vk::CommandBuffer::bindVertexInputCmd(VkBuffer vertexBuffer, VkBuffer indexBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(handle, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(handle, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void vk::CommandBuffer::bindVertexInputCmd(VkBuffer vertexBuffer)
{
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(handle, 0, 1, vertexBuffers, offsets);
}

void vk::CommandBuffer::bindComputeDescriptorSetsCmd(VkPipelineLayout pipelinyLayout,
	const VkDescriptorSet * pDescriptorSets)
{
	bindDescriptorSetsCmd(pipelinyLayout, pDescriptorSets,
		VK_PIPELINE_BIND_POINT_COMPUTE);
}

void vk::CommandBuffer::bindGraphicsDescriptorSetsCmd(VkPipelineLayout pipelinyLayout,
	const VkDescriptorSet * pDescriptorSets)
{
	bindDescriptorSetsCmd(pipelinyLayout, pDescriptorSets,
		VK_PIPELINE_BIND_POINT_GRAPHICS);
}

void vk::CommandBuffer::clearColorImageCmd(VkImage image, VkImageLayout imageLayout)
{
	VkImageSubresourceRange subresourceRange = {};
	subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRange.baseMipLevel = 0;
	subresourceRange.levelCount = 1;
	subresourceRange.baseArrayLayer = 0;
	subresourceRange.layerCount = 1;

	VkClearColorValue clearColorValue = { 0, 0, 0, 1 };

	vkCmdClearColorImage(handle, image, imageLayout, &clearColorValue, 1, &subresourceRange);
}

void vk::CommandBuffer::drawIndexedCmd(uint32_t indexCount)
{
	vkCmdDrawIndexed(handle, indexCount, 1, 0, 0, 0);
}

void vk::CommandBuffer::drawCmd(uint32_t vertexCount)
{
	vkCmdDraw(handle, vertexCount, 1, 0, 0);
}

void vk::CommandBuffer::dispatchCmd(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
	vkCmdDispatch(handle, groupCountX, groupCountY, groupCountZ);
}

void vk::CommandBuffer::copyBufferCmd(VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t srcOffset, uint64_t dstOffset, uint64_t size)
{
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;

	vkCmdCopyBuffer(handle, srcBuffer, dstBuffer, 1, &copyRegion);
}

void vk::CommandBuffer::copyBufferToImageCmd(VkBuffer srcBuffer, VkImage dstImage, uint32_t width, uint32_t height, uint32_t depth)
{
	VkBufferImageCopy copyRegion = {};
	copyRegion.bufferOffset = 0;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;

	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = 0;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;

	copyRegion.imageOffset = { 0, 0, 0 };
	copyRegion.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(handle, srcBuffer, dstImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
}

void vk::CommandBuffer::beginRenderPassCmd(VkRenderPass renderPass, VkFramebuffer frameBuffer,
	uint32_t width, uint32_t height, VkClearValue* clearValues, VkSubpassContents subpassContentsFlag)
{
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.framebuffer = frameBuffer;

	VkRect2D renderArea = {};
	renderArea.offset.x = 0;
	renderArea.offset.y = 0;
	renderArea.extent.width = width;
	renderArea.extent.height = height;

	vkCmdBeginRenderPass(handle, &renderPassBeginInfo, subpassContentsFlag);
}

void vk::CommandBuffer::bindDescriptorSetsCmd(VkPipelineLayout pipelinyLayout,
	const VkDescriptorSet* pDescriptorSets, VkPipelineBindPoint pipelineBindPoint)
{
	vkCmdBindDescriptorSets(handle, pipelineBindPoint,
		pipelinyLayout, 0, 1, pDescriptorSets, 0, nullptr);
}
