#pragma once

#include <vector>

#include "Colors.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"
#include "DX11DebugPassSystem.h"

namespace aoe {

using Segment = std::vector<Vector3f>;
using Line = std::vector<Segment>;

class DX11DebugPassSystem;

class DX11LineComponent {
private:
	friend class DX11DebugPassSystem;

public:
	DX11LineComponent(Vector3f color, Line line)
		: color_(color)
		, line_(std::move(line))
		, color_data_()
		, transform_data_()
		, line_resources_()
	{
		UpdateColorData();
		UpdateLineResources();
	}

	const Vector3f& GetColor() const {
		return color_;
	}

	void SetColor(Vector3f value) {
		color_ = value;
		UpdateColorData();
	}

	const Line& GetLine() const {
		return line_;
	}

	void SetLine(Line value) {
		line_ = std::move(value);
		UpdateLineResources();
	}

	const DX11RenderData<Vector3fData>& GetColorData() const {
		return color_data_;
	}

	const DX11RenderData<Matrix4f>& GetTransformData() const {
		return transform_data_;
	}

	const std::vector<DX11GPUBuffer>& GetLineResources() const {
		return line_resources_;
	}

private:
	Vector3f color_;
	Line line_;
	DX11RenderData<Vector3fData> color_data_;
	DX11RenderData<Matrix4f> transform_data_;
	std::vector<DX11GPUBuffer> line_resources_;

	void UpdateColorData() {
		Vector3fData data{};
		data.value = color_;

		color_data_.Update(&data);
	}

	void UpdateLineResources() {
		line_resources_.clear();

		for (const Segment& segment : line_) {
			line_resources_.emplace_back(CreateVertexBuffer(segment));
		}
	}

	DX11GPUBuffer CreateVertexBuffer(const Segment& segment) {
		const aoe::GPUBufferDescription vertex_buffer_desc{
			aoe::GPUBufferType::kVertexBuffer,
			aoe::GPUResourceUsage::kDefault
		};

		return DX11GPUBuffer::Create<Vector3f>(vertex_buffer_desc, segment.data(), segment.size());
	}
};

} // namespace aoe