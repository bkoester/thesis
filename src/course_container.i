%module course_container
%{
#include "course_container.hpp"
%}

class CourseContainer {
 public:
	//static CourseContainer LoadFromArchive(std::string input_path);
    // Only expose const member function to external clients.
	const Course& Find(Course course) const;

 protected:
    CourseContainer();
};
