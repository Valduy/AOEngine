#pragma once

#include <stdexcept>
#include <numeric>
#include <vector>
#include <array>

#include "../Core/Math.h"

struct Circle2D {
	aoe::Vector2f center;
	float radius;

	Circle2D(aoe::Vector2f center, float radius)
		: center(center)
		, radius(radius)
	{}
};

struct Edge2D {
	aoe::Vector2f a;
	aoe::Vector2f b;

	Edge2D(aoe::Vector2f a, aoe::Vector2f b)
		: a(a)
		, b(b)
	{}

	bool IsSame(const Edge2D& other) const {
		return aoe::Math::IsEqual(a, other.a) && aoe::Math::IsEqual(b, other.b)
			|| aoe::Math::IsEqual(a, other.b) && aoe::Math::IsEqual(b, other.a);
	}
};

struct Triangle2D {
	static const size_t kVerticesCount = 3;

	std::array<aoe::Vector2f, kVerticesCount> vertices;

	Triangle2D(std::array<aoe::Vector2f, kVerticesCount> vertices)
		: vertices(vertices)
	{}

	Triangle2D(aoe::Vector2f a, aoe::Vector2f b, aoe:: Vector2f c)
		: vertices{a, b, c}
	{}
	
	aoe::Vector2f A() const { return vertices[0]; }
	aoe::Vector2f& A() { return vertices[0]; }

	aoe::Vector2f B() const { return vertices[1]; }
	aoe::Vector2f& B() { return vertices[1]; }

	aoe::Vector2f C() const { return vertices[2]; }
	aoe::Vector2f& C() { return vertices[2]; }

	bool IsSame(const Triangle2D& other) {
		return HasVertex(other.A())
			&& HasVertex(other.B())
			&& HasVertex(other.C());
	}

	bool HasVertex(const aoe::Vector2f& vertex) const {
		return std::any_of(vertices.begin(), vertices.end(),
			[&vertex](const aoe::Vector2f& v) {
				return aoe::Math::IsEqual(vertex, v);
			});
	}

	std::array<Edge2D, 3> ToEdges() const {
		return {
			Edge2D(A(), B()),
			Edge2D(B(), C()),
			Edge2D(C(), A()),
		};
	}

	bool HasEdge(const Edge2D& edge) const {
		std::array<Edge2D, 3> edges = ToEdges();
		return std::any_of(edges.begin(), edges.end(),
			[&edge](const Edge2D e) {
				return edge.IsSame(e);
			});
	}
};

class Algorithms {
public:
	Algorithms() = delete;

	// Bowyer-Watson triangulation algorithm. Result may be not perfectly 'Delaunay'.
	static std::vector<Triangle2D> Delaunay2D(const std::vector<aoe::Vector2f>& points) {
		std::vector<Triangle2D> triangulation;
		std::vector<Triangle2D> bad_triangles;
		std::vector<Edge2D> polygon;

		// Super triangle cover all points.
		Triangle2D super_triangle = FindSuperTriangle(points);
		triangulation.push_back(super_triangle);

		// Build triangulation point by point.
		for (const aoe::Vector2f& point : points) {
			bad_triangles.clear();
			polygon.clear();

			// Find invalid triangles relative to a new point.
			for (const Triangle2D& triangle : triangulation) {
				if (Algorithms::IsInBoundingCircle(triangle, point)) {
					bad_triangles.push_back(triangle);
				}
			}

			// Find polygin for rebuilding.
			for (const Triangle2D& bad_triangle : bad_triangles) {
				for (const Edge2D& edge : bad_triangle.ToEdges()) {
					size_t edge_count = std::count_if(bad_triangles.begin(), bad_triangles.end(), 
						[&edge](const Triangle2D& t) {
							return t.HasEdge(edge);
						});

					// IF: is not shared by multiple triangles.
					if (edge_count == 1) {
						polygon.push_back(edge);
					}
				}
			}

			// Remove bad triangles from triangulation.
			for (const Triangle2D& bad_triangle : bad_triangles) {
				for (size_t i = 0; i < triangulation.size(); ++i) {
					if (triangulation[i].IsSame(bad_triangle)) {
						auto it = std::next(triangulation.begin(), i);
						triangulation.erase(it);
						i -= 1;
					}
				}
			}

			// Re-triangulate polygon.
			for (const Edge2D& edge : polygon) {
				Triangle2D triangle(edge.a, edge.b, point);
				triangulation.push_back(triangle);
			}
		}

		// Clean up super triangle.
		for (const aoe::Vector2f& vertex : super_triangle.vertices) {
			for (size_t i = 0; i < triangulation.size(); ++i) {
				if (triangulation[i].HasVertex(vertex)) {
					auto it = std::next(triangulation.begin(), i);
					triangulation.erase(it);
					i -= 1;
				}
			}
		}

		return triangulation;
	}

	static Triangle2D FindSuperTriangle(const std::vector<aoe::Vector2f>& points) {
		Circle2D bounding_circle = FindBoundingCircle(points);
		// Can multiply radius by 1 / tg(30), but suggest to take with margin.
		float half_edge = bounding_circle.radius * 2.0f;

		aoe::Vector2f a = bounding_circle.center + aoe::Vector2f(-half_edge, -bounding_circle.radius);
		aoe::Vector2f b = bounding_circle.center + aoe::Vector2f(half_edge, -bounding_circle.radius);
		aoe::Vector2f c = bounding_circle.center + aoe::Vector2f(0, bounding_circle.radius * 2.0f);

		return { a, b, c };
	}

	static Circle2D FindBoundingCircle(const std::vector<aoe::Vector2f>& points) {
		if (points.size() == 0) {
			throw std::runtime_error("Can't find bounding circle for empty points set.");
		}

		aoe::Vector2f centroid = FindCentroid(points);
		aoe::Vector2f farthest = *std::max_element(points.begin(), points.end(),
			[centroid](const aoe::Vector2f& lhs, const aoe::Vector2f& rhs) {
				float to_lhs_distance = aoe::Vector2f::DistanceSquared(centroid, lhs);
				float to_rhs_distance = aoe::Vector2f::DistanceSquared(centroid, rhs);
				return to_lhs_distance < to_rhs_distance;
			});
		
		float radius = aoe::Vector2f::Distance(centroid, farthest);
		return { centroid, radius };
	}

	static aoe::Vector2f FindCentroid(const std::vector<aoe::Vector2f>& points) {\
		aoe::Vector2f sum = std::accumulate(points.begin(), points.end(), aoe::Math::kZeros2f);
		return sum / static_cast<float>(points.size());
	}

	static bool IsInBoundingCircle(const Triangle2D& triangle, const aoe::Vector2f& p) {
		aoe::Vector2f p1, p2, p3;

		for (size_t i = 0; i < triangle.vertices.size(); ++i) {
			p1 = triangle.vertices[(i + 0) % triangle.vertices.size()];
			p2 = triangle.vertices[(i + 1) % triangle.vertices.size()];
			p3 = triangle.vertices[(i + 2) % triangle.vertices.size()];

			if (!aoe::Math::IsEqual(p1.x, p2.x) && !aoe::Math::IsEqual(p2.x, p3.x)) {
				break;
			}
		}

		// Let line, passing throug points p1 and p2 is a.
		// Let line, passing throug points p2 and p3 is b.
		float ka = (p2.y - p1.y) / (p2.x - p1.x);
		float kb = (p3.y - p2.y) / (p3.x - p2.x);
		
		// [p1, p2] || [p2, p3].
		if (aoe::Math::IsEqual(ka, kb)) {
			return false;
		}

		aoe::Vector2f ha = (p1 + p2) / 2.0f; // [p1, p2] mid point.
		aoe::Vector2f hb = (p2 + p3) / 2.0f; // [p2, p3] mid point.

		// Perpendicular bisector equation for [p1, p2]: y = -1 / ka * (x - ha.x) + ha.y
		// Perpendicular bisector equation for [p2, p3]: y = -1 / kb * (x - hb.x) + hb.y 
		// Express x0 via both perpendicular bisector equations.
		float x0 = (ka * kb * (p1.y - p3.y) + kb * (p1.x + p2.x) - ka * (p2.x + p3.x)) / (2 * (kb - ka));
		float y0 = !aoe::Math::IsEqual(ka, 0.0f) 
			? (-(x0 - ha.x) / ka + ha.y)  // Use [p1, p2] perpendicula bisector equation if it's not vertical.
			: (-(x0 - hb.x) / kb + hb.y); // Use [p2, p3] perpendicula bisector equation in other case.
		
		aoe::Vector2f p0(x0, y0);

		float rr = aoe::Math::Pow(p1.x - x0, 2) + aoe::Math::Pow(p1.y - y0, 2);
		return aoe::Vector2f::DistanceSquared(p0, p) <= rr;
	}
};