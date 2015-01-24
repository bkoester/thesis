#ifndef COURSE_NETWORK_H
#define COURSE_NETWORK_H

#include <iosfwd>
#include <iterator>
#include <unordered_map>

#include "network.hpp"
#include "course.hpp"

class CourseNetwork : public Network<Course, int> {
 public:
	CourseNetwork() {}
	// Creates a graph for the vertices stored in the range first to last.
	template <typename ForwardIt>
	CourseNetwork(ForwardIt first, ForwardIt last);
	// input must be a boost graph archive
	CourseNetwork(std::istream& input);
	CourseNetwork(const graph_t& graph);

	// gets the vertex type in the graph for a specific course
	// Will throw out_of_range exception if the course doesn't exist.
	vertex_t GetVertex(const Course& course) const;

	using Network<Course, int>::operator[];
	const Course& operator[](const Course& course) const;
	Course& operator[](const Course& course);

	void Load(std::istream& input_graph_archive);

	int CalculateValue(const vertex_t& source, const vertex_t& target) const;


 private:
	// Fill out the hash table of courses => vertices.
	void BuildCourseToVertexMap();

	std::unordered_map<Course, vertex_t, CourseHasher> course_to_vertex_;
};

std::ostream& operator<<(
		std::ostream& output, const CourseNetwork& course_network);

template <typename ForwardIt>
CourseNetwork::CourseNetwork(ForwardIt first, ForwardIt last) : 
		Network{static_cast<long unsigned int>(std::distance(first, last))} {
	auto it = first;
	// assign the courses to the vertices and build the course to vertex map
	for (auto vertex : GetVertices()) {
		course_to_vertex_[*it] = vertex;
		Network<Course, int>::operator[](vertex) = *it++;
	}
}




#endif  // COURSE_NETWORK_H
