// CS 300 Project Two
// ABCU Advising Assistance Program
// Eriyana Charles
// October 2026

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Structure to store course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};
// Node structure for the Binary Search Tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Constructor initializes a new node
    Node(Course newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};
// Binary Search Tree class to organize courses
class BinarySearchTree {
private:
    Node* root;
    // Recursively delete all nodes to prevent memory leaks
    void destroyTree(Node* node) {
        if (node == nullptr) {
            return;
        }

        // Delete the left and right subtrees first
        destroyTree(node->left);
        destroyTree(node->right);

        // Release memory for the current node
        delete node;
    }
    // Print courses using in-order traversal
    void printInOrder(Node* node) {
        if (node == nullptr) {
            return;
        }

        // Visit left subtree first
        printInOrder(node->left);

        // Display the current course
        cout << node->course.courseNumber << ", "
            << node->course.courseName << endl;

        // Visit right subtree last
        printInOrder(node->right);
    }
    // Insert a course into the correct location
    Node* insertNode(Node* node, Course course) {

        // If the location is empty, create a new node
        if (node == nullptr) {
            return new Node(course);
        }

        // Smaller course numbers go to the left
        if (course.courseNumber < node->course.courseNumber) {
            node->left = insertNode(node->left, course);
        }

        // Larger course numbers go to the right
        else if (course.courseNumber > node->course.courseNumber) {
            node->right = insertNode(node->right, course);
        }

        return node;
    }
        // Search for a course using its course number
        Node* searchNode(Node * node, string courseNumber) {

            // Stop if the course is found or the tree is empty
            if (node == nullptr ||
                node->course.courseNumber == courseNumber) {
                return node;
            }

            // Search the left side for smaller course numbers
            if (courseNumber < node->course.courseNumber) {
                return searchNode(node->left, courseNumber);
            }

            // Search the right side for larger course numbers
            return searchNode(node->right, courseNumber);
        
    }

public:
    // Constructor starts with an empty tree
    BinarySearchTree() {
        root = nullptr;
    }
    // Destructor releases memory when the BST is destroyed
    ~BinarySearchTree() {
        destroyTree(root);
    }
    // Remove all courses from the Binary Search Tree
    void Clear() {
        destroyTree(root);
        root = nullptr;
    }
    // Check whether a course exists in the BST
    bool CourseExists(string courseNumber) {
        return searchNode(root, courseNumber) != nullptr;
    }

    // Public function to insert a course
    void Insert(Course course) {
        root = insertNode(root, course);
    }
    // Public function to print all courses
    void PrintCourseList() {
        printInOrder(root);
    }
    // Find and display an individual course
    void PrintCourse(string courseNumber) {

        Node* foundCourse = searchNode(root, courseNumber);

        // Display an error if the course does not exist
        if (foundCourse == nullptr) {
            cout << "Course not found." << endl;
            return;
        }

        // Display the course number and title
        cout << foundCourse->course.courseNumber << ", "
            << foundCourse->course.courseName << endl;

        // Display the prerequisites
        if (foundCourse->course.prerequisites.empty()) {
            cout << "Prerequisites: None" << endl;
        }
        else {
            cout << "Prerequisites: ";

            for (size_t i = 0;
                i < foundCourse->course.prerequisites.size(); i++) {

                string prereq = foundCourse->course.prerequisites[i];

                // Find the prerequisite course in the BST
                Node* prereqCourse = searchNode(root, prereq);

                cout << prereq;

                // Include the prerequisite's course title
                if (prereqCourse != nullptr) {
                    cout << " (" << prereqCourse->course.courseName << ")";
                }

                if (i + 1 < foundCourse->course.prerequisites.size()) {
                    cout << ", ";
                }
            }

            cout << endl;
        }
    }
};
// Load course information from a CSV file
bool LoadCourses(string fileName, BinarySearchTree& bst) {

    // Open the CSV file
    ifstream file(fileName);

    // Check whether the file opened successfully
    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }
    // Clear previously loaded courses before loading new data
    bst.Clear();
    string line;
    int courseCount = 0;
    // Track invalid course records
    bool validRecords = true;

    // Read the file one line at a time
    while (getline(file, line)) {

        stringstream ss(line);
        string courseNumber;
        string courseName;

        // Read the course number and name
        getline(ss, courseNumber, ',');
        getline(ss, courseName, ',');

        // Identify incomplete course records
        if (courseNumber.empty() || courseName.empty()) {
            cout << "Error: Invalid course format." << endl;
            validRecords = false;
            continue;
        }

        // Create a new Course object
        Course course;
        course.courseNumber = courseNumber;
        course.courseName = courseName;

        // Read any remaining prerequisites
        string prerequisite;

        while (getline(ss, prerequisite, ',')) {
            if (!prerequisite.empty()) {
                course.prerequisites.push_back(prerequisite);
            }
        }

        // Insert the course into our Binary Search Tree
        bst.Insert(course);
        courseCount++;
    }
    // Validate prerequisites after all courses are loaded
    bool validPrerequisites = true;

    // Return to the beginning of the CSV file
    file.clear();
    file.seekg(0);

    while (getline(file, line)) {
        stringstream ss(line);
        string courseNumber;
        string courseName;
        string prerequisite;

        getline(ss, courseNumber, ',');
        getline(ss, courseName, ',');

        // Check that every prerequisite exists in the BST
        while (getline(ss, prerequisite, ',')) {
            if (!prerequisite.empty() &&
                !bst.CourseExists(prerequisite)) {

                cout << "Error: Invalid prerequisite "
                    << prerequisite << " for "
                    << courseNumber << endl;

                validPrerequisites = false;
            }
        }
    }

    // Reject invalid course records or prerequisites
    if (!validRecords || !validPrerequisites) {
        bst.Clear();
        cout << "Error: Course data failed validation." << endl;
        return false;
    }
    // Close the file
    file.close();

    // Make sure the CSV contained at least one course
    if (courseCount == 0) {
        bst.Clear();
        cout << "Error: No courses were loaded." << endl;
        return false;
    }

    cout << courseCount << " courses loaded successfully." << endl;

    return true;
}
// Main function that controls the course planner
int main() {

    BinarySearchTree bst;
    int choice = 0;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    // Continue displaying the menu until the user exits
    while (choice != 9) {

        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;

        cout << "\nWhat would you like to do? ";

        // Check for invalid input, including letters
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid option. Please enter a number." << endl;
            continue;
        }

        switch (choice) {

        case 1: {
            // Load course information into the BST
            string fileName;

            cout << "Enter the CSV file name: ";
            cin >> ws;
            getline(cin, fileName);

            dataLoaded = LoadCourses(fileName, bst);
            break;
        }

        case 2:
            // Make sure course data has been loaded
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                cout << "\nHere is a sample schedule:\n" << endl;
                bst.PrintCourseList();
            }
            break;

        case 3:
            // Search for a course using its course number
            if (!dataLoaded) {
                cout << "Please load the course data first." << endl;
            }
            else {
                string courseNumber;

                cout << "What course do you want to know about? ";
                cin >> courseNumber;

                // Convert lowercase input to uppercase
                for (char& letter : courseNumber) {
                    letter = toupper(static_cast<unsigned char>(letter));
                }

                bst.PrintCourse(courseNumber);
            }
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            cout << choice << " is not a valid option." << endl;
            break;
        }
    }

    return 0;
}