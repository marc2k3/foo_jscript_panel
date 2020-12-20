#pragma once

static constexpr size_t kNumColourComponents = 3U;
static constexpr size_t kMaxIterations = 12U;
static constexpr size_t kMaxCount = 14U;
using ColourValues = std::array<double, kNumColourComponents>;

class KPoint
{
public:
	KPoint(size_t point_id, const ColourValues& values, size_t pixel_count) : m_point_id(point_id), m_values(values), m_pixel_count(pixel_count), m_cluster_id(SIZE_MAX) {}

	ColourValues m_values;
	size_t m_cluster_id, m_pixel_count, m_point_id;
};

using KPoints = std::vector<KPoint>;

class Cluster
{
public:
	Cluster() {}

	Cluster(const KPoint& point) : central_values(point.m_values)
	{
		points.emplace_back(point);
	}

	double calc_dist(const KPoint& point) const
	{
		double sum = 2.0 * pow(central_values[0] - point.m_values[0], 2.0);
		sum += 4.0 * pow(central_values[1] - point.m_values[1], 2.0);
		sum += 3.0 * pow(central_values[2] - point.m_values[2], 2.0);
		return sum;
	}

	double get_frequency(size_t colours_length)
	{
		return get_total_points() / static_cast<double>(colours_length);
	}

	int get_colour()
	{
		return static_cast<int>(Gdiplus::Color::MakeARGB(UINT8_MAX, get_colour_component(0), get_colour_component(1), get_colour_component(2)));
	}

	size_t get_total_points() const
	{
		return std::accumulate(points.begin(), points.end(), 0U, [](size_t t, const KPoint& point)
			{
				return t + point.m_pixel_count;
			});
	}

	uint8_t get_colour_component(size_t index)
	{
		return static_cast<uint8_t>(central_values[index]);
	}

	void remove_point(size_t point_id)
	{
		auto it = std::remove_if(points.begin(), points.end(), [point_id](const KPoint& point)
			{
				return point.m_point_id == point_id;
			});
		points.erase(it, points.end());
	}

	ColourValues central_values{};
	KPoints points;
};

using Clusters = std::vector<Cluster>;

class KMeans
{
public:
	KMeans(const KPoints& points, size_t count) : m_points(points), m_count(count) {}

	Clusters run()
	{
		size_t count = std::min(std::max(m_count, kMaxCount), m_points.size());

		for (size_t i = 0; i < count; ++i)
		{
			const size_t index_point = i * m_points.size() / count;
			m_points[index_point].m_cluster_id = i;
			Cluster cluster(m_points[index_point]);
			m_clusters.emplace_back(cluster);
		}

		for (size_t i = 0; i < kMaxIterations; ++i)
		{
			bool done = true;

			for (KPoint& point : m_points)
			{
				const size_t old_cluster_id = point.m_cluster_id;
				const size_t nearest_centre_id = get_nearest_centre_id(point);

				if (old_cluster_id != nearest_centre_id)
				{
					if (old_cluster_id != SIZE_MAX)
					{
						m_clusters[old_cluster_id].remove_point(point.m_point_id);
					}

					point.m_cluster_id = nearest_centre_id;
					m_clusters[nearest_centre_id].points.emplace_back(point);
					done = false;
				}
			}

			for (Cluster& cluster : m_clusters)
			{
				for (size_t j = 0; j < kNumColourComponents; ++j)
				{
					const size_t cluster_total_points = cluster.get_total_points();
					if (cluster_total_points == 0) continue;

					const double sum = std::accumulate(cluster.points.begin(), cluster.points.end(), 0.0, [j](double t, const KPoint& point)
						{
							return t + (point.m_values[j] * point.m_pixel_count);
						});

					cluster.central_values[j] = sum / cluster_total_points;
				}
			}

			if (done) break;
		}

		std::ranges::sort(m_clusters, [](const Cluster& a, const Cluster& b)
			{
				return a.get_total_points() > b.get_total_points();
			});

		if (m_count < m_clusters.size()) m_clusters.resize(m_count);
		return m_clusters;
	}

private:
	size_t get_nearest_centre_id(const KPoint& point)
	{
		double min_dist = m_clusters[0].calc_dist(point);
		size_t cluster_centre_id = 0;

		for (size_t i = 1; i < m_clusters.size(); ++i)
		{
			const double dist = m_clusters[i].calc_dist(point);

			if (dist < min_dist)
			{
				min_dist = dist;
				cluster_centre_id = i;
			}
		}

		return cluster_centre_id;
	}

	Clusters m_clusters;
	KPoints m_points;
	size_t m_count;
};
