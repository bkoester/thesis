#ifndef COURSE_CONTAINER_H
#define COURSE_CONTAINER_H

#include <initializer_list>
#include <string>
#include <vector>

#include <boost/serialization/vector.hpp>

#include "course.hpp"


// incomplete declaration
class StudentContainer;


class CourseContainer {
 public:
	using container_t = std::vector<Course>;

	// Read Student::Ids and courses they took from enrollment data.
	// Populate a set of courses and which students took them.
    static CourseContainer LoadFromTsv(std::istream& enrollment_stream);
	// Load the course container from a Boost archive.
	static CourseContainer LoadFromArchive(std::istream& input_archive);
	static CourseContainer LoadFromArchive(std::string input_path);
    // Save the course container to a Boost archive.
	void SaveToArchive(std::ostream& output);

	virtual ~CourseContainer() {}

	bool operator==(const CourseContainer& other) const
	{ return courses_ == other.courses_; }

	container_t::size_type size() { return courses_.size(); }

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int) { ar & courses_; }

	// Inserts and takes ownership of course.
	container_t::iterator Insert(Course course);
	void Insert(std::initializer_list<Course> courses);

	const Course& Find(Course course) const;
	Course& Find(Course course);

	virtual container_t::iterator begin() { return std::begin(courses_); }
	virtual container_t::const_iterator begin() const
	{ return std::begin(courses_); }
	virtual container_t::const_iterator cbegin() const
	{ return courses_.cbegin(); }

	virtual container_t::iterator end() { return std::end(courses_); }
	virtual container_t::const_iterator end() const
	{ return std::end(courses_); }
	virtual container_t::const_iterator cend() const { return courses_.cend(); }

 protected:
	CourseContainer() {}  // use "Load..." static member functions instead

 private:
    friend class CourseContainerTest;  // to access default constructor

	container_t courses_;
};


class CourseNotFound : public std::exception {
 public:
	CourseNotFound(const Course& course) :
		error_message_{"Course " + course.subject() + " "
			+ std::to_string(course.number()) + " "
			+ std::to_string(course.term()) + " not found!"} {}
	const char* what() const noexcept { return error_message_.c_str(); }

 private:
	std::string error_message_;
};


#endif  // COURSE_CONTAINER_H
