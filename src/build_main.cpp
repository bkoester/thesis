#include <fstream>
#include <iostream>
#include <string>

#include <boost/program_options.hpp>

#include "course_container.hpp"
#include "course_network.hpp"
#include "graph_builder.hpp"
#include "mem_usage.hpp"
#include "student_container.hpp"
#include "student_network.hpp"
#include "utility.hpp"
#include "weighting_function.hpp"


using std::cerr; using std::cin; using std::cout; using std::endl;
using std::ifstream; using std::istream; using std::ofstream; using std::ostream;
using std::string;

namespace po = boost::program_options;


int main(int argc, char* argv[]) {
	po::options_description desc{"Options for network building binary:"};
	string student_archive_path, course_archive_path, weighting_function_name;
	NetworkType_e network_to_build;
	desc.add_options()
		("help,h", "Show this help message")
		("weighting_function",
		 po::value<string>(&weighting_function_name)->required(),
		 "Choose the function by which to weight connections in the network.")
		("student_archive_path",
		 po::value<string>(&student_archive_path)->required(),
		 "Set the path at which to find the student file")
		("course_archive_path",
		 po::value<string>(&course_archive_path)->required(),
		 "Set the path at which to find the enrollment file")
		("network_to_build",
		 po::value<NetworkType_e>(&network_to_build)->default_value(
			 NetworkType_e::Student), "Set the network to build "
		 "('student' or 'course')")
		("threads,t", po::value<int>(&num_threads)->default_value(1),
		 "Number of threads to use to build the network");

	po::variables_map vm;
	try {
		po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
		if (vm.count("help")) {
			cout << desc << endl;
			return 0;
		}
		po::notify(vm);
	} catch (po::required_option& e) {
		cerr << e.what() << endl;
		return -1;
	} catch (po::error& e) {
		cerr << e.what() << endl;
		return -1;
	}

	// read students and enrollment data
	ifstream student_archive{student_archive_path};
	ifstream course_archive{course_archive_path};
    StudentContainer students{
        StudentContainer::LoadFromArchive(student_archive)};
	CourseContainer courses{
        CourseContainer::LoadFromArchive(course_archive)};
	students.UpdateCourses(courses);

	if (network_to_build == NetworkType_e::Course) {
		// build the course network
		CourseNetwork course_network{
			BuildCourseNetworkFromEnrollment(students)};
		course_network.Save(cout);
	} else {
		assert(network_to_build == NetworkType_e::Student);

		// build the student network
		auto weighting_func = WeightingFuncFactory(weighting_function_name);
		StudentNetwork student_network{
			BuildStudentNetworkFromStudents(students, weighting_func)};
		student_network.Save(cout);
	}

	return 0;
}
