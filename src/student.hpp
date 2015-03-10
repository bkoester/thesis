#ifndef STUDENT_H
#define STUDENT_H

#include <functional>
#include <iosfwd>
#include <initializer_list>
#include <set>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/serialization/utility.hpp>


class Course;


using StudentId = int;
using StudentIdHasher = std::hash<int>;


class Student {
 public:
	enum class Gender { Male, Female, Unspecified };
	enum class Ethnicity { Hispanic_only, American_Indian, Asian, Black, 
		Pacific_Islander, White, Multiracial, Unknown, Undocumented };

	Student() : id_{uninitialized_id}, gender_{Gender::Unspecified}, 
		ethnicity_{uninitialized_ethnicity},
		first_term_{uninitialized_first_term} {}
	Student(StudentId id) : id_{id}, gender_{Gender::Unspecified}, 
		ethnicity_{uninitialized_ethnicity},
		first_term_{uninitialized_first_term} {}

	int id() const { return id_; }
	Gender gender() const { return gender_; }
	Ethnicity ethnicity() const { return ethnicity_; }
	int first_term() const { return first_term_; }
	int degree_term() const { return degree_term_; }
	bool transfer() const { return transfer_; }
	boost::optional<double> major1() const { return major1_; }
	boost::optional<double> major2() const { return major2_; }
	std::string school() const { return school_; }

	std::string GetMajor1Description() const;
	std::string GetMajor2Description() const;

	bool operator==(const Student& other) const { return id() == other.id(); }
	bool operator!=(const Student& other) const { return !(operator==(other)); }
	bool operator<(const Student& other) const { return id() < other.id(); }

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version) { ar & id_; }

	void AddCourseTaken(const Course* course) { courses_taken_.insert(course); }
	void AddCoursesTaken(std::initializer_list<const Course*> courses);
	bool HasTakenCourse(const Course* course) const
	{ return courses_taken_.count(course) == 1; }
	// provide access to the container to allow stl algorithms to be run on them
	const std::set<const Course*>& courses_taken() const
	{ return courses_taken_; }

	double GetTotalCreditsTaken() const;

 private:
	friend class StudentTest;
	friend class boost::serialization::access;
	friend std::ostream& operator<<(std::ostream& os, const Student& student);
	friend std::istream& operator>>(std::istream& input, Student& student);

	StudentId id_;
	Gender gender_;
	Ethnicity ethnicity_;
	int first_term_;
	int degree_term_;
	bool transfer_;
	boost::optional<double> major1_;
	boost::optional<double> major2_;
	std::string school_;
	std::set<const Course*> courses_taken_;
	static const int uninitialized_id;
	static const int uninitialized_first_term;
	static const Ethnicity uninitialized_ethnicity;
	static const std::unordered_map<double, std::string> major_code_map;
};


std::istream& operator>>(std::istream& input, Student::Gender& gender);
std::ostream& operator<<(std::ostream& output, const Student::Gender& gender);
std::istream& operator>>(std::istream& input, Student::Ethnicity& ethnicity);
std::ostream& operator<<(
		std::ostream& output, const Student::Ethnicity& ethnicity);


struct StudentHasher {
	size_t operator()(const Student& student) const 
	{ return std::hash<int>()(student.id()); }
};


#endif  // STUDENT_H
